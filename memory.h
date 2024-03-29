/*
  chronyd/chronyc - Programs for keeping computer clocks accurate.

 **********************************************************************
 * Copyright (C) Richard P. Curnow  1997-2002
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 * 
 **********************************************************************

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
