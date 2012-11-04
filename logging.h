/*
  $Header: /cvs/src/chrony/logging.h,v 1.13 1999/04/19 20:27:29 richard Exp $

  =======================================================================

  chronyd/chronyc - Programs for keeping computer clocks accurate.

  Copyright (C) 1997-1999 Richard P. Curnow
  All rights reserved.

  For conditions of use, refer to the file LICENCE.

  =======================================================================

  Header file for diagnostic logging module

  */

#ifndef GOT_LOGGING_H
#define GOT_LOGGING_H

/* Definition of severity */
typedef enum {
  LOGS_INFO,
  LOGS_WARN,
  LOGS_ERR
} LOG_Severity;

/* Definition of facility.  Each message is tagged with who generated
   it, so that the user can customise what level of reporting he gets
   for each area of the software */
typedef enum {
  LOGF_Reference,
  LOGF_NtpIO,
  LOGF_NtpCore,
  LOGF_NtpSources,
  LOGF_Scheduler,
  LOGF_SourceStats,
  LOGF_Sources,
  LOGF_Local,
  LOGF_Util,
  LOGF_Main,
  LOGF_Configure,
  LOGF_CmdMon,
  LOGF_Acquire,
  LOGF_Manual,
  LOGF_Logging,
  LOGF_Rtc,
  LOGF_Regress,
  LOGF_SysLinux,
  LOGF_SysSolaris,
  LOGF_SysSunOS,
  LOGF_SysWinnt,
  LOGF_RtcLinux
} LOG_Facility;

/* Init function */
extern void LOG_Initialise(void);

/* Fini function */
extern void LOG_Finalise(void);

/* Line logging function */
extern void LOG_Line_Function(LOG_Severity severity, LOG_Facility facility, const char *format, ...);

/* Logging function for fatal errors */
extern volatile void LOG_Fatal_Function(LOG_Facility facility, const char *format, ...);

/* Position in code reporting function */
extern void LOG_Position(const char *filename, int line_number, const char *function_name);

extern void LOG_GoDaemon(void);

/* Line logging macro.  If the compiler is GNU C, we take advantage of
   being able to get the function name also. */
#if defined(__GNUC__)
#define LOG LOG_Position(__FILE__, __LINE__, __FUNCTION__); LOG_Line_Function
#define LOG_FATAL LOG_Position(__FILE__, __LINE__, __FUNCTION__); LOG_Fatal_Function
#else
#define LOG LOG_Position(__FILE__, __LINE__, ""); LOG_Line_Function
#define LOG_FATAL LOG_Position(__FILE__, __LINE__, ""); LOG_Fatal_Function
#endif /* defined (__GNUC__) */

#endif /* GOT_LOGGING_H */
