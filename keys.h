/*
  $Header: /home/richard/myntp/chrony/chrony-1.02/RCS/keys.h,v 1.5 1998/05/20 06:10:49 richard Exp $

  =======================================================================

  chronyd/chronyc - Programs for keeping computer clocks accurate.

  Copyright (C) 1997-1999 Richard P. Curnow
  All rights reserved.

  For conditions of use, refer to the file LICENCE.

  =======================================================================

  Header for key management module
  */

#ifndef GOT_KEYS_H
#define GOT_KEYS_H

extern void KEY_Initialise(void);
extern void KEY_Finalise(void);

extern void KEY_Reload(void);

extern void KEY_CommandKey(char **key, int *len);

extern int KEY_GetKey(unsigned long key_id, char **key, int *len);
extern int KEY_KeyKnown(unsigned long key_id);

#endif /* GOT_KEYS_H */
