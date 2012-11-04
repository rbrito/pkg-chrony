/*
  $Header: /home/richard/myntp/chrony/chrony-1.1/RCS/sys.h,v 1.5 1999/04/19 20:27:29 richard Exp $

  =======================================================================

  chronyd/chronyc - Programs for keeping computer clocks accurate.

  Copyright (C) 1997-1999 Richard P. Curnow
  All rights reserved.

  For conditions of use, refer to the file LICENCE.

  =======================================================================

  This is the header for the file that links in the operating system-
  specific parts of the software

*/

#ifndef GOT_SYS_H
#define GOT_SYS_H

/* Called at the start of the run to do initialisation */
extern void SYS_Initialise(void);

/* Called at the end of the run to do final clean-up */
extern void SYS_Finalise(void);

#endif /* GOT_SYS_H */
