/*
  $Header: /cvs/src/chrony/cmdparse.c,v 1.7 1999/04/19 20:27:29 richard Exp $

  =======================================================================

  chronyd/chronyc - Programs for keeping computer clocks accurate.

  Copyright (C) 1997-1999 Richard P. Curnow
  All rights reserved.

  For conditions of use, refer to the file LICENCE.

  =======================================================================
  
  Module for parsing various forms of directive and command lines that
  are common to the configuration file and to the command client.

  */

#include "sysincl.h"

#include "cmdparse.h"
#include "nameserv.h"

/* ================================================== */

CPS_Status
CPS_ParseNTPSourceAdd(const char *line, CPS_NTP_Source *src)
{
  int ok, n, done;
  char cmd[2048], hostname[2048];
  CPS_Status result;
  
  src->port = 123;
  src->params.minpoll = 6;
  src->params.maxpoll = 10;
  src->params.presend_minpoll = 0;
  src->params.authkey = INACTIVE_AUTHKEY;
  src->params.max_delay = 16.0;
  src->params.max_delay_ratio = 16384.0;
  src->params.online = 1;

  result = CPS_Success;
  
  ok = 0;
  if (sscanf(line, "%s%n", hostname, &n) == 1) {
    src->ip_addr = DNS_Name2IPAddress(hostname);
    if (src->ip_addr != DNS_Failed_Address) {
      ok = 1;
    }
  }

  if (!ok) {
    result = CPS_BadHost;
  } else {

    line += n;
    
    /* Parse subfields */
    ok = 1;
    done = 0;
    do {
      if (sscanf(line, "%s%n", cmd, &n) == 1) {
        
        line += n;
        
        if (!strncasecmp(cmd, "port", 4)) {
          if (sscanf(line, "%hu%n", &src->port, &n) != 1) {
            result = CPS_BadPort;
            ok = 0;
            done = 1;
          } else {
            line += n;
          }
        } else if (!strncasecmp(cmd, "minpoll", 7)) {
          if (sscanf(line, "%d%n", &src->params.minpoll, &n) != 1) {
            result = CPS_BadMinpoll;
            ok = 0;
            done = 1;
          } else {
            line += n;
          }
        } else if (!strncasecmp(cmd, "maxpoll", 7)) {
          if (sscanf(line, "%d%n", &src->params.maxpoll, &n) != 1) {
            result = CPS_BadMaxpoll;
            ok = 0;
            done = 1;
          } else {
            line += n;
          }
        } else if (!strncasecmp(cmd, "presend", 7)) {
          if (sscanf(line, "%d%n", &src->params.presend_minpoll, &n) != 1) {
            result = CPS_BadPresend;
            ok = 0;
            done = 1;
          } else {
            line += n;
          }
          /* This MUST come before the following one ! */
        } else if (!strncasecmp(cmd, "maxdelayratio", 13)) {
          if (sscanf(line, "%lf%n", &src->params.max_delay_ratio, &n) != 1) {
            result = CPS_BadMaxdelayratio;
            ok = 0;
            done = 1;
          } else {
            line += n;
          }
        } else if (!strncasecmp(cmd, "maxdelay", 8)) {
          if (sscanf(line, "%lf%n", &src->params.max_delay, &n) != 1) {
            result = CPS_BadMaxdelay;
            ok = 0;
            done = 1;
          } else {
            line += n;
          }
        } else if (!strncasecmp(cmd, "key", 3)) {
          if (sscanf(line, "%lu%n", &src->params.authkey, &n) != 1) {
            result = CPS_BadKey;
            ok = 0;
            done = 1;
          } else {
            line += n;
          }
        } else if (!strncasecmp(cmd, "offline", 7)) {
          src->params.online = 0;
        } else {
          result = CPS_BadOption;
          ok = 0;
          done = 1;
        }
      } else {
        done = 1;
      }
    } while (!done);
  }

  return result;
    
}

/* ================================================== */

