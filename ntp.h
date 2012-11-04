/*
  $Header: /cvs/src/chrony/ntp.h,v 1.6 1999/04/19 20:27:29 richard Exp $

  =======================================================================

  chronyd/chronyc - Programs for keeping computer clocks accurate.

  Copyright (C) 1997-1999 Richard P. Curnow
  All rights reserved.

  For conditions of use, refer to the file LICENCE.

  =======================================================================

  Header file containing common NTP bits and pieces
  */

#ifndef GOT_NTP_H
#define GOT_NTP_H

typedef struct {
  unsigned long hi;
  unsigned long lo;
} NTP_int64;

typedef unsigned long NTP_int32;

#define AUTH_DATA_LEN 16 

/* Type definition for leap bits */
typedef enum {
  LEAP_Normal = 0,
  LEAP_InsertSecond = 1,
  LEAP_DeleteSecond = 2,
  LEAP_Unsynchronised = 3
} NTP_Leap;

typedef enum {
  MODE_UNDEFINED = 0,
  MODE_ACTIVE = 1,
  MODE_PASSIVE = 2,
  MODE_CLIENT = 3,
  MODE_SERVER = 4,
  MODE_BROADCAST = 5
} NTP_Mode;

typedef struct {
  unsigned char lvm;
  unsigned char stratum;
  signed char poll;
  signed char precision;
  NTP_int32 root_delay;
  NTP_int32 root_dispersion;
  NTP_int32 reference_id;
  NTP_int64 reference_ts;
  NTP_int64 originate_ts;
  NTP_int64 receive_ts;
  NTP_int64 transmit_ts;
  NTP_int32 auth_keyid;
  unsigned char auth_data[AUTH_DATA_LEN];
} NTP_Packet;

/* We have to declare a buffer type to hold a datagram read from the
   network.  Even though we won't be using them (yet?!), this must be
   large enough to hold NTP control messages. */

/* Define the maximum number of bytes that can be read in a single
   message.  (This is cribbed from ntp.h in the xntpd source code). */

#define MAX_NTP_MESSAGE_SIZE (468+12+16+4)

typedef union {
  NTP_Packet ntp_pkt;
  unsigned char arbitrary[MAX_NTP_MESSAGE_SIZE];
} ReceiveBuffer;

#define NTP_NORMAL_PACKET_SIZE (sizeof(NTP_Packet) - (sizeof(NTP_int32) + AUTH_DATA_LEN))

#endif /* GOT_NTP_H */
