/*
  $Header: /cvs/src/chrony/candm.h,v 1.34 1999/04/21 20:10:58 richard Exp $

  =======================================================================

  chronyd/chronyc - Programs for keeping computer clocks accurate.

  Copyright (C) 1997-1999 Richard P. Curnow
  All rights reserved.

  For conditions of use, refer to the file LICENCE.

  =======================================================================

  Definitions for the network protocol used for command and monitoring
  of the timeserver.

  */

#ifndef GOT_CANDM_H
#define GOT_CANDM_H

#include "sysincl.h"

/* This is the default port to use for CANDM, if no alternative is
   defined */
#define DEFAULT_CANDM_PORT 323

/* Request codes */
#define REQ_NULL 0
#define REQ_ONLINE 1
#define REQ_OFFLINE 2
#define REQ_BURST 3
#define REQ_MODIFY_MINPOLL 4
#define REQ_MODIFY_MAXPOLL 5
#define REQ_DUMP 6
#define REQ_MODIFY_MAXDELAY 7
#define REQ_MODIFY_MAXDELAYRATIO 8
#define REQ_MODIFY_MAXUPDATESKEW 9
#define REQ_LOGON 10
#define REQ_SETTIME 11
#define REQ_LOCAL 12
#define REQ_MANUAL 13
#define REQ_N_SOURCES 14
#define REQ_SOURCE_DATA 15
#define REQ_REKEY 16
#define REQ_ALLOW 17
#define REQ_ALLOWALL 18
#define REQ_DENY 19
#define REQ_DENYALL 20
#define REQ_CMDALLOW 21
#define REQ_CMDALLOWALL 22
#define REQ_CMDDENY 23
#define REQ_CMDDENYALL 24
#define REQ_ACCHECK 25
#define REQ_CMDACCHECK 26
#define REQ_ADD_SERVER 27
#define REQ_ADD_PEER 28
#define REQ_DEL_SOURCE 29
#define REQ_WRITERTC 30
#define REQ_DFREQ 31
#define REQ_DOFFSET 32
#define REQ_TRACKING 33
#define REQ_SOURCESTATS 34
#define REQ_RTCREPORT 35
#define REQ_TRIMRTC 36
#define REQ_CYCLELOGS 37
#define REQ_SUBNETS_ACCESSED 38
#define REQ_CLIENT_ACCESSES 39
#define REQ_CLIENT_ACCESSES_BY_INDEX 40
#define REQ_MANUAL_LIST 41
#define REQ_MANUAL_DELETE 42
#define REQ_MAKESTEP 43
#define N_REQUEST_TYPES 44

/* Special utoken value used to log on with first exchange being the
   password.  (This time value has long since gone by) */
#define SPECIAL_UTOKEN 0x10101010

/* The EOR (end of record) fields are used by the offsetof operator in
   pktlength.c, to get the number of bytes that ought to be
   transmitted for each packet type. */

typedef struct {
  unsigned long mask;
  unsigned long address;
  int EOR;
} REQ_Online;

typedef struct {
  unsigned long mask;
  unsigned long address;
  int EOR;
} REQ_Offline;

typedef struct {
  unsigned long mask;
  unsigned long address;
  int n_good_samples;
  int n_total_samples;
  int EOR;
} REQ_Burst;

typedef struct {
  unsigned long address;
  int new_minpoll;
  int EOR;
} REQ_Modify_Minpoll;

typedef struct {
  unsigned long address;
  int new_maxpoll;
  int EOR;
} REQ_Modify_Maxpoll;

typedef struct {
  int pad;
  int EOR;
} REQ_Dump;

typedef struct {
  unsigned long address;
  long new_max_delay;
  int EOR;
} REQ_Modify_Maxdelay;

typedef struct {
  unsigned long address;
  long new_max_delay_ratio;
  int EOR;
} REQ_Modify_Maxdelayratio;

typedef struct {
  long new_max_update_skew;
  int EOR;
} REQ_Modify_Maxupdateskew;

typedef struct {
  struct timeval ts;
  int EOR;
} REQ_Logon;

