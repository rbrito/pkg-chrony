/*
  $Header: /cvs/src/chrony/strerror.c,v 1.6 1999/04/19 20:27:29 richard Exp $


  =======================================================================

  chronyd/chronyc - Programs for keeping computer clocks accurate.

  Copyright (C) 1997-1999 Richard P. Curnow
  All rights reserved.

  For conditions of use, refer to the file LICENCE.

  =======================================================================

  Replacement strerror function for systems that don't have it
  */

#ifdef SUNOS

#include <errno.h>
extern char *sys_errlist[];

char *strerror(int n) {
  return sys_errlist[n];
}

#endif /* SUNOS */
