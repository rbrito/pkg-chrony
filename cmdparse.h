/*
  $Header: /cvs/src/chrony/cmdparse.h,v 1.5 1999/04/19 20:27:29 richard Exp $

  =======================================================================

  chronyd/chronyc - Programs for keeping computer clocks accurate.

  Copyright (C) 1997-1999 Richard P. Curnow
  All rights reserved.

  For conditions of use, refer to the file LICENCE.

  =======================================================================

  Header file for the command parser
  */

#ifndef GOT_CMDPARSE_H
#define GOT_CMDPARSE_H

#include "srcparams.h"

typedef enum {
  CPS_Success,
  CPS_BadOption,
  CPS_BadHost,
  CPS_BadPort,
  CPS_BadMinpoll,
  CPS_BadMaxpoll,
  CPS_BadPresend,
  CPS_BadMaxdelayratio,
  CPS_BadMaxdelay,
  CPS_BadKey
} CPS_Status;

typedef struct {
  unsigned long ip_addr;
  unsigned short port;
  SourceParameters params;
} CPS_NTP_Source;

/* Parse a command to add an NTP server or peer */
extern CPS_Status CPS_ParseNTPSourceAdd(const char *line, CPS_NTP_Source *src);
  


#endif /* GOT_CMDPARSE_H */
