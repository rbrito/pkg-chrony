/*
  $Header: /home/richard/myntp/chrony/chrony-1.02/RCS/nameserv.h,v 1.5 1998/05/20 06:10:56 richard Exp $

  =======================================================================

  chronyd/chronyc - Programs for keeping computer clocks accurate.

  Copyright (C) 1997-1999 Richard P. Curnow
  All rights reserved.

  For conditions of use, refer to the file LICENCE.

  =======================================================================

  Module header for nameserver functions
  */


#ifndef GOT_NAMESERV_H
#define GOT_NAMESERV_H

static const unsigned long DNS_Failed_Address = 0x0UL;

extern unsigned long DNS_Name2IPAddress(const char *name);

const char *DNS_IPAddress2Name(unsigned long ip_addr);

#endif /* GOT_NAMESERV_H */

