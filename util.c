/*
  $Header: /home/richard/myntp/chrony/chrony-1.02/RCS/util.c,v 1.10 1998/07/12 10:45:27 richard Exp $

  =======================================================================

  chronyd/chronyc - Programs for keeping computer clocks accurate.

  Copyright (C) 1997-1999 Richard P. Curnow
  All rights reserved.

  For conditions of use, refer to the file LICENCE.

  =======================================================================

  Various utility functions
  */

#include "sysincl.h"

#include "util.h"
#include "logging.h"

/* ================================================== */

INLINE_STATIC void
UTI_TimevalToDouble(struct timeval *a, double *b)
{
  *b = (double)(a->tv_sec) + 1.0e-6 * (double)(a->tv_usec);

}

/* ================================================== */

INLINE_STATIC void
UTI_DoubleToTimeval(double a, struct timeval *b)
{
  long int_part, frac_part;
  int_part = (long)(a);
  frac_part = (long)(0.5 + 1.0e6 * (a - (double)(int_part)));
  b->tv_sec = int_part;
  b->tv_usec = frac_part;
  UTI_NormaliseTimeval(b);
}

/* ================================================== */

INLINE_STATIC int
UTI_CompareTimevals(struct timeval *a, struct timeval *b)
{
  if (a->tv_sec < b->tv_sec) {
    return -1;
  } else if (a->tv_sec > b->tv_sec) {
    return +1;
  } else {
    assert(a->tv_sec == b->tv_sec);
    if (a->tv_usec < b->tv_usec) {
      return -1;
    } else if (a->tv_usec > b->tv_usec) {
      return +1;
    } else {
      assert(a->tv_usec == b->tv_usec);
      return 0;
    }
  }
  assert(0); /* Shouldn't be able to fall through. */
}

/* ================================================== */

INLINE_STATIC void
UTI_NormaliseTimeval(struct timeval *x)
{
  while (x->tv_usec >= 1000000) {
    ++x->tv_sec;
    x->tv_usec -= 1000000;
  }

  while (x->tv_usec < 0) {
    --x->tv_sec;
    x->tv_usec += 1000000;
  }

}

/* ================================================== */

INLINE_STATIC void
UTI_DiffTimevals(struct timeval *result,
                 struct timeval *a,
                 struct timeval *b)
{
  result->tv_sec  = a->tv_sec  - b->tv_sec;
  result->tv_usec = a->tv_usec - b->tv_usec;

  /* Correct microseconds field to bring it into the range
     [0,1000000) */

  while (result->tv_usec < 0) {
    result->tv_usec += 1000000;
    --result->tv_sec;
  }

  while (result->tv_usec > 999999) {
    result->tv_usec -= 1000000;
    ++result->tv_sec;
  }

  return;
}

/* ================================================== */

/* Calculate result = a - b and return as a double */
INLINE_STATIC void
UTI_DiffTimevalsToDouble(double *result, 
                         struct timeval *a,
                         struct timeval *b)
{
  *result = (double)(a->tv_sec - b->tv_sec) +
    (double)(a->tv_usec - b->tv_usec) * 1.0e-6;
}

/* ================================================== */

INLINE_STATIC void
UTI_AddDoubleToTimeval(struct timeval *start,
                       double increment,
                       struct timeval *end)
{
  long int_part, frac_part;

  /* Don't want to do this by using (long)(1000000 * increment), since
     that will only cope with increments up to +/- 2148 seconds, which
     is too marginal here. */

  int_part = (long) increment;
  frac_part = (long) (0.5 + 1.0e6 * (increment - (double)int_part));

  end->tv_sec  = int_part  + start->tv_sec;
  end->tv_usec = frac_part + start->tv_usec;

  UTI_NormaliseTimeval(end);
}

/* ================================================== */

