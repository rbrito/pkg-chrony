/*
  $Header: /home/richard/myntp/chrony/chrony-1.02/RCS/manual.h,v 1.6 1998/05/20 06:10:53 richard Exp $

  =======================================================================

  chronyd/chronyc - Programs for keeping computer clocks accurate.

  Copyright (C) 1997-1999 Richard P. Curnow
  All rights reserved.

  For conditions of use, refer to the file LICENCE.

  =======================================================================

  Header file for manual time input module.

  */

#ifndef GOT_MANUAL_H
#define GOT_MANUAL_H

#include "sysincl.h"
#include "reports.h"

extern void MNL_Initialise(void);
extern void MNL_Finalise(void);
extern int MNL_AcceptTimestamp(struct timeval *ts, long *offset_cs, double *dfreq_ppm, double *new_afreq_ppm);

extern void MNL_Enable(void);
extern void MNL_Disable(void);
extern void MNL_Reset(void);

extern void MNL_ReportSamples(RPT_ManualSamplesReport *report, int max, int *n);

#endif /* GOT_MANUAL_H */
