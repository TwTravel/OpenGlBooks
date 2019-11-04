/*____________________________________________________________________
|
| File: vocab.cpp
|
| Description: Contains functions for a vocabulary of words and
|   corresponding phonemes that make up each word.  Compatible with
|   CMU Pronouncing Dictionary (cmudict.0.6).
|
| Functions:	vocab_Init
|              Get_Token
|              Valid_Word
|              Valid_Phoneme
|             vocab_Free
|             vocab_TranslateWord
|             vocab_TranslatePhonemesToVisemes
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

#define DEBUG

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

#include "vocab.h"

/*___________________
|
| Type definitions
|__________________*/

struct Entry {
  char *word;
  int   num_phonemes;
  byte *phonemes;
};

struct VocabularyData {
  int    num_entries;
  Entry *entries;
};

/*___________________
|
| Function prototypes
|__________________*/

static void Get_Token (char *token, FILE *fp);
static bool Valid_Word (char *token);
static bool Valid_Phoneme (char *token, byte *id);

/*___________________
|
| Macros
|__________________*/

#define VOCAB ((VocabularyData *)vocab)

/*____________________________________________________________________
|
| Function: vocab_Init
|
| Outputs: Initialize vocabulary.
|___________________________________________________________________*/

Vocabulary vocab_Init (char *filename)
{
  int i, k, n;
  byte id, phonemes[50];
  char token[256];
  FILE *fp = 0;
  VocabularyData *vocabdata;
  bool initialized = false;

/*___________________________________________________________________
|
| Verify input params
|___________________________________________________________________*/

  assert (filename);

/*___________________________________________________________________
|
| Create the vocab object
|___________________________________________________________________*/

  // Create the vocab object
  vocabdata = (VocabularyData *) calloc (1, sizeof(VocabularyData));
  if (vocabdata) {
    
/*___________________________________________________________________
|
| Initialize vocab
|___________________________________________________________________*/

    fp = fopen (filename, "rt");
    if (fp == 0)
      DEBUG_ERROR ("vocab_Init(): Error opening vocabulary file")
    else {
      // Compute # words in file
      for (n=0;;) {
        // Get next token, if any, from file
        Get_Token (token, fp);
        // End of file reached?
        if (token[0] == 0)
          break;
        if (Valid_Word (token)) 
          n++;
        // Skip rest of line in file (up to and including newline)
        if (token[0] != '\n')
          fgets (token, 256, fp);
      }
      // Allocate space for an array of words
#ifdef DEBUG
      char ss[256];
      sprintf (ss, "%d words in dictionary file", n);
      DEBUG_WRITE (ss)
#endif
      vocabdata->entries = (Entry *) calloc (n, sizeof(Entry));
      if (vocabdata->entries == 0)
        DEBUG_ERROR ("vocab_Init(): Error allocating memory for array of entries")
      else {
//DEBUG_WRITE ("Adding a word to the vocabulary...")
        // Add the words to the vocabulary
        fseek (fp, 0, SEEK_SET);
        for (i=0;;) {
          // Get next token, if any, from file
          Get_Token (token, fp);
//DEBUG_WRITE (token)
          // End of file reached?
          if (token[0] == 0)
            break;
          // If not a valid word, use up the line
          if (NOT Valid_Word (token)) {
//DEBUG_WRITE ("  not a valid word")
            // Skip rest of line in file (up to and including newline)
            if (token[0] != '\n')
              fgets (token, 256, fp);
            continue;
          }
          // Add a word to the vocab
          else {
//DEBUG_WRITE (" a valid word!")
            // Allocate space in vocab entry
            vocabdata->entries[i].word = (char *) calloc (strlen(token)+1, sizeof(char));
            if (vocabdata->entries[i].word == 0) {
              DEBUG_ERROR ("vocab_Init(): Error allocating memory for a word")
              break;
            }
            // Copy word to vocab
            strcpy (vocabdata->entries[i].word, token);
            // Collect phonemes, if any, for this word
            for (k=0;;) {
              // Get next phoneme
              Get_Token (token, fp);
              // End of line or file reached?
              if ((token[0] == 0) OR (token[0] == '\n'))
                break;
              // Add this phoneme to list of phonemes
              if (Valid_Phoneme (token, &id)) {
                phonemes[k] = id;
                // Increment # phonemes found
                k++;
              }
            }
            // Any phonemes found?
            if (k == 0) {
              DEBUG_WRITE (token)
              DEBUG_ERROR ("vocab_Init(): Error no phonemes found for a word")
              break;
            }
            else {
              // Allocate memory in entry for a list of phonemes
              vocabdata->entries[i].phonemes = (byte *) calloc (k, sizeof(byte));
              if (vocabdata->entries[i].phonemes == 0) {
                DEBUG_ERROR ("vocab_Init(): Error allocating memory for phoneme array") 
                break;
              }
              else {
                // Add phonemes to entry
                memcpy (vocabdata->entries[i].phonemes, phonemes, k);
                vocabdata->entries[i].num_phonemes = k;
              }
            }
            // Increment # words found
            i++;
          } // else if (NOT Valid_Word...
        } // for
        // All word added?
#ifdef DEBUG
        char s2[256];
        sprintf (s2, "%d words added to vocabulary", i);
        DEBUG_WRITE (s2)
#endif
        if (i == n) {
          vocabdata->num_entries = i;
          initialized = true;
        }
      }
    }
    // Close file?
    if (fp)
      fclose (fp);
  }

/*___________________________________________________________________
|
| On any error, release any resources created
|___________________________________________________________________*/

  if (NOT initialized) {
    vocab_Free (vocabdata);
    vocabdata = 0;
    DEBUG_ERROR ("vocab_Init(): Error creating vocab object")
  }
    
  return ((Vocabulary)vocabdata);                              
}

