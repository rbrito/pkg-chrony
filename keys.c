/*
  $Header: /home/richard/myntp/chrony/chrony-1.1/RCS/keys.c,v 1.7 1999/04/19 20:27:29 richard Exp $

  =======================================================================

  chronyd/chronyc - Programs for keeping computer clocks accurate.

  Copyright (C) 1997-1999 Richard P. Curnow
  All rights reserved.

  For conditions of use, refer to the file LICENCE.

  =======================================================================

  Module for managing keys used for authenticating NTP packets and commands

  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "keys.h"
#include "conf.h"
#include "memory.h"

typedef struct {
  unsigned long id;
  char *val;
  int len;
} Key;

#define MAX_KEYS 256

static int n_keys;
static Key keys[MAX_KEYS];

static int command_key_valid;
static int command_key_pos;
static int cache_valid;
static unsigned long cache_key_id;
static int cache_key_pos;

/* ================================================== */

void
KEY_Initialise(void)
{
  n_keys = 0;
  command_key_valid = 0;
  cache_valid = 0;
  KEY_Reload();
  return;
}

/* ================================================== */

void
KEY_Finalise(void)
{
  /* Nothing to do */
  return;
}

/* ================================================== */

/* Compare two keys */

static int
compare_keys_by_id(const void *a, const void *b)
{
  const Key *c = (const Key *) a;
  const Key *d = (const Key *) b;

  if (c->id < d->id) {
    return -1;
  } else if (c->id > d->id) {
    return +1;
  } else {
    return 0;
  }

}

/* ================================================== */

void
KEY_Reload(void)
{
  int i;
  char *key_file;
  FILE *in;
  unsigned long key_id;
  char line[2048], keyval[2048];;

  for (i=0; i<n_keys; i++) {
    Free(keys[i].val);
  }

  n_keys = 0;

  key_file = CNF_GetKeysFile();

  if (key_file) {
    in = fopen(key_file, "r");
    if (in) {
      while (fgets(line, sizeof(line), in)) {
        line[strlen(line) - 1] = 0;
        if (sscanf(line, "%lu%s", &key_id, keyval) == 2) {
          keys[n_keys].id = key_id;
          keys[n_keys].len = strlen(keyval);
          keys[n_keys].val = MallocArray(char, 1 + keys[n_keys].len);
          strcpy(keys[n_keys].val, keyval);
          n_keys++;
        }
      }
      fclose(in);
      
      /* Sort keys into order.  Note, if there's a duplicate, it is
         arbitrary which one we use later - the user should have been
         more careful! */
      qsort((void *) keys, n_keys, sizeof(Key), compare_keys_by_id);

    }
  }

  command_key_valid = 0;
  cache_valid = 0;

  return;
}

/* ================================================== */

static int
lookup_key(unsigned long id)
{
  Key specimen, *where;
  int pos;

  specimen.id = id;
  where = (Key *) bsearch((void *)&specimen, (void *)keys, n_keys, sizeof(Key), compare_keys_by_id);
  if (!where) {
    return -1;
  } else {
    pos = where - keys;
    return pos;
  }
}

/* ================================================== */

void
KEY_CommandKey(char **key, int *len)
{
  unsigned long command_key_id;

  if (!command_key_valid) {
    command_key_id = CNF_GetCommandKey();
    command_key_pos = lookup_key(command_key_id);
    command_key_valid = 1;
  }

  if (command_key_pos >= 0) {
    *key = keys[command_key_pos].val;
    *len = keys[command_key_pos].len;
  } else {
    *key = "";
    *len = 0;
  }
}

/* ================================================== */

int
KEY_GetKey(unsigned long key_id, char **key, int *len)
{
  if (!cache_valid || key_id != cache_key_id) {
    cache_valid = 1;
    cache_key_pos = lookup_key(key_id);
    cache_key_id = key_id;
  }

  if (cache_key_pos >= 0) {
    *key = keys[cache_key_pos].val;
    *len = keys[cache_key_pos].len;
    return 1;
  } else {
    *key = "";
    *len = 0;
    return 0;
  }
}

/* ================================================== */

int
KEY_KeyKnown(unsigned long key_id)
{
  int position;

  if (cache_valid && (key_id == cache_key_id)) {
    return 1;
  } else {

    position = lookup_key(key_id);

    if (position >= 0) {
      /* Store key in cache, we will probably be using it in a
         minute... */
      cache_valid = 1;
      cache_key_pos = position;
      cache_key_id = key_id;
      return 1;
    } else {
      return 0;
    }
  }
}
