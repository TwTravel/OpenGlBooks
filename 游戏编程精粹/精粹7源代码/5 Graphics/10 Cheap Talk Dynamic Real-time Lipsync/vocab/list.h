/*___________________________________________________________________
|
|	File: list.h
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

#ifndef _LIST_H_
#define _LIST_H_

/*___________________
|
| Type definitions
|__________________*/

typedef void *List;

/*___________________
|
| Functions
|__________________*/

List list_Init         (unsigned item_size);
void list_Free         (List list);
bool list_Empty        (List list);
void list_Flush        (List list);
int  list_Size         (List list);
void list_AddEnd       (List list, void *item);
bool list_RemoveStart  (List list, void *item);
bool list_RemoveStart  (List list);

#endif
