/*
  $Header: /home/richard/myntp/chrony/chrony-1.02/RCS/conf.c,v 1.25 1998/06/22 05:51:06 richard Exp $

  =======================================================================

  chronyd/chronyc - Programs for keeping computer clocks accurate.

  Copyright (C) 1997-1999 Richard P. Curnow
  All rights reserved.

  For conditions of use, refer to the file LICENCE.

  =======================================================================

  Module that reads and processes the configuration file.

  */

#include "sysincl.h"

#include "conf.h"
#include "ntp_sources.h"
#include "ntp_core.h"
#include "cmdmon.h"
#include "srcparams.h"
#include "logging.h"
#include "nameserv.h"
#include "memory.h"
#include "acquire.h"
#include "cmdparse.h"

/* ================================================== */

/* Was "/etc/chrony.conf" JGH Sun Nov  8 18:11:11 CST 1998 */
#define DEFAULT_CONF_FILE "/etc/chrony/chrony.conf"

/* ================================================== */
/* Forward prototypes */

static void parse_commandkey(const char *);
static void parse_driftfile(const char *);
static void parse_dumpdir(const char *);
static void parse_dumponexit(const char *);
static void parse_keyfile(const char *);
static void parse_rtcfile(const char *);
static void parse_log(const char *);
static void parse_logdir(const char *);
static void parse_maxupdateskew(const char *);
static void parse_peer(const char *);
static void parse_port(const char *);
static void parse_server(const char *);
static void parse_local(const char *);
static void parse_manual(const char *);
static void parse_initstepslew(const char *);
static void parse_allow(const char *);
static void parse_deny(const char *);
static void parse_cmdallow(const char *);
static void parse_cmddeny(const char *);
static void parse_cmdport(const char *);
static void parse_rtconutc(const char *);
static void parse_noclientlog(const char *);
static void parse_logchange(const char *);
static void parse_mailonchange(const char *);
static void parse_bindaddress(const char *);

/* ================================================== */
/* Configuration variables */

static int ntp_port = 123;
static char *keys_file = NULL;
static char *drift_file = NULL;
static char *rtc_file = NULL;
static unsigned long command_key_id;
static double max_update_skew = DBL_MAX;

static int cmd_port = -1;

static int do_log_measurements = 0;
static int do_log_statistics = 0;
static int do_log_tracking = 0;
static int do_log_rtc = 0;
static int do_dump_on_exit = 0;
static char *logdir = ".";
static char *dumpdir = ".";

static int enable_local=0;
#define DEFAULT_LOCAL_STRATUM 8
static int local_stratum;

static int do_init_stepslew = 0;
static int n_init_srcs;

/* Threshold (in seconds) - if absolute value of initial error is less
   than this, slew instead of stepping */
static int init_slew_threshold = -1;
#define MAX_INIT_SRCS 8
static unsigned long init_srcs_ip[MAX_INIT_SRCS];

static int enable_manual=0;

/* Flag set if the RTC runs UTC (default is it runs local time
   incl. daylight saving). */
static int rtc_on_utc = 0;

/* Flag set if we should log to syslog when a time adjustment
   exceeding the threshold is initiated */
static int do_log_change = 0;
static double log_change_threshold = 0.0;

static char *mail_user_on_change = NULL;
static double mail_change_threshold = 0.0;

/* Flag indicating that we don't want to log clients, e.g. to save
   memory */
static int no_client_log = 0;

/* IP address (host order) for binding to.  0 means INADDR_ANY will be
   used */
static unsigned long bind_address = 0UL;

/* ================================================== */

typedef struct {
  char *keyword;
  int len;
  void (*handler)(const char *);
} Command;

static const Command commands[] = {
  {"server", 6, parse_server},
  {"peer", 4, parse_peer},
  {"port", 4, parse_port},
  {"driftfile", 9, parse_driftfile},
  {"keyfile", 7, parse_keyfile},
  {"rtcfile", 7, parse_rtcfile},
  {"logdir", 6, parse_logdir},
  {"log", 3, parse_log},
  {"dumponexit", 10, parse_dumponexit},
  {"dumpdir", 7, parse_dumpdir},
  {"maxupdateskew", 13, parse_maxupdateskew},
  {"commandkey", 10, parse_commandkey},
  {"initstepslew", 12, parse_initstepslew},
  {"local", 5, parse_local},
  {"manual", 6, parse_manual},
  {"allow", 5, parse_allow},
  {"deny", 4, parse_deny},
  {"cmdallow", 8, parse_cmdallow},
  {"cmddeny", 7, parse_cmddeny},
  {"cmdport", 7, parse_cmdport},
  {"rtconutc", 8, parse_rtconutc},
  {"noclientlog", 11, parse_noclientlog},
  {"logchange", 9, parse_logchange},
  {"mailonchange", 12, parse_mailonchange},
  {"bindaddress", 11, parse_bindaddress}
};

