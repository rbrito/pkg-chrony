/*
  $Header: /home/richard/myntp/chrony/chrony-1.02/RCS/util.h,v 1.6 1998/07/12 10:24:50 richard Exp $

  =======================================================================

  chronyd/chronyc - Programs for keeping computer clocks accurate.

  Copyright (C) 1997-1999 Richard P. Curnow
  All rights reserved.

  For conditions of use, refer to the file LICENCE.

  =======================================================================

  Various utility functions
  */

#ifndef GOT_UTIL_H
#define GOT_UTIL_H

#include "sysincl.h"

#include "ntp.h"

/* Convert a timeval into a floating point number of seconds */
extern void UTI_TimevalToDouble(struct timeval *a, double *b);

/* Convert a number of seconds expressed in floating point into a
   timeval */
extern void UTI_DoubleToTimeval(double a, struct timeval *b);

/* Returns -1 if a comes earlier than b, 0 if a is the same time as b,
   and +1 if a comes after b */
extern int UTI_CompareTimevals(struct timeval *a, struct timeval *b);

/* Normalise a struct timeval, by adding or subtracting seconds to bring
   its microseconds field into range */
extern void UTI_NormaliseTimeval(struct timeval *x);

/* Calculate result = a - b */
extern void UTI_DiffTimevals(struct timeval *result, struct timeval *a, struct timeval *b);

/* Calculate result = a - b and return as a double */
extern void UTI_DiffTimevalsToDouble(double *result, struct timeval *a, struct timeval *b);

/* Add a double increment to a timeval to get a new one. 'start' is
   the starting time, 'end' is the result that we return.  This is
   safe to use if start and end are the same */
extern void UTI_AddDoubleToTimeval(struct timeval *start, double increment, struct timeval *end);

/* Calculate the average and difference (as a double) of two timevals */
extern void UTI_AverageDiffTimevals(struct timeval *earlier, struct timeval *later, struct timeval *average, double *diff);

/* Convert a timeval into a temporary string, largely for diagnostic
   display */
extern char *UTI_TimevalToString(struct timeval *tv);

/* Convert an NTP timestamp into a temporary string, largely for
   diagnostic display */
extern char *UTI_TimestampToString(NTP_int64 *ts);

/* Convert an IP address to dotted quad notation, for diagnostics */
extern char *UTI_IPToDottedQuad(unsigned long ip);

extern char *UTI_TimeToLogForm(time_t t);

/* Adjust time following a frequency/offset change */
extern void UTI_AdjustTimeval(struct timeval *old_tv, struct timeval *when, struct timeval *new_tv, double dfreq, double doffset);

#if defined (INLINE_UTILITIES)
#define INLINE_STATIC inline static
#include "util.c"
#else
#define INLINE_STATIC
#endif /* defined (INLINE_UTILITIES) */

#endif /* GOT_UTIL_H */
