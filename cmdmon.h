/*
  $Header: /home/richard/myntp/chrony/chrony-1.02/RCS/cmdmon.h,v 1.5 1998/05/20 06:10:44 richard Exp $

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
