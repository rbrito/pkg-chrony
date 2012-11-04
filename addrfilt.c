/*
  $Header: /home/richard/myntp/chrony/chrony-1.1/RCS/addrfilt.c,v 1.5 1999/04/19 20:27:29 richard Exp $

  =======================================================================

  chronyd/chronyc - Programs for keeping computer clocks accurate.

  Copyright (C) 1997-1999 Richard P. Curnow
  All rights reserved.

  For conditions of use, refer to the file LICENCE.

  =======================================================================

  This module provides a set of routines for checking IP addresses
  against a set of rules and deciding whether they are allowed or
  disallowed.

  */

#include "sysincl.h"

#include "addrfilt.h"
#include "memory.h"

/* Define the number of bits which are stripped off per level of
   indirection in the tables */
#define NBITS 4

/* Define the table size */
#define TABLE_SIZE (1UL<<NBITS)

struct _TableNode;

typedef struct _TableNode ExtendedTable[TABLE_SIZE];

typedef enum {DENY, ALLOW, AS_PARENT} State;

typedef struct _TableNode {
  State state;
  ExtendedTable *extended;
} TableNode;

struct ADF_AuthTableInst {
  TableNode base;
};

/* ================================================== */

inline static unsigned long
get_subnet(unsigned long addr)
{
  return (addr >> (32-NBITS)) & ((1UL<<NBITS) - 1);
}

/* ================================================== */

inline static unsigned long
get_residual(unsigned long addr)
{
  return (addr << NBITS);
}

/* ================================================== */

ADF_AuthTable
ADF_CreateTable(void)
{
  ADF_AuthTable result;
  result = MallocNew(struct ADF_AuthTableInst);

  /* Default is that nothing is allowed */
  result->base.state = DENY;
  result->base.extended = NULL;

  return result;
}

/* ================================================== */
/* This function deletes all definitions of child nodes, in effect
   pruning a whole subnet definition back to a single parent
   record. */
static void
close_node(TableNode *node)
{
  int i;
  TableNode *child_node;

  if (node->extended != NULL) {
    for (i=0; i<TABLE_SIZE; i++) {
      child_node = &((*(node->extended))[i]);
      close_node(child_node);
    }
    Free(node->extended);
    node->extended = NULL;
  }

  return;
}


/* ================================================== */
/* Allocate the extension field in a node, and set all the children's
   states to default to that of the node being extended */

static void
open_node(TableNode *node)
{
  int i;
  TableNode *child_node;

  if (node->extended == NULL) {

    node->extended = MallocNew(ExtendedTable);

    for (i=0; i<TABLE_SIZE; i++) {
      child_node = &((*(node->extended))[i]);
      child_node->state = AS_PARENT;
      child_node->extended = NULL;
    }
  }
  return;
}

/* ================================================== */

static ADF_Status
set_subnet(TableNode *start_node,
           unsigned long ip,
           int subnet_bits,
           State new_state,
           int delete_children)
{
  int bits_to_go;
  unsigned long residual;
  unsigned long subnet;
  TableNode *node;

  bits_to_go = subnet_bits;
  residual = ip;
  node = start_node;

  if ((subnet_bits < 0) ||
      (subnet_bits > 32) ||
      ((subnet_bits % NBITS) != 0)) {
    return ADF_BADSUBNET;

  } else {
    
    
    while (bits_to_go > 0) {
      subnet = get_subnet(residual);
      residual = get_residual(residual);
      if (!(node->extended)) {
        open_node(node);
      }
      
      node = &((*(node->extended))[subnet]);
      bits_to_go -= NBITS;
    }
    
    if (delete_children) {
      close_node(node);
    }
    
    node->state = new_state;
    
    return ADF_SUCCESS;
  }
  
}

/* ================================================== */

ADF_Status
ADF_Allow(ADF_AuthTable table,
          unsigned long ip,
          int subnet_bits)
{
  return set_subnet(&(table->base), ip, subnet_bits, ALLOW, 0);
}

/* ================================================== */


ADF_Status
ADF_AllowAll(ADF_AuthTable table,
             unsigned long ip,
             int subnet_bits)
{
  return set_subnet(&(table->base), ip, subnet_bits, ALLOW, 1);
}

/* ================================================== */

ADF_Status
ADF_Deny(ADF_AuthTable table,
         unsigned long ip,
         int subnet_bits)
{
  return set_subnet(&(table->base), ip, subnet_bits, DENY, 0);
}

/* ================================================== */

ADF_Status
ADF_DenyAll(ADF_AuthTable table,
            unsigned long ip,
            int subnet_bits)
{
  return set_subnet(&(table->base), ip, subnet_bits, DENY, 1);
}

/* ================================================== */

void
ADF_DestroyTable(ADF_AuthTable table)
{
  close_node(&(table->base));
  Free(table);
}

/* ================================================== */

static int
check_ip_in_node(TableNode *start_node, unsigned long ip)
{
  unsigned long residual, subnet;
  int result = 0;
  int finished = 0;
  TableNode *node;
  State state=DENY;

  node = start_node;
  residual = ip;

  do {
    if (node->state != AS_PARENT) {
      state = node->state;
    }
    if (node->extended) {
      subnet = get_subnet(residual);
      residual = get_residual(residual);
      node = &((*(node->extended))[subnet]);
    } else {
      /* Make decision on this node */
      finished = 1;
    }
  } while (!finished);

  switch (state) {
    case ALLOW:
      result = 1;
      break;
    case DENY:
      result = 0;
      break;
    case AS_PARENT:
      assert(0);
      break;
  }

  return result;
}


/* ================================================== */

int
ADF_IsAllowed(ADF_AuthTable table,
              unsigned long ip)
{

  return check_ip_in_node(&(table->base), ip);

}

/* ================================================== */
