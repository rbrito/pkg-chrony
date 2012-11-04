/*
  $Header: /home/richard/myntp/chrony/chrony-1.1/RCS/memory.h,v 1.5 1999/04/19 20:27:29 richard Exp $

  =======================================================================

  chronyd/chronyc - Programs for keeping computer clocks accurate.

  Copyright (C) 1997-1999 Richard P. Curnow
  All rights reserved.

  For conditions of use, refer to the file LICENCE.

  =======================================================================

  Header file for memory functions
  */

#ifndef GOT_MEMORY_H
#define GOT_MEMORY_H

#include <stdlib.h>

#define Malloc(x) malloc(x)
#define MallocNew(T) ((T *) malloc(sizeof(T)))
#define MallocArray(T, n) ((T *) malloc((n) * sizeof(T)))
#define Realloc(x,y) realloc(x,y)
#define ReallocArray(T,n,x) ((T *) realloc((void *)(x), (n)*sizeof(T)))
#define Free(x) free(x)

#endif /* GOT_MEMORY_H */