static int n_commands = (sizeof(commands) / sizeof(commands[0]));

/* The line number in the configuration file being processed */
static int line_number;

/* ================================================== */

typedef enum {
  SERVER, PEER
} NTP_Source_Type;

typedef struct {
  NTP_Source_Type type;
  unsigned long ip_addr;
  unsigned short port;
  SourceParameters params;
} NTP_Source;

#define MAX_NTP_SOURCES 64

static NTP_Source ntp_sources[MAX_NTP_SOURCES];
static int n_ntp_sources = 0;

/* ================================================== */

typedef struct _AllowDeny {
  struct _AllowDeny *next;
  struct _AllowDeny *prev;
  unsigned long ip;
  int subnet_bits;
  int all; /* 1 to override existing more specific defns */
  int allow; /* 0 for deny, 1 for allow */
} AllowDeny;

static AllowDeny ntp_auth_list = {&ntp_auth_list, &ntp_auth_list};
static AllowDeny cmd_auth_list = {&cmd_auth_list, &cmd_auth_list};

/* ================================================== */

/* Read the configuration file */
void
CNF_ReadFile(const char *filename)
{
  FILE *in;
  char line[2048];
  char *p;
  int i, ok;

  if (filename == NULL) {
    filename = DEFAULT_CONF_FILE;
  }

  in = fopen(filename, "r");
  if (!in) {
    LOG(LOGS_ERR, LOGF_Configure, "Could not open configuration file [%s]\n", filename);
  } else {

    line_number = 0;

    /* Success */
    while (fgets(line, sizeof(line), in)) {

      line_number++;

      /* Strip trailing newline */
      line[strlen(line) - 1] = 0;

      /* Discard comment lines, blank lines etc */
      p = line;
      while(*p && (isspace(*p)))
        p++;

      if (!*p || (strchr("!;#%", *p) != NULL))
        continue;

      /* We have a real line, now try to match commands */
      ok = 0;
      for (i=0; i<n_commands; i++) {
        if (!strncasecmp(commands[i].keyword, p, commands[i].len)) {
          (*(commands[i].handler))(p + commands[i].len);
          ok = 1;
        }
      }      

      if (!ok) {
        LOG(LOGS_WARN, LOGF_Configure, "Line %d in configuration file [%s] contains invalid command\n",
            line_number, filename);
      }

    }

    fclose(in);
  }

}

/* ================================================== */

static void
parse_source(const char *line, NTP_Source_Type type)
{
  int i;
  NTP_Source s;
  CPS_Status status;
  CPS_NTP_Source params;

  s.type = type;
  status = CPS_ParseNTPSourceAdd(line, &params);

  switch (status) {
    case CPS_Success:
      s.port = params.port;
      s.ip_addr = params.ip_addr;
      s.params = params.params;

      i = n_ntp_sources++;
      ntp_sources[i] = s;

      break;
    case CPS_BadOption:
      LOG(LOGS_WARN, LOGF_Configure, "Unrecognized subcommand at line %d\n", line_number);
      break;
    case CPS_BadHost:
      LOG(LOGS_WARN, LOGF_Configure, "Invalid host/IP address at line %d\n", line_number);
      break;
    case CPS_BadPort:
      LOG(LOGS_WARN, LOGF_Configure, "Unreadable port number at line %d\n", line_number);
      break;
    case CPS_BadMinpoll:
      LOG(LOGS_WARN, LOGF_Configure, "Unreadable minpoll value at line %d\n", line_number);
      break;
    case CPS_BadMaxpoll:
      LOG(LOGS_WARN, LOGF_Configure, "Unreadable maxpoll value at line %d\n", line_number);
      break;
    case CPS_BadPresend:
      LOG(LOGS_WARN, LOGF_Configure, "Unreadable presend value at line %d\n", line_number);
      break;
    case CPS_BadMaxdelayratio:
      LOG(LOGS_WARN, LOGF_Configure, "Unreadable max delay ratio value at line %d\n", line_number);
      break;
    case CPS_BadMaxdelay:
      LOG(LOGS_WARN, LOGF_Configure, "Unreadable max delay value at line %d\n", line_number);
      break;
    case CPS_BadKey:
      LOG(LOGS_WARN, LOGF_Configure, "Unreadable key value at line %d\n", line_number);
      break;
  }

  return;

}

