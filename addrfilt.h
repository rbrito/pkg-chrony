/*
  $Header: /home/richard/myntp/chrony/chrony-1.02/RCS/addrfilt.h,v 1.3 1998/05/20 06:10:39 richard Exp $

  =======================================================================

  chronyd/chronyc - Programs for keeping computer clocks accurate.

  Copyright (C) 1997-1999 Richard P. Curnow
  All rights reserved.

  For conditions of use, refer to the file LICENCE.

  =======================================================================

  Module for providing an authorisation filter on IP addresses
  */

#ifndef GOT_ADDRFILT_H
#define GOT_ADDRFILT_H

typedef struct ADF_AuthTableInst *ADF_AuthTable;

typedef enum {
  ADF_SUCCESS,
  ADF_BADSUBNET
} ADF_Status;
  

/* Create a new table.  The default rule is deny for everything */
extern ADF_AuthTable ADF_CreateTable(void);

/* Allow anything in the supplied subnet, EXCEPT for any more specific
   subnets that are already defined */
extern ADF_Status ADF_Allow(ADF_AuthTable table,
                            unsigned long ip,
                            int subnet_bits);

/* Allow anything in the supplied subnet, overwriting existing
   definitions for any more specific subnets */
extern ADF_Status ADF_AllowAll(ADF_AuthTable table,
                               unsigned long ip,
                               int subnet_bits);

/* Deny anything in the supplied subnet, EXCEPT for any more specific
   subnets that are already defined */
extern ADF_Status ADF_Deny(ADF_AuthTable table,
                           unsigned long ip,
                           int subnet_bits);

/* Deny anything in the supplied subnet, overwriting existing
   definitions for any more specific subnets */
extern ADF_Status ADF_DenyAll(ADF_AuthTable table,
                              unsigned long ip,
                              int subnet_bits);

/* Clear up the table */
extern void ADF_DestroyTable(ADF_AuthTable table);

/* Check whether a given IP address is allowed by the rules in 
   the table */
extern int ADF_IsAllowed(ADF_AuthTable table,
                         unsigned long ip);

#endif /* GOT_ADDRFILT_H */
