/*
  $Header: /cvs/src/chrony/sys_linux.c,v 1.24 2000/07/18 06:40:14 richard Exp $

  =======================================================================

  chronyd/chronyc - Programs for keeping computer clocks accurate.

  Copyright (C) 1997-1999 Richard P. Curnow
  All rights reserved.

  For conditions of use, refer to the file LICENCE.

  =======================================================================

  This is the module specific to the Linux operating system.

  */

#ifdef LINUX

#include <sys/time.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <ctype.h>
#include <assert.h>

#include "localp.h"
#include "sys_linux.h"
#include "sched.h"
#include "util.h"
#include "logging.h"
#include "wrap_adjtimex.h"

static long current_tick;

/* This is the value of tick, in seconds, including the current vernier
   frequency term */
static double current_total_tick;

/* This is the uncompensated system tick value */
#define NOMINAL_TICK (10000L)

/* This is the scaling required to go between absolute ppm and the
   scaled ppm used as an argument to adjtimex.  Because chronyd is to
   an extent 'closed loop' it doesn't matter if this is wrongly
   determined, UNLESS the system's ppm error is close to a multiple of
   100, in which case the relationship between changing the frequency
   and changing the value of 'tick' will be wrong.  This would (I
   imagine) cause the system to thrash between two states. */

static double freq_scale;

/* ================================================== */

/* The operating system kernel version */
static int version_major;
static int version_minor;
static int version_patchlevel;

/* Flag indicating whether adjtimex() with txc.modes equal to zero
returns the remaining time adjustment or not.  If not we have to read
the outstanding adjustment by setting it to zero, examining the return
value and setting the outstanding adjustment back again. */

static int have_readonly_adjtime;

/* ================================================== */

static void handle_end_of_slew(void *anything);

/* ================================================== */

inline static int
round(double x) {
  int y;
  y = (int)(x + 0.5);
  while ((double)y < x - 0.5) y++;
  while ((double)y > x + 0.5) y--;
  return y;
}

/* ================================================== */
/* Amount of outstanding offset to process */
static double offset_register;

/* Flag set true if a fast slew (one done by altering tick) is being
   run at the moment */
static int fast_slewing;

/* The amount by which the fast slew was supposed to slew the clock */
static double fast_slew_wanted;

/* The value programmed into the kernel's 'tick' variable whilst
   slewing a large offset */
static long slewing_tick;

/* The timeval (raw) at which a fast slew was started.  We need to
   know this for two reasons.  First, if we want to change the
   frequency midway through, we will want to abort the slew and return
   the unprocessed portion to the offset register to start again
   later.  Second, when the end of the slew expires, we need to know
   precisely how long we have been slewing for, so that we can negate
   the excess and slew it back the other way. */
static struct timeval slew_start_tv;

/* This is the ID returned to use by the scheduler's timeout handler.
   We need this if we subsequently wish to abort a slew, because we will have to
   dequeue the timeout */
static SCH_TimeoutID slew_timeout_id;

/* The adjustment that we apply to 'tick', in seconds, whilst applying
   a fast slew */
static double delta_total_tick;

/* Max amount of time that we wish to slew by using adjtime (or its
   equivalent).  If more than this is outstanding, we alter the value
   of tick instead, for a set period.  Set this according to the
   amount of time that a dial-up clock might need to be shifted
   assuming it is resync'ed about once per day. (TBC) */
#define MAX_ADJUST_WITH_ADJTIME (0.2)

/* The amount by which we alter 'tick' when doing a large slew */
#define SLEW_DELTA_TICK 800

/* ================================================== */
/* This routine stops a fast slew, determines how long the slew has
   been running for, and consequently how much adjustment has actually
   been applied. It can be used both when a slew finishes naturally
   due to a timeout, and when a slew is being aborted. */

static void
stop_fast_slew(void)
{
  struct timeval T1, T1d, T1a;
  struct timezone tz;
  double end_window;
  double fast_slew_done;
  double slew_duration;
  double introduced_dispersion;

  /* Should never get here unless this is true */
  if (!fast_slewing) {
    CROAK("Should be fast slewing");
  }
  
  /* Now set the thing off */
  if (gettimeofday(&T1, &tz) < 0) {
    CROAK("gettimeofday() failed in stop_fast_slew");
  }
  
  if (TMX_SetTick(current_tick) < 0) {
    CROAK("adjtimex() failed in stop_fast_slew");
  }
  
  if (gettimeofday(&T1d, &tz) < 0) {
    CROAK("gettimeofday() failed in stop_fast_slew");
  }

  fast_slewing = 0;

  UTI_AverageDiffTimevals(&T1, &T1d, &T1a, &end_window);
  UTI_DiffTimevalsToDouble(&slew_duration, &T1a, &slew_start_tv);
  
  /* Compute the dispersion we have introduced by changing tick this
     way.  If the two samples of gettimeofday differ, there is an
     uncertainty window wrt when the frequency change actually applies
     from.  We handle this by adding dispersion to all statistics held
     at higher levels in the system. */

  introduced_dispersion = end_window * delta_total_tick;
  lcl_InvokeDispersionNotifyHandlers(introduced_dispersion);

  fast_slew_done = delta_total_tick * slew_duration /
    (current_total_tick + delta_total_tick);

  offset_register += (fast_slew_wanted + fast_slew_done);

}