typedef struct {
  struct timeval ts;
  int EOR;
} REQ_Settime;

typedef struct {
  int on_off;
  int stratum;
  int EOR;
} REQ_Local;

typedef struct {
  int option;
  int EOR;
} REQ_Manual;

typedef struct {
  int EOR;
} REQ_N_Sources;

typedef struct {
  int index;
  int EOR;
} REQ_Source_Data;

typedef struct {
  int EOR;
} REQ_Rekey;

typedef struct {
  unsigned long ip;
  int subnet_bits;
  int EOR;
} REQ_Allow_Deny;

typedef struct {
  unsigned long ip;
  int EOR;
} REQ_Ac_Check;

typedef struct {
  unsigned long ip_addr;
  unsigned long port;
  int minpoll;
  int maxpoll;
  int presend_minpoll;
  int online;
  unsigned long authkey;
  long max_delay;
  long max_delay_ratio;
  int EOR;
} REQ_NTP_Source;

typedef struct {
  unsigned long ip_addr;
  int EOR;
} REQ_Del_Source;

typedef struct {
  int EOR;
} REQ_WriteRtc;

typedef struct {
  long dfreq;
  int EOR;
} REQ_Dfreq;

typedef struct {
  long sec;
  long usec;
  int EOR;
} REQ_Doffset;

typedef struct {
  int EOR;
} REQ_Tracking;

typedef struct {
  unsigned long index;
  int EOR;
} REQ_Sourcestats;

typedef struct {
  int EOR;
} REQ_RTCReport;

typedef struct {
  int EOR;
} REQ_TrimRTC;

typedef struct {
  int EOR;
} REQ_CycleLogs;

typedef struct {
  unsigned long ip;
  unsigned long bits_specd;
} REQ_SubnetsAccessed_Subnet;

#define MAX_SUBNETS_ACCESSED 8

typedef struct {
  unsigned long n_subnets;
  REQ_SubnetsAccessed_Subnet subnets[MAX_SUBNETS_ACCESSED];
} REQ_SubnetsAccessed;

/* This is based on the response size rather than the
   request size */
#define MAX_CLIENT_ACCESSES 16

typedef struct {
  unsigned long n_clients;
  unsigned long client_ips[MAX_CLIENT_ACCESSES];
} REQ_ClientAccesses;  

typedef struct {
  unsigned long first_index;
  unsigned long n_indices;
  int EOR;
} REQ_ClientAccessesByIndex;

typedef struct {
  int EOR;
} REQ_ManualList;

typedef struct {
  int index;
  int EOR;
} REQ_ManualDelete;

typedef struct {
  int EOR;
} REQ_MakeStep;

/* ================================================== */

#define PKT_TYPE_CMD_REQUEST 1
#define PKT_TYPE_CMD_REPLY 2

/* This version number needs to be incremented whenever the packet
   size and/or the format of any of the existing messages is changed.
   Other changes, e.g. new command types, should be handled cleanly by
   client.c and cmdmon.c anyway, so the version can stay the same.
   
   Version 1 : original version with fixed size packets

   Version 2 : both command and reply packet sizes made capable of
   being variable length.

 */

#define PROTO_VERSION_NUMBER 2

/* ================================================== */

