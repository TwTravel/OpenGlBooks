/*___________________________________________________________________
|
|	File: list.cpp
|
| Description: Functions for a doubly linked list.
|
| Functions:  list_Init
|             list_Free
|             list_Empty
|             list_Size
|             list_AddEnd
|             list_RemoveStart
|             list_RemoveStart
|
| (C) Copyright 2007 Timothy E. Roden.  
|	
|	This code is included on the CD for the book Game Programming Gems 7.
| The author, Timothy E. Roden, grants the purchaser of the book 
| unrestricted rights to use this software for any research or commercial 
| purpose provided acknowledgement is given of its origin.
|
| Compiler: Tested with Microsoft Visual Studio 2005 (Version 8.0.50727.762)
|___________________________________________________________________*/

/*___________________
|
| Include Files
|__________________*/

// Enable older CRT functions (such as strcpy) without warnings from vc8 (vc 2005 .net)
#if _MSC_VER >= 1400
 #define _CRT_SECURE_NO_DEPRECATE				// shut up the vc8 compiler
 #define _CRT_NONSTDC_NO_DEPRECATE
#endif

#include <windows.h>
#include <winbase.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

#include "common.h"

#include "list.h"

/*___________________
|
| Type definitions
|__________________*/

struct Node {
  void *item;
  Node *next, *previous;
};

struct ListData {
  Node *start, *end;
  int count;
  unsigned item_size;
};

/*___________________
|
| Macros
|__________________*/

#define LIST ((ListData *)list)

/*____________________________________________________________________
|
| Function: list_Init
|
| Output: Initializes a list.  Returns true if initialized, else false 
|   on any error.
|___________________________________________________________________*/

List list_Init (unsigned item_size)
{
  ListData *listdata = 0;

  listdata = (ListData *) malloc (sizeof(ListData));
  if (listdata) {
    listdata->start = 0;
    listdata->end   = 0;
    listdata->count = 0;
    listdata->item_size = item_size;
  }

  return ((List)listdata);
}

/*____________________________________________________________________
|
| Function: list_Free
|
| Output: Frees a list.
|___________________________________________________________________*/

void list_Free (List list)
{
  list_Flush (list);
  free (LIST);
}

/*____________________________________________________________________
|
| Function: list_Empty
|
| Output: Returns true if list is empty.
|___________________________________________________________________*/

bool list_Empty (List list)
{
  return (LIST->count == 0);
}

/*____________________________________________________________________
|
| Function: list_Flush
|
| Output: Flushes all data in list.
|___________________________________________________________________*/

void list_Flush (List list)
{
  Node *temp;

  while (LIST->start) {
    temp = LIST->start;
    LIST->start = LIST->start->next;
    free (temp->item);
    free (temp);
  }
  LIST->end = 0;
  LIST->count = 0;
}

/*____________________________________________________________________
|
| Function: list_Size
|
| Output: Returns # of items in list.
|___________________________________________________________________*/

int list_Size (List list)
{
  return (LIST->count);
}

/*____________________________________________________________________
|
| Function: list_AddEnd
|
| Output: Adds an item to the end of the list.
|___________________________________________________________________*/

void list_AddEnd (List list, void *item)
{
  Node *newnode;

  newnode = (Node *) malloc (sizeof(Node));
  newnode->item = (byte *) malloc (LIST->item_size * sizeof(byte));
  memcpy (newnode->item, item, LIST->item_size);
  newnode->next     = 0;
  newnode->previous = 0;

  if (LIST->start == 0)
    LIST->start = newnode;
  else {
    LIST->end->next = newnode;
    newnode->previous = LIST->end;
  }
  LIST->end = newnode;
  LIST->count++;
}

/*____________________________________________________________________
|
| Function: list_RemoveStart
|
| Output: Removes an item from the start of list.  Returns true if removed,
|   else false.
|___________________________________________________________________*/

bool list_RemoveStart (List list, void *item)
{
  Node *temp;
  bool removed = false;

  if (LIST->count) {
    memcpy (item, LIST->start->item, LIST->item_size);
    temp = LIST->start;
    LIST->start = LIST->start->next;
    if (LIST->start)
      LIST->start->previous = 0;
    else
      LIST->end = 0;
    free (temp->item);
    free (temp);
    LIST->count--;
    removed = true;
  }

  return (removed);
}

/*____________________________________________________________________
|
| Function: list_RemoveStart
|
| Output: Removes an item from the start of list.  Returns true if removed,
|   else false.
|___________________________________________________________________*/

bool list_RemoveStart (List list)
{
  Node *temp;
  bool removed = false;

  if (LIST->count) {
    temp = LIST->start;
    LIST->start = LIST->start->next;
    if (LIST->start)
      LIST->start->previous = 0;
    else
      LIST->end = 0;
    free (temp->item);
    free (temp);
    LIST->count--;
    removed = true;
  }

  return (removed);
}