/* Calculate the average and difference (as a double) of two timevals */
INLINE_STATIC void
UTI_AverageDiffTimevals (struct timeval *earlier,
                         struct timeval *later,
                         struct timeval *average,
                         double *diff)
{
  struct timeval tvdiff;
  struct timeval tvhalf;

  UTI_DiffTimevals(&tvdiff, later, earlier);
  *diff = (double)tvdiff.tv_sec + 1.0e-6 * (double)tvdiff.tv_usec;

  if (*diff < 0.0) {
    /* Either there's a bug elsewhere causing 'earlier' and 'later' to
       be backwards, or something wierd has happened.  Maybe when we
       change the frequency on Linux? */

    /* This seems to be fairly benign, so don't bother logging it */

#if 0
    LOG(LOGS_INFO, LOGF_Util, "Earlier=[%s] Later=[%s]\n",
        UTI_TimevalToString(earlier), UTI_TimevalToString(later));
#endif

    /* Assume the required behaviour is to treat it as zero */
    *diff = 0.0;
  }

#if 0
  assert(*diff >= 0.0); /* Otherwise, the definition of earlier and later
                           is likely to be bogus elsewhere, and we want
                           to know */
#endif

  tvhalf.tv_sec = tvdiff.tv_sec / 2;
  tvhalf.tv_usec = tvdiff.tv_usec / 2 + (tvdiff.tv_sec % 2);
  
  average->tv_sec  = earlier->tv_sec  + tvhalf.tv_sec;
  average->tv_usec = earlier->tv_usec + tvhalf.tv_usec;
  
  /* Bring into range */
  UTI_NormaliseTimeval(average);

  while (average->tv_usec >= 1000000) {
    ++average->tv_sec;
    average->tv_usec -= 1000000;
  }

  while (average->tv_usec < 0) {
    --average->tv_sec;
    average->tv_usec += 1000000;
  }

}

/* ================================================== */

#define POOL_ENTRIES 16
static char buffer_pool[POOL_ENTRIES][64];
static int  pool_ptr = 0;

#define NEXT_BUFFER (buffer_pool[pool_ptr = ((pool_ptr + 1) % POOL_ENTRIES)])

/* ================================================== */
/* Convert a timeval into a temporary string, largely for diagnostic
   display */

char *
UTI_TimevalToString(struct timeval *tv)
{
  char buffer[64], *result;
  struct tm stm;
  stm = *gmtime((time_t *) &(tv->tv_sec));
  strftime(buffer, sizeof(buffer), "%a %x %X", &stm);
  result = NEXT_BUFFER;
  snprintf(result, 64, "%s.%06ld", buffer, (unsigned long)(tv->tv_usec)); /* was sprintf JGH 2/28/99 */
  return result;
}

/* ================================================== */
#define JAN_1970 0x83aa7e80UL

inline static void
int64_to_timeval(NTP_int64 *src,
                 struct timeval *dest)
{
  dest->tv_sec = ntohl(src->hi) - JAN_1970;
  
  /* Until I invent a slick way to do this, just do it the obvious way */
  dest->tv_usec = (int)(0.5 + (double)(ntohl(src->lo)) / 4294.967296);
}

/* ================================================== */
/* Convert an NTP timestamp into a temporary string, largely
   for diagnostic display */

char *
UTI_TimestampToString(NTP_int64 *ts)
{
  struct timeval tv;
  int64_to_timeval(ts, &tv);
  return UTI_TimevalToString(&tv);
}

/* ================================================== */

char *
UTI_IPToDottedQuad(unsigned long ip)
{
  unsigned long a, b, c, d;
  char *result;
  a = (ip>>24) & 0xff;
  b = (ip>>16) & 0xff;
  c = (ip>> 8) & 0xff;
  d = (ip>> 0) & 0xff;
  result = NEXT_BUFFER;
  snprintf(result, 16, "%ld.%ld.%ld.%ld", a, b, c, d); /* was sprintf JGH 2/28/99 */
  return result;
}

/* ================================================== */

char *
UTI_TimeToLogForm(time_t t)
{
  struct tm stm;
  char *result;
  static char *months[] = {"Jan", "Feb", "Mar", "Apr",
                           "May", "Jun", "Jul", "Aug",
                           "Sep", "Oct", "Nov", "Dec"};


  result = NEXT_BUFFER;

  stm = *gmtime(&t);
  snprintf(result, 17, "%2d%s%02d %02d:%02d:%02d", /* was sprintf JGH 2/28/99 */
          stm.tm_mday, months[stm.tm_mon], stm.tm_year % 100,
          stm.tm_hour, stm.tm_min, stm.tm_sec);

  return result;
}

/* ================================================== */

void
UTI_AdjustTimeval(struct timeval *old_tv, struct timeval *when, struct timeval *new_tv, double dfreq, double doffset)
{
  double elapsed, delta_time;

  UTI_DiffTimevalsToDouble(&elapsed, when, old_tv);
  delta_time = elapsed * dfreq - doffset;
  UTI_AddDoubleToTimeval(old_tv, delta_time, new_tv);
}

/* ================================================== */
