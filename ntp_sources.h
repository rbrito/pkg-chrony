/*
  $Header: /home/richard/myntp/chrony/chrony-1.1/RCS/ntp_sources.h,v 1.9 1999/04/19 20:27:29 richard Exp $

  =======================================================================

  chronyd/chronyc - Programs for keeping computer clocks accurate.

  Copyright (C) 1997-1999 Richard P. Curnow
  All rights reserved.

  For conditions of use, refer to the file LICENCE.

  =======================================================================

  Header for the part of the software that deals with the set of
  current NTP servers and peers, which can resolve an IP address into
  a source record for further processing.

  */

#ifndef GOT_NTP_SOURCES_H
#define GOT_NTP_SOURCES_H

#include "ntp.h"
#include "addressing.h"
#include "srcparams.h"
#include "ntp_core.h"
#include "reports.h"

/* Status values returned by operations that indirectly result from user
   input. */
typedef enum {
  NSR_Success, /* Operation successful */
  NSR_NoSuchSource, /* Remove - attempt to remove a source that is not known */
  NSR_AlreadyInUse, /* AddServer, AddPeer - attempt to add a source that is already known */ 
  NSR_TooManySources /* AddServer, AddPeer - too many sources already present */
} NSR_Status;

/* Procedure to add a new server source (to which this machine will be
   a client) */
extern NSR_Status NSR_AddServer(NTP_Remote_Address *remote_addr, SourceParameters *params);

/* Procedure to add a new peer source.  We will use symmetric active
   mode packets when communicating with this source */
extern NSR_Status NSR_AddPeer(NTP_Remote_Address *remote_addr, SourceParameters *params);

/* Procedure to remove a source */
extern NSR_Status NSR_RemoveSource(NTP_Remote_Address *remote_addr);

/* This routine is called by ntp_io when a new packet arrives off the network */
extern void NSR_ProcessReceive(NTP_Packet *message, struct timeval *now, NTP_Remote_Address *remote_addr);

/* This routine is called by ntp_io when a new packet with an authentication tail arrives off the network */
extern void NSR_ProcessAuthenticatedReceive(NTP_Packet *message, struct timeval *now, NTP_Remote_Address *remote_addr);

/* Initialisation function */
extern void NSR_Initialise(void);

/* Finalisation function */
extern void NSR_Finalise(void);

/* This routine is used to indicate that sources whose IP addresses
   match a particular subnet should be set online again.  Returns a
   flag indicating whether any hosts matched the address */
extern int NSR_TakeSourcesOnline(unsigned long mask, unsigned long address);

/* This routine is used to indicate that sources whose IP addresses
   match a particular subnet should be set offline.  Returns a flag
   indicating whether any hosts matched the address */
extern int NSR_TakeSourcesOffline(unsigned long mask, unsigned long address);

extern int NSR_ModifyMinpoll(unsigned long address, int new_minpoll);

extern int NSR_ModifyMaxpoll(unsigned long address, int new_maxpoll);

extern int NSR_ModifyMaxdelay(unsigned long address, double new_max_delay);

extern int NSR_ModifyMaxdelayratio(unsigned long address, double new_max_delay_ratio);

extern int NSR_InitiateSampleBurst(int n_good_samples, int n_total_samples, unsigned long mask, unsigned long address);

extern void NSR_ReportSource(RPT_SourceReport *report, struct timeval *now);

#endif /* GOT_NTP_SOURCES_H */
