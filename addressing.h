/*
  $Header: /cvs/src/chrony/addressing.h,v 1.5 1999/04/19 20:27:29 richard Exp $

  =======================================================================

  chronyd/chronyc - Programs for keeping computer clocks accurate.

  Copyright (C) 1997-1999 Richard P. Curnow
  All rights reserved.

  For conditions of use, refer to the file LICENCE.

  =======================================================================

  Types used for addressing sources etc
  */

#ifndef GOT_ADDRESSING_H
#define GOT_ADDRESSING_H

/* This type is used to represent an IPv4 address and port
   number.  Both parts are in HOST order, NOT network order. */
typedef struct {
  unsigned long ip_addr;
  unsigned short port;
} NTP_Remote_Address;

#if 0
unsigned long NTP_IP_Address;
#endif

#endif /* GOT_ADDRESSING_H */

