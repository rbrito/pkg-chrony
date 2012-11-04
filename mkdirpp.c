/*
  $Header: /cvs/src/chrony/mkdirpp.c,v 1.7 1999/08/17 21:24:52 richard Exp $

  =======================================================================

  chronyd/chronyc - Programs for keeping computer clocks accurate.

  Copyright (C) 1997-1999 Richard P. Curnow
  All rights reserved.

  For conditions of use, refer to the file LICENCE.

  =======================================================================

  A function for creating a directory and any parent directories that
  don't exist.

  */

#include "sysincl.h"

#include "mkdirpp.h"

static int
do_dir(char *p)
{
  int status;
  struct stat buf;

  /* See if directory exists */
  status = stat(p, &buf);

  if (status < 0) {
    if (errno == ENOENT) {
      /* Try to create directory */
      status = mkdir(p, 0755);
      return status;
    } else {
      return status;
    }
  }

  if (!S_ISDIR(buf.st_mode)) {
    return -1;
  }

  return 0;
}

/* ================================================== */
/* Return 0 if the directory couldn't be created, 1 if it could (or
   already existed) */

int
mkdir_and_parents(const char *path)
{
  char *p;
  int len;
  int i, j, last;
  len = strlen(path);

  p = (char *) malloc(1 + len);

  i = 1;
  p[0] = path[0]; /* So leading / doesn't trip the next test */
  while (1) {
    p[i] = path[i];
    i++;
    
    if (path[i] == '/' || !path[i]) {
      p[i] = 0;

      if (do_dir(p) < 0) {
        return 0;
      }

      if (!path[i]) {
        /* End of the string */
        break;
      }

      /* check whether its a trailing / or group of / */
      last = 1;
      j = i+1;
      while (path[j]) {
        if (path[j] != '/') {
          last = 0;
          break;
        }
      }

      if (last) {
        break;
      }
    }

    if (!path[i]) break;

  }  

  free(p);
  return 1;

}

/* ================================================== */

#if 0
int main(int argc, char **argv) {
  if (argc > 1) {
    mkdir_and_parents(argv[1]);
    return 0;
  } else {
    return 1;
  }
}
#endif
                                  
