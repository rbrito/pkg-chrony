/*
  $Header: /home/richard/myntp/chrony/chrony-1.1/RCS/sysincl.h,v 1.3 1999/05/02 20:22:08 richard Exp $

  =======================================================================

  chronyd/chronyc - Programs for keeping computer clocks accurate.

  Copyright (C) 1997-1999 Richard P. Curnow
  All rights reserved.

  For conditions of use, refer to the file LICENCE.

  =======================================================================

  This file includes all system header files that the software
  requires.  This allows us to isolate system dependencies to this file
  alone.
  */

#ifndef GOT_SYSINCL_H
#define GOT_SYSINCL_H

#if defined (SOLARIS) || defined(SUNOS) || defined(LINUX)

#include <alloca.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <float.h>
#include <malloc.h>
#include <math.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdarg.h>
#include <stddef.h>
#include <stddef.h> /* To get definition of offsetof */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>

#endif

#if defined (SOLARIS) || defined(SUNOS)
/* Only needed on these platforms, and doesn't exist on some Linux
   versions. */
#include <nlist.h>
#endif

#if defined (WINNT)

/* Designed to work with the GCC from the GNAT-3.10 for Win32
   distribution */

#define Win32_Winsock
#include <assert.h>
#include <ctype.h>

#if 1
/* Cheat and inline the necessary bits from <errno.h>.  We don't
   include it directly because it redefines some EXXX constants that
   conflict with <windows32/sockets.h> (included by <windows.h>) */

int*	_errno();
int*	__doserrno();

#define	errno		(*_errno())
#define	_doserrno	(*__doserrno())

#define ENOENT 2
#else

#include <errno.h>
#endif


#include <float.h>
#include <math.h>
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <windows.h>
#endif

#endif /* GOT_SYSINCL_H */
