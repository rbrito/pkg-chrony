/*
  $Header: /home/richard/myntp/chrony/chrony-1.02/RCS/sched.h,v 1.5 1998/05/20 06:11:06 richard Exp $

  =======================================================================

  chronyd/chronyc - Programs for keeping computer clocks accurate.

  Copyright (C) 1997-1999 Richard P. Curnow
  All rights reserved.

  For conditions of use, refer to the file LICENCE.

  =======================================================================

  Exported header file for sched.c
  */

#ifndef GOT_SCHED_H
#define GOT_SCHED_H

#include "sysincl.h"

typedef unsigned long SCH_TimeoutID;

typedef unsigned long SCH_TimeoutClass;
static const SCH_TimeoutClass SCH_ReservedTimeoutValue = 0;
static const SCH_TimeoutClass SCH_NtpSamplingClass = 1;

typedef void* SCH_ArbitraryArgument;
typedef void (*SCH_FileHandler)(SCH_ArbitraryArgument);
typedef void (*SCH_TimeoutHandler)(SCH_ArbitraryArgument);

/* Exported functions */

/* Initialisation function for the module */
extern void SCH_Initialise(void);

/* Finalisation function for the module */
extern void SCH_Finalise(void);

/* Register a handler for when select goes true on a file descriptor */
extern void SCH_AddInputFileHandler
(int fd,                        /* The file descriptor */
 SCH_FileHandler,               /* The handler routine */
 SCH_ArbitraryArgument          /* An arbitrary passthrough argument to the handler */
);
extern void SCH_RemoveInputFileHandler(int fd);

/* This queues a timeout to elapse at a given (raw) local time */
extern SCH_TimeoutID SCH_AddTimeout(struct timeval *tv, SCH_TimeoutHandler, SCH_ArbitraryArgument);

/* This queues a timeout to elapse at a given delta time relative to the current (raw) time */
extern SCH_TimeoutID SCH_AddTimeoutByDelay(double delay, SCH_TimeoutHandler, SCH_ArbitraryArgument);

/* This queues a timeout in a particular class, ensuring that the
   expiry time is at least a given separation away from any other
   timeout in the same class */
extern SCH_TimeoutID SCH_AddTimeoutInClass(double min_delay, double separation,
                                           SCH_TimeoutClass class,
                                           SCH_TimeoutHandler handler, SCH_ArbitraryArgument);

/* The next one probably ought to return a status code */
extern void SCH_RemoveTimeout(SCH_TimeoutID);

extern void SCH_MainLoop(void);

extern void SCH_QuitProgram(void);

#endif /* GOT_SCHED_H */
