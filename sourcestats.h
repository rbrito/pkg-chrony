/*
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
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 * 
 **********************************************************************

  =======================================================================

  Header file for module that deals with the measurements and statistics of
  each of the sources. */

#ifndef GOT_SOURCESTATS_H
#define GOT_SOURCESTATS_H

#include "sysincl.h"

#include "reports.h"

typedef struct SST_Stats_Record *SST_Stats;

/* Init and fini functions */
extern void SST_Initialise(void);
extern void SST_Finalise(void);

/* This function creates a new instance of the statistics handler */
extern SST_Stats SST_CreateInstance(uint32_t refid, IPAddr *addr);

/* This function deletes an instance of the statistics handler. */
extern void SST_DeleteInstance(SST_Stats inst);

/* This function accumulates a single sample into the statistics handler

   sample_time is the epoch at which the sample is to be considered to
   have been made.

   offset is the offset of the local clock relative to the source in
   seconds.  Positive indicates that the local clock if FAST (contrary
   to the NTP parts of the software)

   root_distance is the Lambda+Delta/2 term in RFC1305, but excluding
   the extra dispersion due to the residual standard deviation after
   we have done the regression fit.

   stratum is the stratum of the source from which the sample came.
  */

extern void SST_AccumulateSample(SST_Stats inst, struct timeval *sample_time, double offset, double peer_delay, double peer_dispersion, double root_delay, double root_dispersion, int stratum);

/* This function runs the linear regression operation on the data.  It
   finds the set of most recent samples that give the tightest
   confidence interval for the frequency, and truncates the register
   down to that number of samples. */
extern void SST_DoNewRegression(SST_Stats inst);

/* This function does a simple regression on what is in the register,
   without trying to optimise the error bounds on the frequency by
   deleting old samples */
extern void SST_DoUpdateRegression(SST_Stats inst);

/* Return the assumed worst case range of values that this source's
   frequency lies within.  Frequency is defined as the amount of time
   the local clock gains relative to the source per unit local clock
   time. */
extern void SST_GetFrequencyRange(SST_Stats inst, double *lo, double *hi);

/* Get data needed for selection */
extern void
SST_GetSelectionData(SST_Stats inst, struct timeval *now,
                     int *stratum,
                     double *best_offset, double *best_root_delay,
                     double *best_root_dispersion,
                     double *variance,
                     int *select_ok);

/* Get data needed when setting up tracking on this source */
extern void
SST_GetTrackingData(SST_Stats inst, struct timeval *ref_time,
                    double *average_offset, double *offset_sd,
                    double *frequency, double *skew,
                    double *root_delay, double *root_dispersion);

/* Get parameters for using this source as the reference */
extern void
SST_GetReferenceData(SST_Stats inst, struct timeval *now, 
                     int *stratum, double *offset,
                     double *root_delay, double *root_dispersion,
                     double *frequency, double *skew);



/* This routine is called when the local machine clock parameters are
   changed.  It adjusts all existing samples that we are holding for
   each peer so that it looks like they were made under the new clock
   regime rather than the old one.

   when = cooked local time when the change occurs

   dfreq = delta frequency. positive means the clock has been adjusted
   because it was previously gaining time relative to the external
   reference(s).

   doffset = offset slewed onto local clock.  positive => local clock
   has been made fast by that amount.

*/

extern void SST_SlewSamples(SST_Stats inst, struct timeval *when, double dfreq, double doffset);

/* This routine is called when an indeterminate offset is introduced
   into the local time. */
extern void SST_AddDispersion(SST_Stats inst, double dispersion);

/* Predict the offset of the local clock relative to a given source at
   a given local cooked time. Positive indicates local clock is FAST
   relative to reference. */
extern double SST_PredictOffset(SST_Stats inst, struct timeval *when);

/* Find the minimum round trip delay in the register */
extern double SST_MinRoundTripDelay(SST_Stats inst);

/* This routine determines if a new sample is good enough that it should be
   accumulated */
extern int SST_IsGoodSample(SST_Stats inst, double offset, double delay,
   double max_delay_dev_ratio, double clock_error, struct timeval *when);

extern void SST_SaveToFile(SST_Stats inst, FILE *out);

extern int SST_LoadFromFile(SST_Stats inst, FILE *in);

extern void SST_DoSourceReport(SST_Stats inst, RPT_SourceReport *report, struct timeval *now);

extern void SST_DoSourcestatsReport(SST_Stats inst, RPT_SourcestatsReport *report, struct timeval *now);

typedef enum {
  SST_Skew_Decrease,
  SST_Skew_Nochange,
  SST_Skew_Increase
} SST_Skew_Direction;

extern SST_Skew_Direction SST_LastSkewChange(SST_Stats inst);

extern int SST_Samples(SST_Stats inst);

#endif /* GOT_SOURCESTATS_H */