/*____________________________________________________________________
|
| Function: Get_Token
|
| Input: Called from vocab_Init()
| Output: Returns next token from input file or 0 on any error.
|
|   A token is a string of characters with no whitespace or a newline 
|   character by itself.
|___________________________________________________________________*/

static void Get_Token (char *token, FILE *fp)
{
  int n;
  char c;

  n = 0; // # characters in token
  for (;;) {
    // Get a character
    c = (char) fgetc (fp);
    // End of file reached
    if (feof(fp))
      break;
    // Non-whitespace?
    if (NOT isspace(c)) {
      token[n] = c;
      n++;
    }
    // Newline character?
    else if (c == '\n') {
      // At end of a token?
      if (n) {
        fseek (fp, -1, SEEK_CUR);
        break;
      }
      // A token by itself?
      else {
        token[n] = c;
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
  token[n] = 0;
}

/*____________________________________________________________________
|
| Function: Valid_Word
|
| Input: Called from vocab_Init()
| Output: Returns true if token is a valid word.  Valid words start 
|   with a letter and have only alphabetic characters and at most one
|   apostrophe.
|___________________________________________________________________*/

static bool Valid_Word (char *token)
{
  int i, non_alpha, apostrophe;
  bool valid = false;

  // Is first letter alphabetic?
  if (isalpha(token[0])) {
    // Look at rest of letters
    for (i=1, non_alpha=0, apostrophe=0; i<(int)strlen(token); i++) 
      if (NOT isalpha(token[i]))
        if (token[i] == '\'')
          apostrophe++;
        else {
          non_alpha++;
          break;
        }
    // Is the word valid?
    if ((non_alpha == 0) AND (apostrophe <= 1))
      valid = true;
  }
  else
    valid = false;

  return (valid);
}

/*____________________________________________________________________
|
| Function: Valid_Phoneme
|
| Input: Called from vocab_Init()
| Output: Returns true if token is a valid phoneme (see vocab.h).
|   If valid, returns in callers variable an integer associated with
|   the phoneme.
|___________________________________________________________________*/

static bool Valid_Phoneme (char *token, byte *id)
{
  int i;
  bool stripped;
  char str[256];
  bool valid = false;

  static struct {
    char *phoneme;
    byte  id;
  } phoneme_list[vocab_NUM_PHONEMES] = {
    { "AA", vocab_PHONEME_AA },
    { "AE", vocab_PHONEME_AE },
    { "AH", vocab_PHONEME_AH },
    { "AO", vocab_PHONEME_AO },
    { "AW", vocab_PHONEME_AW },
    { "AY", vocab_PHONEME_AY },
    { "B",  vocab_PHONEME_B  },
    { "CH", vocab_PHONEME_CH },
    { "D",  vocab_PHONEME_D  },
    { "DH", vocab_PHONEME_DH },
    { "EH", vocab_PHONEME_EH },
    { "ER", vocab_PHONEME_ER },
    { "EY", vocab_PHONEME_EY },
    { "F",  vocab_PHONEME_F  },
    { "G",  vocab_PHONEME_G  },
    { "HH", vocab_PHONEME_HH },
    { "IH", vocab_PHONEME_IH },
    { "IY", vocab_PHONEME_IY },
    { "JH", vocab_PHONEME_JH },
    { "K",  vocab_PHONEME_K  },
    { "L",  vocab_PHONEME_L  },
    { "M",  vocab_PHONEME_M  },
    { "N",  vocab_PHONEME_N  },
    { "NG", vocab_PHONEME_NG },
    { "OW", vocab_PHONEME_OW },
    { "OY", vocab_PHONEME_OY },
    { "P",  vocab_PHONEME_P  },
    { "R",  vocab_PHONEME_R  },
    { "S",  vocab_PHONEME_S  },
    { "SH", vocab_PHONEME_SH },
    { "T",  vocab_PHONEME_T  },
    { "TH", vocab_PHONEME_TH },
    { "UH", vocab_PHONEME_UH },
    { "UW", vocab_PHONEME_UW },
    { "V",  vocab_PHONEME_V  },
    { "W",  vocab_PHONEME_W  },
    { "Y",  vocab_PHONEME_Y  },
    { "Z",  vocab_PHONEME_Z  },
    { "ZH", vocab_PHONEME_ZH }
  };

  // Strip any trailing numbers
  strcpy (str, token);
  do {
    stripped = false;
    if (isdigit(str[strlen(str)-1])) {
      str[strlen(str)-1] = 0;
      stripped = true;
    }
  } while (stripped AND strlen(str));
  
  // Any characters left?
  if (str[0]) {
    // Convert to all uppercase
    for (i=0; i<(int)strlen(str); i++)
      str[i] = toupper (str[i]);

    // Identify this phoneme?
    for (i=0; i<vocab_NUM_PHONEMES; i++) 
      if (!strcmp(str, phoneme_list[i].phoneme)) {
        *id = phoneme_list[i].id;
        valid = true;
        break;
      }

    // Print error message if not found
    if (NOT valid) {
      strcpy (str, "Valid_Phoneme(): Unknown phoneme = ");
      strcat (str, token);
      DEBUG_ERROR (str)
    }
  }

  return (valid);
}

/*____________________________________________________________________
|
| Function: vocab_Free
|
| Output: Frees the vocabulary.
|___________________________________________________________________*/

void vocab_Free (Vocabulary vocab)
{
  int i;

  if (vocab) {
    if (VOCAB->entries) {
      // Free each entry in the dictionary array
      for (i=0; i<VOCAB->num_entries; i++) {
        if (VOCAB->entries[i].word)
          free (VOCAB->entries[i].word);
        if (VOCAB->entries[i].phonemes)
          free (VOCAB->entries[i].phonemes);
      }
      // Free the array
      free (VOCAB->entries);
      VOCAB->entries = 0;
      VOCAB->num_entries = 0;
    }
    free (VOCAB);
  }
}

/*____________________________________________________________________
|
| Function: vocab_TranslateWord
|
| Output: Translates a word into phonemes.  Allocates memory for
|   phonemes array.  Caller must free the memory.
|___________________________________________________________________*/

bool vocab_TranslateWord (Vocabulary vocab, char *word, int *num_phonemes, byte **phonemes)
{
  int i, x, first, last, mid;
  char *upperword;
  bool translated = false;

/*___________________________________________________________________
|
| Verify input params
|___________________________________________________________________*/

  assert (VOCAB);
  assert (VOCAB->entries);
  assert (word);
  assert (num_phonemes);
  assert (phonemes);

/*___________________________________________________________________
|
| Convert the word to all uppercase
|___________________________________________________________________*/

  if (strlen(word)) {
    // Allocate memory for a copy of the word
    upperword = (char *) calloc (strlen(word)+1, sizeof(char));
    if (upperword == 0) 
      DEBUG_WRITE ("vocab_TranslateWord(): Error allocating memory for word")
    else {
      strcpy (upperword, word);
      for (i=0; i<(int)strlen(upperword); i++)
        upperword[i] = toupper (upperword[i]);

/*___________________________________________________________________
|
| Translate the word
|___________________________________________________________________*/

      // Binary search for the word
      first = 0;
      last = VOCAB->num_entries - 1;
      mid = last / 2;
      while (first <= last) {
        mid = (last-first)/2+first;
        x = strcmp (upperword, VOCAB->entries[mid].word);
        // Is this entry the word?
        if (x == 0) {
          // Allocate memory for phonemes
          *phonemes = (byte *) calloc (VOCAB->entries[mid].num_phonemes, sizeof(byte));
          if (*phonemes == 0) 
            DEBUG_WRITE ("vocab_TranslateWord(): Error allocating memory for phonemes")
          else {
            // Copy phonemes
            memcpy (*phonemes, VOCAB->entries[mid].phonemes, VOCAB->entries[mid].num_phonemes);
            *num_phonemes = VOCAB->entries[mid].num_phonemes;
            translated = true;
          }
          break;
        }
        else if (x < 0) 
          last = mid - 1;
        else
          first = mid + 1;
      }
      free (upperword);
    }
  }

  return (translated);
}


/*____________________________________________________________________
|
| Function: vocab_TranslateWord
|
| Output: Translates each of n phonemes into a viseme.  Both arrays
|   must be valid.  This function does not allocate any memory.
|___________________________________________________________________*/

void vocab_TranslatePhonemesToVisemes (byte *phonemes, byte *visemes, int n)
{
  int i, j;
  static struct {
    byte phoneme, viseme;
  } xlat [vocab_NUM_PHONEMES] = {
    { vocab_PHONEME_AA, vocab_VISEME_BIG_AAH },
    { vocab_PHONEME_AE, vocab_VISEME_AAH     },
    { vocab_PHONEME_AH, vocab_VISEME_AAH     },
    { vocab_PHONEME_AO, vocab_VISEME_BIG_AAH },
    { vocab_PHONEME_AW, vocab_VISEME_BIG_AAH },
    { vocab_PHONEME_AY, vocab_VISEME_AAH     },
    { vocab_PHONEME_B,  vocab_VISEME_B_M_P   },
    { vocab_PHONEME_CH, vocab_VISEME_CH_J_SH },
    { vocab_PHONEME_D,  vocab_VISEME_D_S_T   },
    { vocab_PHONEME_DH, vocab_VISEME_TH      },
    { vocab_PHONEME_EH, vocab_VISEME_EH      },
    { vocab_PHONEME_ER, vocab_VISEME_R       },
    { vocab_PHONEME_EY, vocab_VISEME_EH      },
    { vocab_PHONEME_F,  vocab_VISEME_F_V     },
    { vocab_PHONEME_G,  vocab_VISEME_CH_J_SH },
    { vocab_PHONEME_HH, vocab_VISEME_EH      },
    { vocab_PHONEME_IH, vocab_VISEME_I       },
    { vocab_PHONEME_IY, vocab_VISEME_EE      },
    { vocab_PHONEME_JH, vocab_VISEME_CH_J_SH },
    { vocab_PHONEME_K,  vocab_VISEME_CH_J_SH },
    { vocab_PHONEME_L,  vocab_VISEME_TH      },
    { vocab_PHONEME_M,  vocab_VISEME_B_M_P   },
    { vocab_PHONEME_N,  vocab_VISEME_N       },
    { vocab_PHONEME_NG, vocab_VISEME_D_S_T   },
    { vocab_PHONEME_OW, vocab_VISEME_OH      },
    { vocab_PHONEME_OY, vocab_VISEME_OOH_Q   },
    { vocab_PHONEME_P,  vocab_VISEME_B_M_P   },
    { vocab_PHONEME_R,  vocab_VISEME_R       },
    { vocab_PHONEME_S,  vocab_VISEME_D_S_T   },
    { vocab_PHONEME_SH, vocab_VISEME_CH_J_SH },
    { vocab_PHONEME_T,  vocab_VISEME_D_S_T   },
    { vocab_PHONEME_TH, vocab_VISEME_TH      },
    { vocab_PHONEME_UH, vocab_VISEME_OH      },
    { vocab_PHONEME_UW, vocab_VISEME_OOH_Q   },
    { vocab_PHONEME_V,  vocab_VISEME_F_V     },
    { vocab_PHONEME_W,  vocab_VISEME_W       },
    { vocab_PHONEME_Y,  vocab_VISEME_EE      },
    { vocab_PHONEME_Z,  vocab_VISEME_W       },
    { vocab_PHONEME_ZH, vocab_VISEME_CH_J_SH }
  };

/*___________________________________________________________________
|
| Verify input params
|___________________________________________________________________*/

  assert (phonemes);
  assert (visemes);
  assert (n > 0);

/*___________________________________________________________________
|
| Translate the phonemes
|___________________________________________________________________*/

  // For each phoneme
  for (i=0; i<n; i++) {
    // Find it in the translation table
    for (j=0; j<vocab_NUM_PHONEMES; j++)
      if (phonemes[i] == xlat[j].phoneme)
        break;
    // Found it?
    if (j < vocab_NUM_PHONEMES)
      visemes[i] = xlat[j].viseme;
    else
      DEBUG_ERROR ("vocab_TranslatePhonemesToVisemes(): Error, can't find phoneme in translation table")
  }
}
