/*
  $Header: /home/richard/myntp/chrony/chrony-1.02/RCS/conf.h,v 1.13 1998/06/10 20:46:29 richard Exp $

  =======================================================================

  chronyd/chronyc - Programs for keeping computer clocks accurate.

  Copyright (C) 1997-1999 Richard P. Curnow
  All rights reserved.

  For conditions of use, refer to the file LICENCE.

  =======================================================================

  Header file for configuration module
  */

#ifndef GOT_CONF_H
#define GOT_CONF_H

extern void CNF_ReadFile(const char *filename);

extern void CNF_AddSources(void);

extern void CNF_ProcessInitStepSlew(void (*after_hook)(void *), void *anything);

extern unsigned short CNF_GetNTPPort(void);
extern char *CNF_GetDriftFile(void);
extern char *CNF_GetLogDir(void);
extern char *CNF_GetDumpDir(void);
extern int CNF_GetLogMeasurements(void);
extern int CNF_GetLogStatistics(void);
extern int CNF_GetLogTracking(void);
extern int CNF_GetLogRtc(void);
extern char *CNF_GetKeysFile(void);
extern char *CNF_GetRtcFile(void);
extern unsigned long CNF_GetCommandKey(void);
extern int CNF_GetDumpOnExit(void);
extern int CNF_GetManualEnabled(void);
extern int CNF_GetCommandPort(void);
extern int CNF_GetRTCOnUTC(void);
extern void CNF_GetLogChange(int *enabled, double *threshold);
extern void CNF_GetMailOnChange(int *enabled, double *threshold, char **user);
extern int CNF_GetNoClientLog(void);
extern void CNF_GetBindAddress(unsigned long *addr);

/* Value returned in ppm, as read from file */
extern double CNF_GetMaxUpdateSkew(void);
extern int CNF_AllowLocalReference(int *stratum);

extern void CNF_SetupAccessRestrictions(void);

#endif /* GOT_CONF_H */
