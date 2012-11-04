/*
  $Header: /home/richard/myntp/chrony/chrony-1.1/RCS/main.c,v 1.23 1999/04/19 20:27:29 richard Exp $

  =======================================================================

  chronyd/chronyc - Programs for keeping computer clocks accurate.

  Copyright (C) 1997-1999 Richard P. Curnow
  All rights reserved.

  For conditions of use, refer to the file LICENCE.

  =======================================================================

  The main program
  */

#include "sysincl.h"

#include "main.h"
#include "sched.h"
#include "local.h"
#include "sys.h"
#include "ntp_io.h"
#include "ntp_sources.h"
#include "ntp_core.h"
#include "sources.h"
#include "sourcestats.h"
#include "reference.h"
#include "logging.h"
#include "conf.h"
#include "cmdmon.h"
#include "keys.h"
#include "acquire.h"
#include "manual.h"
#include "version.h"
#include "rtc.h"
#include "clientlog.h"

/* ================================================== */

static int reload = 0;

/* ================================================== */

volatile void
MAI_CleanupAndExit(void)
{
  if (CNF_GetDumpOnExit()) {
    SRC_DumpSources();
  }

  RTC_Finalise();
  MNL_Finalise();
  ACQ_Finalise();
  CAM_Finalise();
  KEY_Finalise();
  CLG_Finalise();
  NIO_Finalise();
  NSR_Finalise();
  NCR_Finalise();
  SRC_Finalise();
  SST_Finalise();
  REF_Finalise();
  SYS_Finalise();
  SCH_Finalise();
  LCL_Finalise();
  LOG_Finalise();

  exit(0);
}

/* ================================================== */

static void
signal_cleanup(int x)
{
  LOG(LOGS_WARN, LOGF_Main, "chronyd exiting on signal");
  MAI_CleanupAndExit();
}

/* ================================================== */

static void
post_acquire_hook(void *anything)
{

  CNF_AddSources();
  if (reload) {
    /* Note, we want reload to come well after the initialisation from
       the real time clock - this gives us a fighting chance that the
       system-clock scale for the reloaded samples still has a
       semblence of validity about it. */
    SRC_ReloadSources();
  }
  CNF_SetupAccessRestrictions();

  RTC_StartMeasurements();
}

/* ================================================== */

static void
post_init_rtc_hook(void *anything)
{
  CNF_ProcessInitStepSlew(post_acquire_hook, NULL);
}

/* ================================================== */

int main
(int argc, char **argv)
{
  char *conf_file = NULL;
  int debug = 0;
  int do_init_rtc = 0;

  LOG_Initialise();

  /* Parse command line options */
  while (++argv, (--argc)>0) {

    if (!strcmp("-f", *argv)) {
      ++argv, --argc;
      conf_file = *argv;
    } else if (!strcmp("-r", *argv)) {
      reload = 1;
    } else if (!strcmp("-s", *argv)) {
      do_init_rtc = 1;
    } else if (!strcmp("-v", *argv) || !strcmp("--version",*argv)) {
      /* This write to the terminal is OK, it comes before we turn into a daemon */
      printf("chronyd (chrony) version %s\n", PROGRAM_VERSION_STRING);
      exit(0);
    } else if (!strcmp("-d", *argv)) {
      debug = 1;
    } else {
      LOG(LOGS_WARN, LOGF_Main, "Unrecognized command line option [%s]\n", *argv);
    }
  }

#ifndef SYS_WINNT
  if (getuid() != 0) {
    /* This write to the terminal is OK, it comes before we turn into a daemon */
    fprintf(stderr,"Not superuser\n");
    exit(1);
  }

  /* Turn into a daemon */
  if (!debug) {
    LOG_GoDaemon();
  }
#endif

  CNF_ReadFile(conf_file);

  if (do_init_rtc) {
    RTC_TimePreInit();
  }

  LCL_Initialise();
  SCH_Initialise();
  SYS_Initialise();
  REF_Initialise();
  SST_Initialise();
  SRC_Initialise();
  NCR_Initialise();
  NSR_Initialise();
  NIO_Initialise();
  CLG_Initialise();
  KEY_Initialise();
  CAM_Initialise();
  ACQ_Initialise();
  MNL_Initialise();
  RTC_Initialise();

  if (do_init_rtc) {
    RTC_TimeInit(post_init_rtc_hook, NULL);
  } else {
    post_init_rtc_hook(NULL);
  }

  signal(SIGINT, signal_cleanup);
  signal(SIGTERM, signal_cleanup);
#if !defined(WINNT)
  signal(SIGQUIT, signal_cleanup);
  signal(SIGHUP, signal_cleanup);
#endif /* WINNT */

  /* The program normally runs under control of the main loop in
     the scheduler. */
  SCH_MainLoop();

  MAI_CleanupAndExit();

  return 0;
}

/* ================================================== */