/* ================================================== */

static void
parse_server(const char *line)
{
  parse_source(line, SERVER);
}

/* ================================================== */

static void
parse_peer(const char *line)
{
  parse_source(line, PEER);
}

/* ================================================== */

static void
parse_port(const char *line)
{
  if (sscanf(line, "%d", &ntp_port) != 1) {
    LOG(LOGS_WARN, LOGF_Configure, "Could not read port number at line %d in file\n", line_number);
  }
}

/* ================================================== */

static void
parse_maxupdateskew(const char *line)
{
  if (sscanf(line, "%lf", &max_update_skew) != 1) {
    LOG(LOGS_WARN, LOGF_Configure, "Could not read max update skew at line %d in file\n", line_number);
  }
}

/* ================================================== */

static void
parse_driftfile(const char *line)
{
  /* This must allocate enough space! */
  drift_file = MallocArray(char, 1 + strlen(line));
  sscanf(line, "%s", drift_file);
}

/* ================================================== */

static void
strip_trailing_spaces(char *p)
{
  char *q;
  for (q=p; *q; q++)
    ;
  
  for (q--; isspace(*q); q--)
    ;

  *++q = 0;
}

/* ================================================== */

static void
parse_keyfile(const char *line)
{
  /* This must allocate enough space! */
  keys_file = MallocArray(char, 1 + strlen(line));
  sscanf(line, "%s", keys_file);
  strip_trailing_spaces(keys_file);
}

/* ================================================== */

static void
parse_rtcfile(const char *line)
{
  rtc_file = MallocArray(char, 1 + strlen(line));
  sscanf(line, "%s", rtc_file);
  strip_trailing_spaces(rtc_file);
}  

/* ================================================== */

static void
parse_logdir(const char *line)
{
  logdir = MallocArray(char, 1 + strlen(line));
  sscanf(line, "%s", logdir);
}

/* ================================================== */

static void
parse_dumpdir(const char *line)
{
  dumpdir = MallocArray(char, 1 + strlen(line));
  sscanf(line, "%s", dumpdir);
}

/* ================================================== */

static void
parse_dumponexit(const char *line)
{
  do_dump_on_exit = 1;
}

/* ================================================== */

static void
parse_log(const char *line)
{
  do {
    while (*line && isspace(*line)) line++;
    if (*line) {
      if (!strncmp(line, "measurements", 12)) {
        do_log_measurements = 1;
        line += 12;
      } else if (!strncmp(line, "statistics", 10)) {
        do_log_statistics = 1;
        line += 10;
      } else if (!strncmp(line, "tracking", 8)) {
        do_log_tracking = 1;
        line += 8;
      } else if (!strncmp(line, "rtc", 3)) {
        do_log_rtc = 1;
        line += 3;
      } else {
        break;
      }
    } else {
      break;
    }
  } while (1);
}

/* ================================================== */

static void
parse_commandkey(const char *line)
{
  if (sscanf(line, "%lu", &command_key_id) != 1) {
    LOG(LOGS_WARN, LOGF_Configure, "Could not read command key ID at line %d\n", line_number);
  }
}

/* ================================================== */

static void
parse_local(const char *line)
{
  int stratum;
  enable_local = 1;
  if (sscanf(line, "%*[ \t]stratum%d", &stratum) == 1) {
    local_stratum = stratum;
  } else {
    local_stratum = DEFAULT_LOCAL_STRATUM;
  }
}

/* ================================================== */

static void
parse_cmdport(const char *line)
{
  if (sscanf(line, "%d", &cmd_port) != 1) {
    LOG(LOGS_WARN, LOGF_Configure, "Could not read command port number at line %d\n", line_number);
  }
}

/* ================================================== */

static void
parse_initstepslew(const char *line)
{
  const char *p;
  char hostname[256];
  int n;
  unsigned long ip_addr;

  do_init_stepslew = 1;
  n_init_srcs = 0;
  p = line;

  if (sscanf(p, "%d%n", &init_slew_threshold, &n) == 1) {
    p += n;
  } else {
    LOG(LOGS_WARN, LOGF_Configure, "Could not parse initstepslew threshold at line %d\n", line_number);
    init_slew_threshold = -1;
    return;
  }
    
  while (*p) {
    if (sscanf(p, "%s%n", hostname, &n) == 1) {
      ip_addr = DNS_Name2IPAddress(hostname);
      if (ip_addr != DNS_Failed_Address) {
        init_srcs_ip[n_init_srcs] = ip_addr;
        ++n_init_srcs;
      }
      
      if (n_init_srcs >= MAX_INIT_SRCS) {
        return;
      }

    } else {
      /* If we get invalid trailing syntax, forget it ... */
      return;
    }

    p += n;
  }

}

