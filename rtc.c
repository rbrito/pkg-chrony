/*
  $Header: /home/richard/myntp/chrony/chrony-1.02/RCS/rtc.c,v 1.8 1998/07/27 21:24:40 richard Exp $

  =======================================================================

  chronyd/chronyc - Programs for keeping computer clocks accurate.

  Copyright (C) 1997-1999 Richard P. Curnow
  All rights reserved.

  For conditions of use, refer to the file LICENCE.

  =======================================================================

  */

#include "sysincl.h"

#include "rtc.h"
#include "logging.h"
#include "conf.h"

#if defined LINUX
#include "rtc_linux.h"
#endif /* defined LINUX */

/* ================================================== */

static int driver_initialised = 0;

static struct {
  int  (*init)(void);
  void (*fini)(void);
  void (*time_pre_init)(void);
  void (*time_init)(void (*after_hook)(void*), void *anything);
  void (*start_measurements)(void);
  int  (*write_parameters)(void);
  int  (*get_report)(RPT_RTC_Report *report);
  int  (*trim)(void);
  void (*cycle_logfile)(void);
} driver =
{
#if defined LINUX
  RTC_Linux_Initialise,
  RTC_Linux_Finalise,
  RTC_Linux_TimePreInit,
  RTC_Linux_TimeInit,
  RTC_Linux_StartMeasurements,
  RTC_Linux_WriteParameters,
  RTC_Linux_GetReport,
  RTC_Linux_Trim,
  RTC_Linux_CycleLogFile
#else
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL
#endif 
};
     
/* ================================================== */

void
RTC_Initialise(void)
{
  char *file_name;
  int ok;

  /* This is how we tell whether the user wants to load the RTC
     driver, if he is on a machine where it is an option. */
  file_name = CNF_GetRtcFile();

  if (file_name) {
    if (driver.init) {
      if ((driver.init)()) {
        ok = 1;
      } else {
        ok = 0;
      }
    } else {
      ok = 0;
    }

    if (ok) {
      driver_initialised = 1;
    } else {
      driver_initialised = 0;
      LOG(LOGS_ERR, LOGF_Rtc, "Real time clock not supported on this operating system\n");
    }

  } else {
    driver_initialised = 0;
  }

  return;

}

/* ================================================== */

void
RTC_Finalise(void)
{
  if (driver.fini) {
    (driver.fini)();
  }
}

/* ================================================== */
/* Start the processing to get a single measurement from the real time
   clock, and use it to trim the system time, based on knowing the
   drift rate of the RTC and the error the last time we set it.  The
   TimePreInit routine has already run, so we can be sure that the
   trim required is not *too* large.

   We are called with a hook to a function to be called after the
   initialisation is complete.  We also call this if we cannot do the
   initialisation. */

void
RTC_TimeInit(void (*after_hook)(void *), void *anything)
{
  if (driver_initialised) {
    (driver.time_init)(after_hook, anything);
  } else {
    LOG(LOGS_ERR, LOGF_Rtc, "Can't initialise from real time clock, driver not loaded\n");
    (after_hook)(anything);
  }
}

/* ================================================== */
/* Do an initial read of the RTC and set the system time to it.  This
   is analogous to what /sbin/clock -s -u would do on Linux. */

void
RTC_TimePreInit(void)
{
  if (driver.time_pre_init) {
    (driver.time_pre_init)();
  }
}

/* ================================================== */
/* Start the RTC measurement process */

void
RTC_StartMeasurements(void)
{
  if (driver_initialised) {
    (driver.start_measurements)();
  }
  /* Benign if driver not present */
}

/* ================================================== */
/* Write RTC information out to RTC file.  Return 0 for success, 1 if
   RTC driver not running, or 2 if the file cannot be written. */

int
RTC_WriteParameters(void)
{
  if (driver_initialised) {
    return (driver.write_parameters)();
  } else {
    return RTC_ST_NODRV;
  }
}

/* ================================================== */

int
RTC_GetReport(RPT_RTC_Report *report)
{
  if (driver_initialised) {
    return (driver.get_report)(report);
  } else {
    return 0;
  }
}

/* ================================================== */

int
RTC_Trim(void)
{
  if (driver_initialised) {
    return (driver.trim)();
  } else {
    return 0;
  }
}

/* ================================================== */

void
RTC_CycleLogFile(void)
{
  if (driver_initialised) {
    (driver.cycle_logfile)();
  }
}

/* ================================================== */

