/*
  $Header: /home/richard/myntp/chrony/chrony-1.1/RCS/ntp_core.h,v 1.13 1999/04/19 20:27:29 richard Exp $

  =======================================================================

  chronyd/chronyc - Programs for keeping computer clocks accurate.

  Copyright (C) 1997-1999 Richard P. Curnow
  All rights reserved.

  For conditions of use, refer to the file LICENCE.

  =======================================================================

  Header file for the main NTP protocol engine
  */

#ifndef GOT_NTP_CORE_H
#define GOT_NTP_CORE_H

#include "sysincl.h"

#include "addressing.h"
#include "srcparams.h"
#include "ntp.h"
#include "reports.h"

/* This is a private data type used for storing the instance record for
   each source that we are chiming with */
typedef struct NCR_Instance_Record *NCR_Instance;

/* Init and fini functions */
extern void NCR_Initialise(void);
extern void NCR_Finalise(void);

/* Get a new instance for a server */
extern NCR_Instance NCR_GetServerInstance(NTP_Remote_Address *remote_addr, SourceParameters *params);

/* Get a new instance for a peer */
extern NCR_Instance NCR_GetPeerInstance(NTP_Remote_Address *remote_addr, SourceParameters *params);

/* Destroy an instance */
extern void NCR_DestroyInstance(NCR_Instance instance);

/* This routine is called when a new packet arrives off the network,
   and it relates to a source we have an ongoing protocol exchange with */
extern void NCR_ProcessNoauthKnown(NTP_Packet *message, struct timeval *now, NCR_Instance data);

/* This routine is called when a new packet arrives off the network,
   and we do not recognize its source */
extern void NCR_ProcessNoauthUnknown(NTP_Packet *message, struct timeval *now, NTP_Remote_Address *remote_addr);

/* This routine is called when a new authenticated packet arrives off
   the network, and it relates to a source we have an ongoing protocol
   exchange with */
extern void NCR_ProcessAuthKnown(NTP_Packet *message, struct timeval *now, NCR_Instance data);

/* This routine is called when a new authenticated packet arrives off
   the network, and we do not recognize its source */
extern void NCR_ProcessAuthUnknown(NTP_Packet *message, struct timeval *now, NTP_Remote_Address *remote_addr);

/* Slew receive and transmit times in instance records */
extern void NCR_SlewTimes(NCR_Instance inst, struct timeval *when, double dfreq, double doffset);

/* Take a particular source online (i.e. start sampling it) */
extern void NCR_TakeSourceOnline(NCR_Instance inst);

/* Take a particular source offline (i.e. stop sampling it, without
   marking it unreachable in the source selection stuff) */
extern void NCR_TakeSourceOffline(NCR_Instance inst);

extern void NCR_ModifyMinpoll(NCR_Instance inst, int new_minpoll);

extern void NCR_ModifyMaxpoll(NCR_Instance inst, int new_maxpoll);

extern void NCR_ModifyMaxdelay(NCR_Instance inst, double new_max_delay);

extern void NCR_ModifyMaxdelayratio(NCR_Instance inst, double new_max_delay_ratio);

extern void NCR_InitiateSampleBurst(NCR_Instance inst, int n_good_samples, int n_total_samples);

extern void NCR_ReportSource(NCR_Instance inst, RPT_SourceReport *report, struct timeval *now);

extern int NCR_AddAccessRestriction(unsigned long ip_addr, int subnet_bits, int allow, int all);
extern int NCR_CheckAccessRestriction(unsigned long ip_addr);

extern void NCR_CycleLogFile(void);

#endif /* GOT_NTP_CORE_H */
