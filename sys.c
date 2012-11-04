/*
  $Header: /home/richard/myntp/chrony/chrony-1.1/RCS/sys.c,v 1.8 1999/04/19 20:27:29 richard Exp $

  =======================================================================

  chronyd/chronyc - Programs for keeping computer clocks accurate.

  Copyright (C) 1997-1999 Richard P. Curnow
  All rights reserved.

  For conditions of use, refer to the file LICENCE.

  =======================================================================

  This file contains all the conditionally compiled bits that pull
  in the various operating-system specific modules
  */

#include "sys.h"

#if defined (LINUX)
#include "sys_linux.h"
#endif

#if defined (SOLARIS)
#include "sys_solaris.h"
#endif

#if defined (SUNOS)
#include "sys_sunos.h"
#endif

/* ================================================== */

void
SYS_Initialise(void)
{

#if defined(LINUX)
  SYS_Linux_Initialise();
#endif

#if defined(SOLARIS)
  SYS_Solaris_Initialise();
#endif

#if defined(SUNOS)
  SYS_SunOS_Initialise();
#endif

}

/* ================================================== */

void
SYS_Finalise(void)
{
  
#if defined(LINUX)
  SYS_Linux_Finalise();
#endif

#if defined(SOLARIS)
  SYS_Solaris_Finalise();
#endif

#if defined(SUNOS)
  SYS_SunOS_Finalise();
#endif

  return;
}

/* ================================================== */
/* ================================================== */
/* ================================================== */



