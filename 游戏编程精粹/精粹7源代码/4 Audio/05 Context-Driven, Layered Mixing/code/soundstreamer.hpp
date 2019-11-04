//----------------------------------------------------------------------------
// File: soundstreamer.hpp
// Copyright (C) 2007, Robert Sparks
//----------------------------------------------------------------------------
#ifndef SOUNDSTREAMER_HPP
#define SOUNDSTREAMER_HPP

//----------------------------------------------------------------------------
#include "soundfile.hpp"
#include "mixingsystem.hpp"

//----------------------------------------------------------------------------
class SoundStreamer
{
    public:
    
        SoundStreamer( MixingCategory mixingCategory );
        ~SoundStreamer( void );
        
        void SetFile( SoundFile* pFile, unsigned int bufferSize );
        void Play( void );
        void SetActiveChannel( unsigned int channel );
        bool GetIsPlaying( void ) { return mIsPlaying; }
        void Update( void );

    private:

        void UpdateSettings( void );
        void UpdatePlay( void );
        void UpdateLoad( void );

        // Maintain a constant power level between the channel volumes
        int CalculateSecondaryVolume( int primaryVolume );

        // Calculate the next volume to use while fading in
        int IncrementFadeInVolume( int volume );


        SoundFile* mpFile;
        IDirectSoundBuffer* mpIDirectSoundBuffer;
        MixingCategory mMixingCategory;
        unsigned int mActiveChannel;
        unsigned int mBufferSize;
        unsigned int mWrite;
        unsigned int mWriteCount;
        unsigned int mPlay;
        unsigned int mPlayCount;
        bool mIsPlaying;
};

#endif // SOUNDSTREAMER_HPP
