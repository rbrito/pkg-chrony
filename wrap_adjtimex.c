/*
  $Header: /cvs/src/chrony/wrap_adjtimex.c,v 1.9 2002/11/19 21:33:42 richard Exp $

  =======================================================================

  chronyd/chronyc - Programs for keeping computer clocks accurate.

 **********************************************************************
 * Copyright (C) Richard P. Curnow  1997-2002
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 * 
 **********************************************************************

  =======================================================================

  This is a wrapper around the Linux adjtimex system call.  It isolates the
  inclusion of <linux/adjtimex.h> from the need to include other header files,
  many of which conflict with those in <linux/...> on some recent distributions
  (as of Jul 2000) using kernels around 2.2.16 onwards.

  */

#ifdef LINUX

#define _LOOSE_KERNEL_NAMES

#include <sys/timex.h> /* Includes definitions from linux/timex.h */
#include <asm/param.h>

#include "wrap_adjtimex.h"

/* From linux/timex.h
 *
 * The SHIFT_SCALE define establishes the decimal point of the time_phase
 * variable which serves as an extension to the low-order bits of the
 * system clock variable. The SHIFT_UPDATE define establishes the decimal
 * point of the time_offset variable which represents the current offset
 * with respect to standard time. The FINENSEC define represents 1 nsec in
 * scaled units.
 * 
 * SHIFT_USEC defines the scaling (shift) of the time_freq and
 * time_tolerance variables, which represent the current frequency
 * offset and maximum frequency tolerance.
 * 
 * FINENSEC is 1 ns in SHIFT_UPDATE units of the time_phase variable.
 */
#define SHIFT_SCALE 22          /* phase scale (shift) */
#define SHIFT_UPDATE (SHIFT_KG + MAXTC) /* time offset scale (shift) */
#define SHIFT_USEC 16           /* frequency offset scale (shift) */
#define FINENSEC (1L << (SHIFT_SCALE - 10)) /* ~1 ns in phase units */

#define MAXPHASE 512000L        /* max phase error (us) */
#define MAXFREQ (512L << SHIFT_USEC)  /* max frequency error (ppm) */
#define MAXTIME (200L << PPS_AVG) /* max PPS error (jitter) (200 us) */
#define MINSEC 16L              /* min interval between updates (s) */
#define MAXSEC 1200L            /* max interval between updates (s) */
#define NTP_PHASE_LIMIT (MAXPHASE << 5) /* beyond max. dispersion */

/* This doesn't seem to be in any include files !! */

extern int adjtimex(struct timex *);

int
TMX_SetTick(long tick)
{
  struct timex txc;
  txc.modes = ADJ_TICK;
  txc.tick = tick;
  
  return adjtimex(&txc);
}

int
TMX_ApplyOffset(long *offset)
{
  struct timex txc;
  int result;

  txc.modes = ADJ_OFFSET_SINGLESHOT;
  txc.offset = *offset;
  result = adjtimex(&txc);
  *offset = txc.offset;
  return result;
}

int
TMX_SetFrequency(double freq, long tick)
{
  struct timex txc;
  
  txc.modes = ADJ_TICK | ADJ_FREQUENCY | ADJ_STATUS;

  txc.freq = (long)(freq * (double)(1 << SHIFT_USEC));
  txc.tick = tick;
  txc.status = STA_UNSYNC; /* Prevent any of the FLL/PLL stuff coming
                              up */

  return adjtimex(&txc);
}

int
TMX_GetFrequency(double *freq)
{
  struct timex txc;
  int result;
  txc.modes = 0; /* pure read */
  result = adjtimex(&txc);
  *freq = txc.freq / (double)(1 << SHIFT_USEC);
  return result;
}

int
TMX_GetOffsetLeft(long *offset)
{
  struct timex txc;
  int result;
  txc.modes = 0; /* pure read */
  result = adjtimex(&txc);
  *offset = txc.offset;
  return result;
}

int
TMX_ReadCurrentParams(struct tmx_params *params)
{
  struct timex txc;
  int result;
  
  txc.modes = 0; /* pure read */
  result = adjtimex(&txc);

  params->tick     = txc.tick;
  params->offset   = txc.offset;
  params->freq     = txc.freq;
  params->dfreq    = txc.freq / (double)(1 << SHIFT_USEC);
  params->maxerror = txc.maxerror;
  params->esterror = txc.esterror;
  
  params->sta_pll       = (txc.status & STA_PLL);
  params->sta_ppsfreq   = (txc.status & STA_PPSFREQ);
  params->sta_ppstime   = (txc.status & STA_PPSTIME);
  params->sta_fll       = (txc.status & STA_FLL);
  params->sta_ins       = (txc.status & STA_INS);
  params->sta_del       = (txc.status & STA_DEL);
  params->sta_unsync    = (txc.status & STA_UNSYNC);
  params->sta_freqhold  = (txc.status & STA_FREQHOLD);
  params->sta_ppssignal = (txc.status & STA_PPSSIGNAL);
  params->sta_ppsjitter = (txc.status & STA_PPSJITTER);
  params->sta_ppswander = (txc.status & STA_PPSWANDER);
  params->sta_ppserror  = (txc.status & STA_PPSERROR);
  params->sta_clockerr  = (txc.status & STA_CLOCKERR);

  params->constant  = txc.constant;
  params->precision = txc.precision;
  params->tolerance = txc.tolerance;
  params->ppsfreq   = txc.ppsfreq;
  params->jitter    = txc.jitter;
  params->shift     = txc.shift;
  params->stabil    = txc.stabil;
  params->jitcnt    = txc.jitcnt;
  params->calcnt    = txc.calcnt;
  params->errcnt    = txc.errcnt;
  params->stbcnt    = txc.stbcnt;

  return result;
}

#endif

