/*
  $Header: /cvs/src/chrony/wrap_adjtimex.h,v 1.1 2000/07/15 21:20:07 richard Exp $

  =======================================================================

  chronyd/chronyc - Programs for keeping computer clocks accurate.

  Copyright (C) 1997-1999 Richard P. Curnow
  All rights reserved.

  For conditions of use, refer to the file LICENCE.

  =======================================================================

  The header file for the adjtimex wrapper
  */

#ifndef GOT_WRAP_ADJTIMEX_H
#define GOT_WRAP_ADJTIMEX_H

int TMX_SetTick(long tick);
int TMX_ApplyOffset(long *offset);
int TMX_SetFrequency(double freq, long tick);
int TMX_GetFrequency(double *freq);
int TMX_GetOffsetLeft(long *offset);

#endif  /* GOT_WRAP_ADJTIMEX_H */

