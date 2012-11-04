/*
  $Header: /home/richard/myntp/chrony/chrony-1.1/RCS/rtc_linux.h,v 1.9 1999/04/19 20:27:29 richard Exp $

  ======================================================================

  chronyd/chronyc - Programs for keeping computer clocks accurate.

  Copyright (C) 1999 Richard P. Curnow
  All rights reserved.

  For conditions of use, refer to the file LICENCE.

  ======================================================================

  */

#ifndef _GOT_RTC_LINUX_H
#define _GOT_RTC_LINUX_H

#include "reports.h"

#if defined LINUX

extern int RTC_Linux_Initialise();
extern void RTC_Linux_Finalise(void);
extern void RTC_Linux_TimePreInit(void);
extern void RTC_Linux_TimeInit(void (*after_hook)(void *), void *anything);
extern void RTC_Linux_StartMeasurements(void);

/* 0=success, 1=no driver, 2=can't write file */
extern int RTC_Linux_WriteParameters(void);

extern int RTC_Linux_GetReport(RPT_RTC_Report *report);
extern int RTC_Linux_Trim(void);

extern void RTC_Linux_CycleLogFile(void);

#endif /* defined LINUX */

#endif _GOT_RTC_LINUX_H
