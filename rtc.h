/*
  $Header: /cvs/src/chrony/rtc.h,v 1.7 1999/04/19 20:27:29 richard Exp $

  =======================================================================

  chronyd/chronyc - Programs for keeping computer clocks accurate.

  Copyright (C) 1997-1999 Richard P. Curnow
  All rights reserved.

  For conditions of use, refer to the file LICENCE.

  =======================================================================

  */

#ifndef GOT_RTC_H
#define GOT_RTC_H

#include "reports.h"

extern void RTC_Initialise(void);
extern void RTC_Finalise(void);
extern void RTC_TimePreInit(void);
extern void RTC_TimeInit(void (*after_hook)(void *), void *anything);
extern void RTC_StartMeasurements(void);
extern int  RTC_GetReport(RPT_RTC_Report *report);

#define RTC_ST_OK 0
#define RTC_ST_NODRV 1
#define RTC_ST_BADFILE 2

extern int RTC_WriteParameters(void);

extern int RTC_Trim(void);

extern void RTC_CycleLogFile(void);

#endif /* GOT_RTC_H */