/* ================================================== */
/* This routine is called to start a clock offset adjustment */

static void
initiate_slew(void)
{
  double dseconds;
  long tick_adjust;
  long offset;
  struct timeval T0, T0d, T0a;
  struct timeval end_of_slew;
  struct timezone tz;
  double start_window;
  double introduced_dispersion;

  /* Don't want to get here if we already have an adjust on the go! */
  if (fast_slewing) {
    CROAK("Should not be fast slewing");
  }

  if (offset_register == 0.0) {
    return;
  }

  if (fabs(offset_register) < MAX_ADJUST_WITH_ADJTIME) {
    /* Use adjtime to do the shift */
    offset = (long)(0.5 + 1.0e6*(-offset_register));

    if (TMX_ApplyOffset(&offset) < 0) {
      CROAK("adjtimex() failed in initiate_slew");
    }

    offset_register = 0.0;

  } else {
    /* Use alteration to tick to do the shift */
    if (offset_register > 0) { /* Need to reduce the increment per tick period */
      tick_adjust = -SLEW_DELTA_TICK;
    } else {
      tick_adjust = +SLEW_DELTA_TICK;
    }

    slewing_tick = current_tick + tick_adjust;

    delta_total_tick = (double) tick_adjust / 1.0e6;
    dseconds = - offset_register * (current_total_tick + delta_total_tick) / delta_total_tick;

    /* Now set the thing off */
    if (gettimeofday(&T0, &tz) < 0) {
      CROAK("gettimeofday() failed in initiate_slew");
    }

    if (TMX_SetTick(slewing_tick) < 0) {
      CROAK("adjtimex() failed to start big slew");
    }

    if (gettimeofday(&T0d, &tz) < 0) {
      CROAK("gettimeofday() failed in initiate_slew");
    }

    /* Now work out the uncertainty in when we actually started the
       slew. */
    
    UTI_AverageDiffTimevals(&T0, &T0d, &T0a, &start_window);

    /* Compute the dispersion we have introduced by changing tick this
    way.  If the two samples of gettimeofday differ, there is an
    uncertainty window wrt when the frequency change actually applies
    from.  We handle this by adding dispersion to all statistics held
    at higher levels in the system. */

    introduced_dispersion = start_window * delta_total_tick;
    lcl_InvokeDispersionNotifyHandlers(introduced_dispersion);

    fast_slewing = 1;
    slew_start_tv = T0a;

    /* Set up timeout for end of slew */
    UTI_AddDoubleToTimeval(&T0a, dseconds, &end_of_slew);

    slew_timeout_id = SCH_AddTimeout(&end_of_slew, handle_end_of_slew, NULL);

    fast_slew_wanted = offset_register;
    offset_register = 0.0;

  }

  return;
}

/* ================================================== */

/* This is the callback routine invoked by the scheduler at the end of
   a slew. */

static void
handle_end_of_slew(void *anything)
{
  stop_fast_slew();
  initiate_slew(); /* To do any fine trimming required */
}

/* ================================================== */
/* This routine is used to abort a slew that is in progress, if any */

static void
abort_slew(void)
{
  if (fast_slewing) {
    stop_fast_slew();
    SCH_RemoveTimeout(slew_timeout_id);
  }
}

/* ================================================== */
/* This routine accrues an offset into the offset register, and starts
   a slew if required.

   The offset argument is measured in seconds.  Positive means the
   clock needs to be slewed backwards (i.e. is currently fast of true
   time) */

static void
accrue_offset(double offset)
{
  long toffset;
  
  /* Add the new offset to the register */
  offset_register += offset;

  /* Cancel any standard adjtime that is running */
  toffset = 0;
  if (TMX_ApplyOffset(&toffset) < 0) {
    CROAK("adjtimex() failed in accrue_offset");
  }

  offset_register -= (double) toffset / 1.0e6;

  if (!fast_slewing) {
    initiate_slew();
  } /* Otherwise, when the fast slew completes, any other stuff
       in the offset register will be applied */

}

/* ================================================== */
/* Positive means currently fast of true time, i.e. jump backwards */

