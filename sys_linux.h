/*
  $Header: /cvs/src/chrony/sys_linux.h,v 1.6 1999/04/19 20:27:29 richard Exp $

  =======================================================================

  chronyd/chronyc - Programs for keeping computer clocks accurate.

  Copyright (C) 1997-1999 Richard P. Curnow
  All rights reserved.

  For conditions of use, refer to the file LICENCE.

  =======================================================================

  The header file for the linux driver
  */

#ifndef GOT_SYS_LINUX_H
#define GOT_SYS_LINUX_H

extern void SYS_Linux_Initialise(void);

extern void SYS_Linux_Finalise(void);

extern void SYS_Linux_GetKernelVersion(int *major, int *minor, int *patchlevel);

#endif  /* GOT_SYS_LINUX_H */