/* ================================================== */

static void
parse_manual(const char *line)
{
  enable_manual = 1;
}

/* ================================================== */

static void
parse_rtconutc(const char *line)
{
  rtc_on_utc = 1;
}

/* ================================================== */

static void
parse_noclientlog(const char *line)
{
  no_client_log = 1;
}

/* ================================================== */

static void
parse_logchange(const char *line)
{
  if (sscanf(line, "%lf", &log_change_threshold) == 1) {
    do_log_change = 1;
  } else {
    do_log_change = 0;
    LOG(LOGS_WARN, LOGF_Configure,
        "Could not read threshold for logging clock changes at line %d\n",
        line_number);
  }
}


/* ================================================== */

static void
parse_mailonchange(const char *line)
{
  char buffer[128];
  if (sscanf(line, "%s%lf", buffer, &mail_change_threshold) == 2) {
    mail_user_on_change = MallocArray(char, strlen(buffer)+1);
    strcpy(mail_user_on_change, buffer);
  } else {
    mail_user_on_change = NULL;
    LOG(LOGS_WARN, LOGF_Configure,
        "Could not read user or threshold for clock change mail notify at line %d\n",
        line_number);
  }
}

/* ================================================== */

static void
parse_allow_deny(const char *line, AllowDeny *list, int allow)
{
  const char *p;
  unsigned long a, b, c, d, n;
  int all = 0;
  AllowDeny *new_node = NULL;
  unsigned long ip_addr;

  p = line;

  while (*p && isspace(*p)) p++;

  if (!strncmp(p, "all", 3)) {
    all = 1;
    p += 3;
  }

  while (*p && isspace(*p)) p++;
  if (!*p) {
    /* Empty line applies to all addresses */
    new_node = MallocNew(AllowDeny);
    new_node->allow = allow;
    new_node->all = all;
    new_node->ip = 0UL;
    new_node->subnet_bits = 0;
  } else {

    n = sscanf(p, "%lu.%lu.%lu.%lu", &a, &b, &c, &d);
   
    if (n >= 1) {
      new_node = MallocNew(AllowDeny);
      new_node->allow = allow;
      new_node->all = all;

      a &= 0xff;
      b &= 0xff;
      c &= 0xff;
      d &= 0xff;
      
      switch (n) {
        case 1:
          new_node->ip = (a<<24);
          new_node->subnet_bits = 8;
          break;
        case 2:
          new_node->ip = (a<<24) | (b<<16);
          new_node->subnet_bits = 16;
          break;
        case 3:
          new_node->ip = (a<<24) | (b<<16) | (c<<8);
          new_node->subnet_bits = 24;
          break;
        case 4:
          new_node->ip = (a<<24) | (b<<16) | (c<<8) | d;
          new_node->subnet_bits = 32;
          break;
        default:
          assert(0);
          
      }
    
    } else {
      ip_addr = DNS_Name2IPAddress(p);
      if (ip_addr != DNS_Failed_Address) {
        new_node = MallocNew(AllowDeny);
        new_node->allow = allow;
        new_node->all = all;
        new_node->ip = ip_addr;
        new_node->subnet_bits = 32;
      } else {
        LOG(LOGS_WARN, LOGF_Configure, "Could not read address at line %d\n", line_number);
      }      
    }
  }
  
  if (new_node) {
    new_node->prev = list->prev;
    new_node->next = list;
    list->prev->next = new_node;
    list->prev = new_node;
  }

}
  

/* ================================================== */

static void
parse_allow(const char *line)
{
  parse_allow_deny(line, &ntp_auth_list, 1);
}


/* ================================================== */

static void
parse_deny(const char *line)
{
  parse_allow_deny(line, &ntp_auth_list, 0);
}

/* ================================================== */

static void
parse_cmdallow(const char *line)
{
  parse_allow_deny(line, &cmd_auth_list, 1);
}


/* ================================================== */

static void
parse_cmddeny(const char *line)
{
  parse_allow_deny(line, &cmd_auth_list, 0);
}

/* ================================================== */