typedef struct {
  unsigned char version; /* Protocol version */
  unsigned char pkt_type; /* What sort of packet this is */
  unsigned char res1;
  unsigned char res2;
  unsigned short command; /* Which command is being issued */
  unsigned short attempt; /* How many resends the client has done
                             (count up from zero for same sequence
                             number) */
  unsigned long sequence; /* Client's sequence number */
  unsigned long utoken; /* Unique token per incarnation of daemon */
  unsigned long token; /* Command token (to prevent replay attack) */
  unsigned long auth[4]; /* MD5 authentication of the packet */

  union {
    REQ_Online online;
    REQ_Offline offline;
    REQ_Burst burst;
    REQ_Modify_Minpoll modify_minpoll;
    REQ_Modify_Maxpoll modify_maxpoll;
    REQ_Dump dump;
    REQ_Modify_Maxdelay modify_maxdelay;
    REQ_Modify_Maxdelayratio modify_maxdelayratio;
    REQ_Modify_Maxupdateskew modify_maxupdateskew;
    REQ_Logon logon;
    REQ_Settime settime;
    REQ_Local local;
    REQ_Manual manual;
    REQ_N_Sources n_sources;
    REQ_Source_Data source_data;
    REQ_Rekey rekey;
    REQ_Allow_Deny allow_deny;
    REQ_Ac_Check ac_check;
    REQ_NTP_Source ntp_source;
    REQ_Del_Source del_source;
    REQ_WriteRtc writertc;
    REQ_Dfreq dfreq;
    REQ_Doffset doffset;
    REQ_Tracking tracking;
    REQ_Sourcestats sourcestats;
    REQ_RTCReport rtcreport;
    REQ_TrimRTC trimrtc;
    REQ_CycleLogs cyclelogs;
    REQ_SubnetsAccessed subnets_accessed;
    REQ_ClientAccesses client_accesses;
    REQ_ClientAccessesByIndex client_accesses_by_index;
    REQ_ManualList manual_list;
    REQ_ManualDelete manual_delete;
    REQ_MakeStep make_step;
  } data; /* Command specific parameters */

} CMD_Request;

/* ================================================== */
/* Authority codes for command types */

#define PERMIT_OPEN 0
#define PERMIT_LOCAL 1
#define PERMIT_AUTH 2

/* ================================================== */
/* These conversion utilities are used to convert between the internal
   and the 'wire' representation of real quantities */

#define WIRE2REAL(x) ((double) ((long) ntohl(x)) / 65536.0)
#define REAL2WIRE(x) (htonl((long)(0.5 + 65536.0 * (x))))

/* ================================================== */

/* Reply codes */
#define RPY_NULL 1
#define RPY_N_SOURCES 2
#define RPY_SOURCE_DATA 3
#define RPY_MANUAL_TIMESTAMP 4
#define RPY_TRACKING 5
#define RPY_SOURCESTATS 6
#define RPY_RTC 7
#define RPY_SUBNETS_ACCESSED 8
#define RPY_CLIENT_ACCESSES 9
#define RPY_CLIENT_ACCESSES_BY_INDEX 10
#define RPY_MANUAL_LIST 11
#define N_REPLY_TYPES 12

/* Status codes */
#define STT_SUCCESS 0
#define STT_FAILED 1
#define STT_UNAUTH 2
#define STT_INVALID 3
#define STT_NOSUCHSOURCE 4
#define STT_INVALIDTS 5
#define STT_NOTENABLED 6
#define STT_BADSUBNET 7
#define STT_ACCESSALLOWED 8
#define STT_ACCESSDENIED 9
#define STT_NOHOSTACCESS 10
#define STT_SOURCEALREADYKNOWN 11
#define STT_TOOMANYSOURCES 12
#define STT_NORTC 13
#define STT_BADRTCFILE 14
#define STT_INACTIVE 15
#define STT_BADSAMPLE 16

typedef struct {
  int EOR;
} RPY_Null;

typedef struct {
  unsigned long n_sources;
  int EOR;
} RPY_N_Sources;

#define RPY_SD_MD_CLIENT 0
#define RPY_SD_MD_PEER   1
#define RPY_SD_MD_REF    2

#define RPY_SD_ST_SYNC 0
#define RPY_SD_ST_UNREACH 1
#define RPY_SD_ST_FALSETICKER 2
#define RPY_SD_ST_JITTERY 3
#define RPY_SD_ST_OTHER 4

typedef struct {
  unsigned long ip_addr;
  unsigned short poll;
  unsigned short stratum;
  unsigned short state;
  unsigned short mode;
  unsigned long  since_sample;
  long orig_latest_meas;
  long latest_meas;
  unsigned long latest_meas_err;
  long est_offset;
  unsigned long est_offset_err;
  long resid_freq;
  unsigned long resid_skew;
  int EOR;
} RPY_Source_Data;