static void
apply_step_offset(double offset)
{
  struct timeval old_time, new_time;
  struct timezone tz;

  if (fast_slewing) {
    abort_slew();
  }

  if (gettimeofday(&old_time, &tz) < 0) {
    CROAK("gettimeofday in apply_step_offset");
  }

  UTI_AddDoubleToTimeval(&old_time, -offset, &new_time);

  if (settimeofday(&new_time, &tz) < 0) {
    CROAK("settimeofday in apply_step_offset");
  }

  initiate_slew();

}

/* ================================================== */
/* This call sets the Linux kernel frequency to a given value in parts
   per million relative to the nominal running frequency.  Nominal is
   taken to be tick=10000, freq=0.  The convention is that this is
   called with a positive argument if the local clock runs fast when
   uncompensated.  */

static void
set_frequency(double freq_ppm) {

  long required_tick;
  double required_freq; /* what we use */
  double scaled_freq; /* what adjtimex & the kernel use */
  int required_delta_tick;
  int neg; /* True if estimate is that local clock runs slow,
              i.e. positive frequency correction required */


  /* If we in the middle of slewing the time by having the value of
     tick altered, we have to stop doing that, because the timeout
     expiry etc will change if we don't. */

  if (fast_slewing) {
    abort_slew();
  }

  if (freq_ppm < 0.0) {
    neg = 1;
    freq_ppm = -freq_ppm;
  } else {
    neg = 0;
  }

  required_delta_tick = round(freq_ppm / 100.0);
  required_freq = freq_ppm - 100.0 * (double) required_delta_tick;

  if (neg) {
    /* Uncompensated local clock runs slow */
    required_tick = NOMINAL_TICK + required_delta_tick;
    scaled_freq = freq_scale * required_freq;
  } else {
    /* Uncompensated local clock runs fast */
    required_tick = NOMINAL_TICK - required_delta_tick;
    scaled_freq = -freq_scale * required_freq;
  }

  if (TMX_SetFrequency(scaled_freq, required_tick) < 0) {
    CROAK("adjtimex failed for set_frequency");
  }

  current_tick = required_tick;
  current_total_tick = ((double)current_tick + required_freq/100.0) / 1.0e6 ;

  initiate_slew(); /* Restart any slews that need to be restarted */

  return;

}

/* ================================================== */
/* Read the ppm frequency from the kernel */

static double
read_frequency(void)
{
  double tick_term;
  double unscaled_freq;
  double freq_term;

  if (TMX_GetFrequency(&unscaled_freq) < 0) {
    CROAK("adjtimex failed in read_frequency");
  }

  /* Use current_tick here rather than txc.tick, otherwise we're
     thrown off course when doing a fast slew (in which case, txc.tick
     is nowhere near the nominal value) */
  tick_term = 100.0 * (double)(NOMINAL_TICK - current_tick);
  freq_term = unscaled_freq / freq_scale;
  
#if 0
  LOG(LOGS_INFO, LOGF_SysLinux, "txc.tick=%ld txc.freq=%ld tick_term=%f freq_term=%f\n",
      txc.tick, txc.freq, tick_term, freq_term);
#endif

  return tick_term - freq_term;

}

/* ================================================== */
/* Given a raw time, determine the correction in seconds to generate
   the 'cooked' time.  The correction has to be added to the
   raw time */

static void
get_offset_correction(struct timeval *raw,
                      double *corr)
{

  /* Correction is given by these things :
     1. Any value in offset register
     2. Amount of fast slew remaining
     3. Any amount of adjtime correction remaining */


  double adjtime_left;
  double fast_slew_duration;
  double fast_slew_achieved;
  double fast_slew_remaining;
  long offset;

  if (have_readonly_adjtime) {
    if (TMX_GetOffsetLeft(&offset) < 0) {
      CROAK("adjtimex() failed in get_offset_correction");
    }
    
    adjtime_left = (double)offset / 1.0e6;
  } else {
    offset = 0;
    if (TMX_ApplyOffset(&offset) < 0) {
      CROAK("adjtimex() failed in get_offset_correction");
    }
    
    adjtime_left = (double)offset / 1.0e6;

    /* txc.offset still set from return value of last call */
    if (TMX_ApplyOffset(&offset) < 0) {
      CROAK("adjtimex() failed in get_offset_correction");
    }
  }

  if (fast_slewing) {
    UTI_DiffTimevalsToDouble(&fast_slew_duration, raw, &slew_start_tv);
    fast_slew_achieved = delta_total_tick * fast_slew_duration /
      (current_total_tick + delta_total_tick);
    fast_slew_remaining = fast_slew_wanted + fast_slew_achieved;
  } else {
    fast_slew_remaining = 0.0;
  }  

  *corr = - (offset_register + fast_slew_remaining) + adjtime_left;

  return;
}

/* ================================================== */

