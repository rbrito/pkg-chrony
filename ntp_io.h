/*
  $Header: /home/richard/myntp/chrony/chrony-1.02/RCS/ntp_io.h,v 1.6 1998/05/20 06:10:59 richard Exp $

  =======================================================================

  chronyd/chronyc - Programs for keeping computer clocks accurate.

  Copyright (C) 1997-1999 Richard P. Curnow
  All rights reserved.

  For conditions of use, refer to the file LICENCE.

  =======================================================================

  This is the header file for the NTP socket I/O bits.

  */

#ifndef GOT_NTP_IO_H
#define GOT_NTP_IO_H

#include "ntp.h"
#include "addressing.h"

/* Function to initialise the module. */
extern void NIO_Initialise(void);

/* Function to finalise the module */
extern void NIO_Finalise(void);

/* Function to transmit a packet */
extern void NIO_SendNormalPacket(NTP_Packet *packet, NTP_Remote_Address *remote_addr);

/* Function to transmit an authenticated packet */
extern void NIO_SendAuthenticatedPacket(NTP_Packet *packet, NTP_Remote_Address *remote_addr);

/* Function to send a datagram to a remote machine's UDP echo port. */
extern void NIO_SendEcho(NTP_Remote_Address *remote_addr);

#endif /* GOT_NTP_IO_H */
