/*
  $Header: /home/richard/myntp/chrony/chrony-1.02/RCS/srcparams.h,v 1.6 1998/05/20 06:11:09 richard Exp $

  =======================================================================

  chronyd/chronyc - Programs for keeping computer clocks accurate.

  Copyright (C) 1997-1999 Richard P. Curnow
  All rights reserved.

  For conditions of use, refer to the file LICENCE.

  =======================================================================

  Header file defining parameters that can be set on a per source basis
  */

#ifndef GOT_SRCPARAMS_H
#define GOT_SRCPARAMS_H

typedef struct {
  int minpoll;
  int maxpoll;
  int online;
  int presend_minpoll;
  unsigned long authkey;
  double max_delay;
  double max_delay_ratio;
} SourceParameters;

#define INACTIVE_AUTHKEY 0UL

#endif /* GOT_SRCPARAMS_H */