static void
immediate_step(void)
{
  struct timeval old_time, new_time;
  struct timezone tz;
  long offset;

  if (fast_slewing) {
    abort_slew();
  }

  offset = 0;
  if (TMX_ApplyOffset(&offset) < 0) {
    CROAK("adjtimex() failed in immediate_step");
  }

  offset_register -= (double) offset / 1.0e6;

  if (gettimeofday(&old_time, &tz) < 0) {
    CROAK("gettimeofday() failed in immediate_step");
  }

  UTI_AddDoubleToTimeval(&old_time, -offset_register, &new_time);

  if (settimeofday(&new_time, &tz) < 0) {
    CROAK("settimeofday() failed in immediate_step");
  }

  offset_register = 0.0;

  return;
}

/* ================================================== */
/* Compute the scaling to use on any frequency we set, according to
   the vintage of the Linux kernel being used. */

static void
get_version_specific_details(void)
{
  FILE *in;
  char line[1024], *p;
  int major, minor, patch;

  in = fopen("/proc/version", "r");
  if (!in) {
    LOG_FATAL(LOGF_SysLinux, "Cannot open /proc/version to check the kernel version, sorry.\n");
  }

  if (!fgets(line, sizeof(line), in)) {
    LOG_FATAL(LOGF_SysLinux, "Cannot read information from /proc/version, sorry\n");
  }

  p = line;
  while (*p && !isdigit(*p)) p++;

  if (sscanf(p, "%d.%d.%d", &major, &minor, &patch) != 3) {
    LOG_FATAL(LOGF_SysLinux, "Cannot read information from /proc/version, sorry\n");
  }

  LOG(LOGS_INFO, LOGF_SysLinux, "Linux kernel major=%d minor=%d patch=%d\n", major, minor, patch);

  version_major = major;
  version_minor = minor;
  version_patchlevel = patch;
  
  switch (major) {
    case 1:
      switch (minor) {
        case 2:
          if (patch == 13) {
            freq_scale = 128.0 / 100.0; /* I _think_! */
	    have_readonly_adjtime = 1;
          } else {
            LOG_FATAL(LOGF_SysLinux, "Kernel version not supported yet, sorry.\n");
          }
          break;
        case 3:
          /* I guess the change from the 1.2.x scaling to the 2.0.x
             scaling must have happened during 1.3 development.  I
             haven't a clue where though, until someone looks it
             up. */
          LOG_FATAL(LOGF_SysLinux, "Kernel version not supported yet, sorry.\n");
          break;
        default:
          LOG_FATAL(LOGF_SysLinux, "Kernel version not supported yet, sorry.\n");
          break;
      }
      break;
    case 2:
      switch (minor) {
        case 0:
          if (patch < 32) {
            freq_scale = 128.0 / 125.0;
	    have_readonly_adjtime = 1;
          } else if (patch >= 32) {
            freq_scale = 128.0 / 128.125;
	    
	    /* The functionality in kernel/time.c in the kernel source
               was modified with regard to what comes back in the
               txc.offset field on return from adjtimex.  If txc.modes
               was ADJ_OFFSET_SINGLESHOT on entry, the outstanding
               adjustment is returned, however the running offset will
               be modified to the passed value. */
	    have_readonly_adjtime = 0;
          }
          break;
        case 1:
          /* I know that earlier 2.1 kernels were like 2.0.31, hence
             the settings below.  However, the 2.0.32 behaviour may
             have been added late in the 2.1 series, however I have no
             idea at which patch level.  Leave it like this until
             someone supplies some info. */
          freq_scale = 128.0 / 125.0;
          have_readonly_adjtime = 0; /* For safety ! */
          break;
        case 2:
        case 3:
        case 4:
          /* These seem to be like 2.0.32 */
          freq_scale = 128.0 / 128.125;
          have_readonly_adjtime = 0;
          break;
        default:
          LOG_FATAL(LOGF_SysLinux, "Kernel version not supported yet, sorry.\n");
      }
      break;
    default:
      LOG_FATAL(LOGF_SysLinux, "Kernel's major version not supported yet, sorry\n");
      break;
  }


  fclose(in);

}

/* ================================================== */
/* Initialisation code for this module */

void
SYS_Linux_Initialise(void)
{
  current_tick = 10000;
  current_total_tick = 0.01;
  offset_register = 0.0;
  fast_slewing = 0;

  get_version_specific_details();

  lcl_RegisterSystemDrivers(read_frequency, set_frequency,
                            accrue_offset, apply_step_offset,
                            get_offset_correction, immediate_step);
}

/* ================================================== */
/* Finalisation code for this module */

void
SYS_Linux_Finalise(void)
{
  /* Must *NOT* leave a fast slew running - clock would drift way off
     if the daemon is not restarted */
  abort_slew();
}

/* ================================================== */

void
SYS_Linux_GetKernelVersion(int *major, int *minor, int *patchlevel)
{
  *major = version_major;
  *minor = version_minor;
  *patchlevel = version_patchlevel;
}

/* ================================================== */

#endif /* LINUX */
