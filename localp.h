/*
  $Header: /home/richard/myntp/chrony/chrony-1.1/RCS/localp.h,v 1.7 1999/04/21 20:16:23 richard Exp $

  =======================================================================

  chronyd/chronyc - Programs for keeping computer clocks accurate.

  Copyright (C) 1997-1999 Richard P. Curnow
  All rights reserved.

  For conditions of use, refer to the file LICENCE.

  =======================================================================

  Private include file for local.c and all system dependent
  driver modules.
  */


#ifndef GOT_LOCALP_H
#define GOT_LOCALP_H

/* System driver to read the current local frequency, in ppm relative
   to nominal.  A positive value indicates that the local clock runs
   fast when uncompensated. */
typedef double (*lcl_ReadFrequencyDriver)(void);

/* System driver to set the current local frequency, in ppm relative
   to nominal.  A positive value indicates that the local clock runs
   fast when uncompensated. */
typedef void (*lcl_SetFrequencyDriver)(double freq_ppm);

/* System driver to accrue an offset. A positive argument means slew
   the clock forwards. */
typedef void (*lcl_AccrueOffsetDriver)(double offset);

/* System driver to apply a step offset. A positive argument means step
   the clock forwards. */
typedef void (*lcl_ApplyStepOffsetDriver)(double offset);

/* System driver to convert a raw time to an adjusted (cooked) time.
   The number of seconds returned in 'corr' have to be added to the
   raw time to get the corrected time */
typedef void (*lcl_OffsetCorrectionDriver)(struct timeval *raw, double *corr);

/* System driver to stop slewing the current offset and to apply is
   as an immediate step instead */
typedef void (*lcl_ImmediateStepDriver)(void);

extern void lcl_InvokeDispersionNotifyHandlers(double dispersion);

extern void
lcl_RegisterSystemDrivers(lcl_ReadFrequencyDriver read_freq,
                          lcl_SetFrequencyDriver set_freq,
                          lcl_AccrueOffsetDriver accrue_offset,
                          lcl_ApplyStepOffsetDriver apply_step_offset,
                          lcl_OffsetCorrectionDriver offset_convert,
                          lcl_ImmediateStepDriver immediate_step_driver);

#endif /* GOT_LOCALP_H */
