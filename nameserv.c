/*
  $Header: /home/richard/myntp/chrony/chrony-1.1/RCS/nameserv.c,v 1.11 1999/04/19 20:27:29 richard Exp $

  =======================================================================

  chronyd/chronyc - Programs for keeping computer clocks accurate.

  Copyright (C) 1997-1999 Richard P. Curnow
  All rights reserved.

  For conditions of use, refer to the file LICENCE.

  =======================================================================

  Functions to do name to IP address conversion

  */

#include "sysincl.h"

#include "nameserv.h"

/* ================================================== */

unsigned long
DNS_Name2IPAddress(const char *name)
{
  struct hostent *host;
  unsigned char *address0;
  unsigned long result;

  host = gethostbyname(name);
  if (host == NULL) {
    result = DNS_Failed_Address;
  } else {
    address0 = host->h_addr_list[0];
    result = ((((unsigned long)address0[0])<<24) |
	      (((unsigned long)address0[1])<<16) |
	      (((unsigned long)address0[2])<<8) |
	      (((unsigned long)address0[3])));
  }

  return result;

}

/* ================================================== */

const char *
DNS_IPAddress2Name(unsigned long ip_addr)
{
  struct hostent *host;
  static char buffer[16];
  unsigned int a, b, c, d;
  unsigned long addr;

  addr = htonl(ip_addr);
  if (addr == 0UL) {
    /* Catch this as a special case that will never resolve to
       anything */
    strcpy(buffer, "0.0.0.0");
    return buffer;
  } else {
    host = gethostbyaddr((const char *) &addr, sizeof(ip_addr), AF_INET);
    if (!host) {
      a = (ip_addr >> 24) & 0xff;
      b = (ip_addr >> 16) & 0xff;
      c = (ip_addr >>  8) & 0xff;
      d = (ip_addr)       & 0xff;
      snprintf(buffer, 16, "%u.%u.%u.%u", a, b, c, d); /* was sprintf JGH 2/8/99 */
      return buffer;
    } else {
      return host->h_name;
    }
  }
}

/* ================================================== */

