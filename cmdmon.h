/*
  $Header: /cvs/src/chrony/cmdmon.h,v 1.6 1999/04/19 20:27:29 richard Exp $

  =======================================================================

  chronyd/chronyc - Programs for keeping computer clocks accurate.

  Copyright (C) 1997-1999 Richard P. Curnow
  All rights reserved.

  For conditions of use, refer to the file LICENCE.

  =======================================================================

  Header file for the control and monitoring module in the software
  */

#ifndef GOT_CMDMON_H
#define GOT_CMDMON_H

extern void CAM_Initialise(void);

extern void CAM_Finalise(void);

extern int CAM_AddAccessRestriction(unsigned long ip_addr, int subnet_bits, int allow, int all);
extern int CAM_CheckAccessRestriction(unsigned long ip_addr);

#endif /* GOT_CMDMON_H */
