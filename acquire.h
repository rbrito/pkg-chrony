/*
  $Header: /home/richard/myntp/chrony/chrony-1.02/RCS/acquire.h,v 1.6 1998/05/20 06:10:38 richard Exp $

  =======================================================================

  chronyd/chronyc - Programs for keeping computer clocks accurate.

  Copyright (C) 1997-1999 Richard P. Curnow
  All rights reserved.

  For conditions of use, refer to the file LICENCE.

  =======================================================================

  Header file for acquisition module
  */

#ifndef GOT_ACQUIRE_H
#define GOT_ACQUIRE_H

typedef struct ACQ_SourceRecord *ACQ_Source;

extern void ACQ_Initialise(void);

extern void ACQ_Finalise(void);

extern void ACQ_StartAcquisition(int n, unsigned long *ip_addrs, int init_slew_threshold,
                                 void (*after_hook)(void *), void *anything);

extern void ACQ_AccumulateSample(ACQ_Source acq_source, double offset, double root_distance);

extern void ACQ_MissedSample(ACQ_Source acq_source);

#endif /* GOT_ACQUIRE_H */
