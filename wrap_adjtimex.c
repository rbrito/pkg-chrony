/*
  $Header: /cvs/src/chrony/wrap_adjtimex.c,v 1.4 2000/08/08 21:37:24 richard Exp $

  =======================================================================

  chronyd/chronyc - Programs for keeping computer clocks accurate.

  Copyright (C) 1997-1999 Richard P. Curnow
  All rights reserved.

  For conditions of use, refer to the file LICENCE.

  =======================================================================

  This is a wrapper around the Linux adjtimex system call.  It isolates the
  inclusion of <linux/adjtimex.h> from the need to include other header files,
  many of which conflict with those in <linux/...> on some recent distributions
  (as of Jul 2000) using kernels around 2.2.16 onwards.

  */

#ifdef LINUX

#define _LOOSE_KERNEL_NAMES

#include <linux/time.h>
#include <linux/timex.h>

#include "wrap_adjtimex.h"

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

#endif

