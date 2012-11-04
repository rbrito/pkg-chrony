/*
  $Header: /cvs/src/chrony/pktlength.h,v 1.2 1999/04/19 20:27:29 richard Exp $

  =======================================================================

  chronyd/chronyc - Programs for keeping computer clocks accurate.

  Copyright (C) 1997-1999 Richard P. Curnow
  All rights reserved.

  For conditions of use, refer to the file LICENCE.

  =======================================================================

  Header for pktlength.c, routines for working out the expected length
  of a network command/reply packet.

  */

#ifndef GOT_PKTLENGTH_H
#define GOT_PKTLENGTH_H

#include "candm.h"

extern int PKL_CommandLength(CMD_Request *r);

extern int PKL_ReplyLength(CMD_Reply *r);

#endif /* GOT_PKTLENGTH_H */