typedef struct {
  unsigned long ref_id;
  unsigned long stratum;
  unsigned long ref_time_s;
  unsigned long ref_time_us;
  unsigned long current_correction_s;
  unsigned long current_correction_us;
  long freq_ppm;
  long resid_freq_ppm;
  long skew_ppm;
  long root_delay;
  long root_dispersion;
  int EOR;
} RPY_Tracking;

typedef struct {
  unsigned long ip_addr;
  unsigned long n_samples;
  unsigned long n_runs;
  unsigned long span_seconds;
  unsigned long sd_us;
  long resid_freq_ppm;
  long skew_ppm;
  int EOR;
} RPY_Sourcestats;

typedef struct {
  unsigned long ref_time;
  unsigned short n_samples;
  unsigned short n_runs;
  unsigned long span_seconds;
  long rtc_seconds_fast;
  long rtc_gain_rate_ppm;
  int EOR;
} RPY_Rtc;

typedef struct {
  unsigned long centiseconds;
  long dfreq_ppm;
  long new_afreq_ppm;
  int EOR;
} RPY_ManualTimestamp;

typedef struct {
  /* Assume sizeof(unsigned long) == 4; if not so, other parts of the s/w will break
     horribly anyway */
  unsigned long ip;
  unsigned long bits_specd;
  unsigned long bitmap[8];
} RPY_SubnetsAccessed_Subnet;

typedef struct {
  unsigned long n_subnets;
  RPY_SubnetsAccessed_Subnet subnets[MAX_SUBNETS_ACCESSED];
} RPY_SubnetsAccessed;

typedef struct {
  unsigned long ip;
  unsigned long client_hits;
  unsigned long peer_hits;
  unsigned long cmd_hits_auth;
  unsigned long cmd_hits_normal;
  unsigned long cmd_hits_bad;
  unsigned long last_ntp_hit_ago;
  unsigned long last_cmd_hit_ago;
} RPY_ClientAccesses_Client;

typedef struct {
  unsigned long n_clients;
  RPY_ClientAccesses_Client clients[MAX_CLIENT_ACCESSES];
} RPY_ClientAccesses;

typedef struct {
  unsigned long n_indices;      /* how many indices there are in the server's table */
  unsigned long next_index;     /* the index 1 beyond those processed on this call */
  unsigned long n_clients;      /* the number of valid entries in the following array */
  RPY_ClientAccesses_Client clients[MAX_CLIENT_ACCESSES];
} RPY_ClientAccessesByIndex;

#define MAX_MANUAL_LIST_SAMPLES 32

typedef struct {
  unsigned long when;
  long slewed_offset;
  long orig_offset;
  long residual;
} RPY_ManualListSample;

typedef struct {
  unsigned long n_samples;
  RPY_ManualListSample samples[MAX_MANUAL_LIST_SAMPLES];
} RPY_ManualList;

typedef struct {
  unsigned char version;
  unsigned char pkt_type;
  unsigned char res1;
  unsigned char res2;
  unsigned short command; /* Which command is being replied to */
  unsigned short reply; /* Which format of reply this is */
  unsigned short status; /* Status of command processing */
  unsigned short number; /* Which packet this is in reply sequence */
  unsigned short total; /* Number of replies to expect in this sequence */
  unsigned short pad1; /* Get up to 4 byte alignment */
  unsigned long sequence; /* Echo of client's sequence number */
  unsigned long utoken; /* Unique token per incarnation of daemon */
  unsigned long token; /* New command token (only if command was successfully
                          authenticated) */
  unsigned long auth[4]; /* MD5 authentication of the packet */

  union {
    RPY_Null null;
    RPY_N_Sources n_sources;
    RPY_Source_Data source_data;
    RPY_ManualTimestamp manual_timestamp;
    RPY_Tracking tracking;
    RPY_Sourcestats sourcestats;
    RPY_Rtc rtc;
    RPY_SubnetsAccessed subnets_accessed;
    RPY_ClientAccesses client_accesses;
    RPY_ClientAccessesByIndex client_accesses_by_index;
    RPY_ManualList manual_list;
  } data; /* Reply specific parameters */

} CMD_Reply;

/* ================================================== */

#endif /* GOT_CANDM_H */
