/*____________________________________________________________________
|
| File: vocab.h
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

#ifndef _VOCAB_H_
#define _VOCAB_H_

typedef unsigned char byte;
typedef void *Vocabulary;

// Input required to generate real-time lipsync data
struct LipsyncInput {
  // pcm audio data
  unsigned  num_samples;
  unsigned  bits_per_sample;
  unsigned  duration;         // in milliseconds
  byte     *data;
  // text data
  char     *str;
};

// Output format of real-time lipsync data
struct LipsyncPosition {
  int      viseme;      // viseme #
  unsigned start, end;  // time stamp in milliseconds
};

struct LipsyncOutput {
  int              num_positions; // # in array
  LipsyncPosition *position;      // array of positions 
  unsigned         start_time;    // used when playing an animation
};

//      Phoneme                  Example  Translation
#define vocab_PHONEME_AA  1   // odd      AA D
#define vocab_PHONEME_AE  2   // at       AE T
#define vocab_PHONEME_AH  3   // hut      HH AH T
#define vocab_PHONEME_AO  4   // ought    AO T
#define vocab_PHONEME_AW  5   // cow      K AW
#define vocab_PHONEME_AY  6   // hide     HH AY D
#define vocab_PHONEME_B   7   // be       B IY
#define vocab_PHONEME_CH  8   // cheese   CH IY Z
#define vocab_PHONEME_D   9   // dee      D IY
#define vocab_PHONEME_DH  10  // thee     DH IY
#define vocab_PHONEME_EH  11  // Ed       EH D
#define vocab_PHONEME_ER  12  // hurt     HH ER T
#define vocab_PHONEME_EY  13  // ate      EY T
#define vocab_PHONEME_F   14  // fee      F IY
#define vocab_PHONEME_G   15  // green    G R IY N
#define vocab_PHONEME_HH  16  // he       HH IY
#define vocab_PHONEME_IH  17  // it       IH T
#define vocab_PHONEME_IY  18  // eat      IY T
#define vocab_PHONEME_JH  19  // gee      JH IY
#define vocab_PHONEME_K   20  // key      K IY
#define vocab_PHONEME_L   21  // lee      L IY
#define vocab_PHONEME_M   22  // me       M IY
#define vocab_PHONEME_N   23  // knee     N IY
#define vocab_PHONEME_NG  24  // ping     P IH NG
#define vocab_PHONEME_OW  25  // oat      OW T
#define vocab_PHONEME_OY  26  // toy      T OY
#define vocab_PHONEME_P   27  // pee      P IY
#define vocab_PHONEME_R   28  // read     R IY D
#define vocab_PHONEME_S   29  // sea      S IY
#define vocab_PHONEME_SH  30  // she      SH IY
#define vocab_PHONEME_T   31  // tea      T IY
#define vocab_PHONEME_TH  32  // theta    TH EY T AH
#define vocab_PHONEME_UH  33  // hood     HH UH D
#define vocab_PHONEME_UW  34  // two      T UW
#define vocab_PHONEME_V   35  // vee      V IY
#define vocab_PHONEME_W   36  // we       W IY
#define vocab_PHONEME_Y   37  // yield    Y IY L D
#define vocab_PHONEME_Z   38  // zee      Z IY
#define vocab_PHONEME_ZH  39  // seizure  S IY ZH ER
#define vocab_NUM_PHONEMES 39

// Visemes used by FaceGen Modeller 3.1
#define vocab_VISEME_AAH      1   // aah
#define vocab_VISEME_B_M_P    2   // B,M,P
#define vocab_VISEME_BIG_AAH  3   // big aah
#define vocab_VISEME_CH_J_SH  4   // ch,J,sh
#define vocab_VISEME_D_S_T    5   // D,S,T
#define vocab_VISEME_EE       6   // ee
#define vocab_VISEME_EH       7   // eh
#define vocab_VISEME_F_V      8   // F,V
#define vocab_VISEME_I        9   // i
#define vocab_VISEME_K        10  // K
#define vocab_VISEME_N        11  // N
#define vocab_VISEME_OH       12  // oh
#define vocab_VISEME_OOH_Q    13  // ooh,Q
#define vocab_VISEME_R        14  // R
#define vocab_VISEME_TH       15  // th
#define vocab_VISEME_W        16  // W
#define vocab_NUM_VISEMES     16

// VOCAB.CPP
// Initialize vocabulary
Vocabulary vocab_Init (char *filename);
// Frees the vocabulary
void       vocab_Free (Vocabulary vocab);
// Translates a word into phonemes (allocates memory for phonemes array - caller must free)
bool       vocab_TranslateWord (Vocabulary vocab, char *word, int *num_phonemes, byte **phonemes);
// Translates a set of n phonemes into visemes
void       vocab_TranslatePhonemesToVisemes (byte *phonemes, byte *visemes, int n);

// LIPSYNC.CPP
// Generates lipsync data (allocates memory for out.positions array - caller must free)
void       vocab_Lipsync (Vocabulary vocab, LipsyncInput *in, LipsyncOutput *out);

#endif
