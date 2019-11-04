/*____________________________________________________________________
|
| File: lipsync.cpp
|
| Description: Contains functions to generate real-time lipsync data.
|
| Functions:	vocab_Lipsync
|              Get_Word
|
| Notes: Possible Improvements:
|   1) Fusing same consecutive visemes into one longer viseme.
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

//#define DEBUG

/*___________________
|
| Include Files
|__________________*/

// Enable older CRT functions (such as strcpy) without warnings from vc8 (vc 2005 .net)
#if _MSC_VER >= 1400
 #define _CRT_SECURE_NO_DEPRECATE				// shut up the vc8 compiler
 #define _CRT_NONSTDC_NO_DEPRECATE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "common.h"
#include "list.h"

#include "vocab.h"

/*___________________
|
| Function prototypes
|__________________*/

static void Get_Word (char *str, char *word, int *pos);

/*___________________
|
| Constants
|__________________*/

#define END_OF_SENTENCE           -1    // period
#define END_OF_SENTENCE_DURATION  500   // milliseconds (average time of silence on a period - found by looking at tts generated wav files)
#define VISEME_OVERLAP            80    // percent of time of overlap (before and after)

/*____________________________________________________________________
|
| Function: vocab_Lipsync
|
| Outputs: Generates real-time lipsync data.  If successful, this
|   function allocates memory in callers out struct.  This must be
|   freed by the caller.
|___________________________________________________________________*/

void vocab_Lipsync (Vocabulary vocab, LipsyncInput *in, LipsyncOutput *out)
{
  int i, pos, n, num_periods, t;
  char *buff;
  unsigned total_viseme_time, viseme_time, index_time;
  byte *phonemes, *visemes;
  LipsyncPosition lipsync_position;
  List list = 0;

/*___________________________________________________________________
|
| Verify input params
|___________________________________________________________________*/

  assert (in);
  assert (in->num_samples > 0);
  assert ((in->bits_per_sample == 8) OR (in->bits_per_sample == 16));
  assert (in->duration> 0);
  assert (in->data);
  assert (in->str);
  assert (out);

/*___________________________________________________________________
|
| Initialize variables
|___________________________________________________________________*/

  // Zero out the output struct
  memset ((void *)out, 0, sizeof(LipsyncOutput));
  // Create an empty linked list
  list = list_Init (sizeof(LipsyncPosition));
  // Create a temp string
  buff = (char *) calloc (strlen(in->str)+1, sizeof(char));
  if (buff == 0)
    DEBUG_ERROR ("vocab_Lipsync(): Error allocating memory for temp string")

  num_periods = 0;

/*___________________________________________________________________
|
| Generate lipsync data in a linked list
|___________________________________________________________________*/

  // Get first word in input string
  pos = 0;
  Get_Word (in->str, buff, &pos);
  // Is there a word to process?
  while (buff[0]) {
    // End of sentence?
    if (buff[0] == '.') {
      // Insert 'period' marker
      lipsync_position.viseme = END_OF_SENTENCE;
      list_AddEnd (list, (void *)&lipsync_position);
      num_periods++;
    }
    else {
      // Translate the word into phonemes
      phonemes = 0;
      n = 0;
#ifdef DEBUG
char str2[300];
sprintf (str2, "Translating word: %s", buff);
DEBUG_WRITE (str2);
#endif
      vocab_TranslateWord (vocab, buff, &n, &phonemes);
      if (n) {
#ifdef DEBUG
sprintf (str2, "%d visemes", n);
DEBUG_WRITE (str2);
#endif
        // Translate the phonemes into visemes
        visemes = (byte *) malloc (n);
        if (visemes) {
          vocab_TranslatePhonemesToVisemes (phonemes, visemes, n);
          // Add the visemes to the list
          for (i=0; i<n; i++) {
            lipsync_position.viseme = visemes[i];
            list_AddEnd (list, (void *)&lipsync_position);
          }
          free (visemes);
        }
        if (phonemes) // if here, should always be true
          free (phonemes);
      }
    }
    // Get next word in input string
    Get_Word (in->str, buff, &pos);
  }

/*___________________________________________________________________
|
| Calculate time for each viseme
|___________________________________________________________________*/

  if (list_Size (list) - num_periods) {
    // Compute total viseme time (total time visemes are active)
    total_viseme_time = in->duration - (num_periods * END_OF_SENTENCE_DURATION);
    // Compute time for each viseme
    viseme_time = total_viseme_time / (list_Size (list) - num_periods);
  }
  
/*___________________________________________________________________
|
| Convert linked list to array
|___________________________________________________________________*/

  index_time = 0;

  if (list_Size (list) - num_periods) {
    out->num_positions = list_Size (list) - num_periods;
    out->position = (LipsyncPosition *) calloc (out->num_positions, sizeof(LipsyncPosition));
    if (out->position == 0)
      DEBUG_ERROR ("vocab_Lipsync(): Error allocating memory for array of positions")
    else {
      // Get each entry from linked list
      for (i=0; NOT list_Empty (list);) {
        list_RemoveStart (list, (void *)&lipsync_position);
        // Is this a period?
        if (lipsync_position.viseme == END_OF_SENTENCE)
          // Skip ahead in time
          index_time += END_OF_SENTENCE_DURATION;
        // Else, it's a viseme
        else {
          out->position[i].viseme = lipsync_position.viseme;
          // Calculate start time for the viseme
          t  = (int)index_time - (int)(viseme_time * ((float)VISEME_OVERLAP / 100));
          if (t < 0)
            t = 0;
          out->position[i].start = t;
          // Calculate end time for the viseme
          t = (int)((index_time + viseme_time - 1) + (viseme_time * ((float)VISEME_OVERLAP / 100)));
          if ((unsigned)t > in->duration)
            t = in->duration;
          out->position[i].end = t;
          i++;
          index_time += viseme_time;
        }
      }
    }
  }

/*___________________________________________________________________
|
| Free resources used
|___________________________________________________________________*/

  if (buff)
    free (buff);
  if (list)
    list_Free (list);
}

/*____________________________________________________________________
|
| Function: Get_Word
|
| Input: Called from vocab_Lipsync()
| Outputs: Strips next word, starting at pos, from str putting into
|   word.  Advances pos.  A word can consist of a single period ('.')
|   or a set of characters with no whitespace.
|___________________________________________________________________*/

static void Get_Word (char *str, char *word, int *pos)
{
  int n;
  char c;

  n = 0; // # characters in word
  for (;;) {
    // End of str reached?
    if (*pos >= (int)strlen(str))
      break;
    // Get a character
    c = (char) str[*pos];
    (*pos)++;
    // Non-whitespace?
    if ((NOT isspace(c)) AND (c != '.')) {
      word[n] = c;
      n++;
    }
    // Period character?
    else if (c == '.') {
      // At end of a word?
      if (n) {
        (*pos)--;
        break;
      }
      // A word by itself?
      else {
        word[n] = c;
        n++;
        break;
      }
    }
    // Whitespace otherwise
    else {
      if (n == 0)
        continue;
      else
        break;
    }
  }
  word[n] = 0;
}
