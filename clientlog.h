/*
  $Header: /cvs/src/chrony/clientlog.h,v 1.5 1999/09/01 21:31:34 richard Exp $

  =======================================================================

  chronyd/chronyc - Programs for keeping computer clocks accurate.

  Copyright (C) 1997-1999 Richard P. Curnow
  All rights reserved.

  For conditions of use, refer to the file LICENCE.

  =======================================================================

  This module contains facilities for logging access by clients.

  */

#ifndef GOT_CLIENTLOG_H
#define GOT_CLIENTLOG_H

#include "sysincl.h"
#include "reports.h"

typedef unsigned long CLG_IP_Addr;

/* Enough to hold flags for 256 hosts in a class C */
typedef unsigned long CLG_Bitmap[8];

extern void CLG_Initialise(void);
extern void CLG_Finalise(void);
extern void CLG_LogNTPClientAccess(CLG_IP_Addr client, time_t now);
extern void CLG_LogNTPPeerAccess(CLG_IP_Addr client, time_t now);

/* When logging command packets, there are several subtypes */

typedef enum {
  CLG_CMD_AUTH,                 /* authenticated */
  CLG_CMD_NORMAL,               /* normal */
  CLG_CMD_BAD_PKT               /* bad version or packet length */
} CLG_Command_Type;

extern void CLG_LogCommandAccess(CLG_IP_Addr client, CLG_Command_Type type, time_t now);

/* And some reporting functions, for use by chronyc. */
/* TBD */

typedef enum {
  CLG_SUCCESS,                  /* All is well */
  CLG_EMPTYSUBNET,              /* No hosts logged in requested subnet */
  CLG_BADSUBNET,                /* Subnet requested is not 0, 8, 16 or 24 bits */
  CLG_INACTIVE,                 /* Facility not active */
  CLG_INDEXTOOLARGE             /* Node index is higher than number of nodes present */
} CLG_Status;

/* For bits=0, 8, 16, flag which immediate subnets of that subnet are
   known.  For bits=24, flag which hosts in that subnet are known.
   Other values, return 0 (failed) */

extern CLG_Status CLG_GetSubnetBitmap(CLG_IP_Addr subnet, int bits, CLG_Bitmap result);

extern CLG_Status
CLG_GetClientAccessReportByIP(unsigned long ip, RPT_ClientAccess_Report *report, time_t now);

CLG_Status
CLG_GetClientAccessReportByIndex(int index, RPT_ClientAccessByIndex_Report *report,
                                 time_t now, unsigned long *n_indices);

/* And an iterating function, to call 'fn' for each client or peer
   that has accessed us since 'since'. */

extern void CLG_IterateNTPClients
(void (*fn)(CLG_IP_Addr client, void *arb),
 void *arb,
 time_t since);


#endif /* GOT_CLIENTLOG_H */