static void
parse_bindaddress(const char *line)
{
  unsigned long a, b, c, d;
  int n;
  n = sscanf(line, "%lu.%lu.%lu.%lu", &a, &b, &c, &d);
  if (n == 4) {
    bind_address = (((a&0xff)<<24) | ((b&0xff)<<16) | 
                    ((c&0xff)<<8) | (d&0xff));
  } else {
    LOG(LOGS_WARN, LOGF_Configure, "Could not read bind address at line %d\n", line_number);
  }    
}

/* ================================================== */

void
CNF_ProcessInitStepSlew(void (*after_hook)(void *), void *anything)
{
  if (do_init_stepslew) {
    ACQ_StartAcquisition(n_init_srcs, init_srcs_ip, init_slew_threshold, after_hook, anything);
  } else {
    (after_hook)(anything);
  }
}

/* ================================================== */

void
CNF_AddSources(void) {
  NTP_Remote_Address server;
  int i;

  for (i=0; i<n_ntp_sources; i++) {
    server.ip_addr = ntp_sources[i].ip_addr;
    server.port = ntp_sources[i].port;

    switch (ntp_sources[i].type) {
      case SERVER:
        NSR_AddServer(&server, &ntp_sources[i].params);
        break;

      case PEER:
        NSR_AddPeer(&server, &ntp_sources[i].params);
        break;
    }

  }

  return;

}

/* ================================================== */

unsigned short
CNF_GetNTPPort(void)
{
  return ntp_port;
}

/* ================================================== */

char *
CNF_GetDriftFile(void)
{
  return drift_file;
}

/* ================================================== */

char *
CNF_GetLogDir(void)
{
  return logdir;
}

/* ================================================== */

char *
CNF_GetDumpDir(void)
{
  return dumpdir;
}

/* ================================================== */

int
CNF_GetLogMeasurements(void)
{
  return do_log_measurements;
}

/* ================================================== */

int
CNF_GetLogStatistics(void)
{
  return do_log_statistics;
}

/* ================================================== */

int
CNF_GetLogTracking(void)
{
  return do_log_tracking;
}

/* ================================================== */

int
CNF_GetLogRtc(void)
{
  return do_log_rtc;
}

/* ================================================== */

char *
CNF_GetKeysFile(void)
{
  return keys_file;
}

/* ================================================== */

char *
CNF_GetRtcFile(void)
{
  return rtc_file;
}

/* ================================================== */

unsigned long
CNF_GetCommandKey(void)
{
  return command_key_id;
}

/* ================================================== */

int
CNF_GetDumpOnExit(void)
{
  return do_dump_on_exit;
}

/* ================================================== */

double
CNF_GetMaxUpdateSkew(void)
{
  return max_update_skew;
}

/* ================================================== */

int
CNF_GetManualEnabled(void)
{
  return enable_manual;
}

/* ================================================== */

int
CNF_GetCommandPort(void) {
  return cmd_port;
}

/* ================================================== */

int
CNF_AllowLocalReference(int *stratum)
{
  if (enable_local) {
    *stratum = local_stratum;
    return 1;
  } else {
    return 0;
  }
}

/* ================================================== */

int
CNF_GetRTCOnUTC(void)
{
  return rtc_on_utc;
}

/* ================================================== */

void
CNF_GetLogChange(int *enabled, double *threshold)
{
  *enabled = do_log_change;
  *threshold = log_change_threshold;
}

/* ================================================== */

void
CNF_GetMailOnChange(int *enabled, double *threshold, char **user)
{
  if (mail_user_on_change) {
    *enabled = 1;
    *threshold = mail_change_threshold;
    *user = mail_user_on_change;
  } else {
    *enabled = 0;
    *threshold = 0.0;
    *user = NULL;
  }
}  

/* ================================================== */

void
CNF_SetupAccessRestrictions(void)
{
  AllowDeny *node;
  int status;

  for (node = ntp_auth_list.next; node != &ntp_auth_list; node = node->next) {
    status = NCR_AddAccessRestriction(node->ip, node->subnet_bits, node->allow, node->all);
    if (!status) {
      LOG(LOGS_WARN, LOGF_Configure, "Bad subnet for %08lx\n", node->ip);
    }
  }

  for (node = cmd_auth_list.next; node != &cmd_auth_list; node = node->next) {
    status = CAM_AddAccessRestriction(node->ip, node->subnet_bits, node->allow, node->all);
    if (!status) {
      LOG(LOGS_WARN, LOGF_Configure, "Bad subnet for %08lx\n", node->ip);
    }
  }

  return;
}

/* ================================================== */

int
CNF_GetNoClientLog(void)
{
  return no_client_log;
}

/* ================================================== */

void
CNF_GetBindAddress(unsigned long *addr)
{
  *addr = bind_address;
}

