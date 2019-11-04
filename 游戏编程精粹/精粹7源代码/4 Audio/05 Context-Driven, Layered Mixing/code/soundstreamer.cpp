//----------------------------------------------------------------------------
// File: soundstreamer.cpp
// Copyright (C) 2007, Robert Sparks
//----------------------------------------------------------------------------
#include "soundstreamer.hpp"
#include <math.h>
#include "soundsystem.hpp"
#include "util.hpp"

//----------------------------------------------------------------------------
// Helper struct for locking direct sound buffers
struct BufferLock
{
    BufferLock( void ) 
        : 
        pBuffer1( NULL),
        numBytes1( 0 ),
        pBuffer2( NULL ),
        numBytes2( 0 )
    {
    }        
    
    void* pBuffer1;
    DWORD numBytes1;
    void* pBuffer2;
    DWORD numBytes2;
};

//----------------------------------------------------------------------------
SoundStreamer::SoundStreamer( MixingCategory mixingCategory )
    :
    mpFile( NULL ),
    mpIDirectSoundBuffer( NULL ),
    mMixingCategory( mixingCategory ),
    mActiveChannel( 0 ),
    mBufferSize( 0 ),
    mWrite( 0 ),
    mWriteCount( 0 ),
    mPlay( 0 ),
    mPlayCount( 0 ),
    mIsPlaying( false )
{
}

//----------------------------------------------------------------------------
SoundStreamer::~SoundStreamer( void )
{   
    if ( NULL != mpIDirectSoundBuffer )
    {
        mpIDirectSoundBuffer->Release( );
        mpIDirectSoundBuffer = NULL;
    }        
}

//----------------------------------------------------------------------------
void SoundStreamer::SetFile( SoundFile* pFile, unsigned int bufferSize )
{
    // Clean up old resources
    if ( NULL != mpFile )
    {
        mpFile = NULL;
    }
    
    if ( NULL != mpIDirectSoundBuffer )
    {
        mpIDirectSoundBuffer->Release( );
        mpIDirectSoundBuffer = NULL;
    }
    
    // Reset trackers
    mpFile = pFile;
    mBufferSize = bufferSize;
    mWrite = 0;
    mWriteCount = 0;
    mPlay = 0;
    mPlayCount = 0;

    if ( NULL != mpFile )
    {
        WAVEFORMATEX wfx = mpFile->GetFormat( );
        SoundSystem::Get( )->CreateDirectSoundBuffer( &mpIDirectSoundBuffer, &wfx, mBufferSize, false );
    }   

    // Starting Loading right away
    UpdateLoad( );
}

//----------------------------------------------------------------------------
void SoundStreamer::Play( void )
{
    ASSERT( NULL != mpFile );
    HRESULT hr;

    // Make sure the buffers hav fresh data before starting to play
    UpdateSettings( );
    UpdateLoad( );

    if ( NULL != mpIDirectSoundBuffer )
    {
        hr = mpIDirectSoundBuffer->Play( 0, 0, DSBPLAY_LOOPING );
        ASSERT( SUCCEEDED( hr ) );
    }
    mIsPlaying = true;
}

//----------------------------------------------------------------------------
void SoundStreamer::SetActiveChannel( unsigned int channel )
{
    mActiveChannel = channel;
}

//----------------------------------------------------------------------------
void SoundStreamer::Update( void )
{
    if ( !mIsPlaying )
    {
        return;
    }

    UpdateSettings( );
    UpdatePlay( );
    UpdateLoad( );
}

//----------------------------------------------------------------------------
void SoundStreamer::UpdateSettings( void )
{
    HRESULT hr;

    LONG volume = DSBVOLUME_MIN;
    
    // Multiply by the mixing system central mix.
    // Give the volume a bit of a curve
    float mixingCatVolumeFactor = MixingSystem::Get( )->GetCategoryVolume( mMixingCategory );
    mixingCatVolumeFactor = pow( mixingCatVolumeFactor, 0.25f );
    
    volume = (LONG)( ( 1.0f - mixingCatVolumeFactor ) * volume );

    // Update the buffer settings
    if ( NULL != mpIDirectSoundBuffer )
    {
        hr = mpIDirectSoundBuffer->SetVolume( volume );
    }
}            

//----------------------------------------------------------------------------
void SoundStreamer::UpdatePlay( void )
{
    HRESULT hr;
    
    if ( NULL == mpFile )
    {
        return;
    }
    
    if ( NULL != mpIDirectSoundBuffer )
    {
        // Get the current play position
        DWORD playPosition = 0;
        DWORD writePosition = 0;    
        hr = mpIDirectSoundBuffer->GetCurrentPosition( &playPosition, &writePosition );
        ASSERT( SUCCEEDED( hr ) );

        // Calculate how many bytes have been played
        unsigned int bytesPlayed;
        if ( playPosition > mPlay )
        {   
            bytesPlayed = playPosition - mPlay;
        }
        else
        {
            bytesPlayed = mBufferSize - ( mPlay - playPosition );
        }
        
        // Update our trackers for total count and current position
        mPlayCount += bytesPlayed;
        mPlay = playPosition;
        
        // Detect skipping and keep mPlayCount less than mWriteCount
        if ( mPlayCount > mWriteCount )
        {
            mPlayCount = mPlayCount - mBufferSize;
        }
    }
    
    // Remember that total play count needs to be multiplied by the number of buffers
    if ( mPlayCount * mpFile->GetFormat( ).nChannels >= mpFile->GetSize( ) )
    {
        // We've played the entire file.  It's time to stop
        mIsPlaying = false;
        if ( NULL != mpIDirectSoundBuffer )
        {
            hr = mpIDirectSoundBuffer->Stop( );
            ASSERT( SUCCEEDED( hr ) );
        }
    }        
}

//----------------------------------------------------------------------------
void SoundStreamer::UpdateLoad( void )
{
    if ( NULL == mpFile )
    {
        return;
    }

    HRESULT hr;

    // Calculate the number of bytes the in the buffer that have already been played
    // and need to be loaded again
    unsigned int dirtyBytes = mBufferSize - ( mWriteCount - mPlayCount );

    // If there are dirty bytes, we'll replace them with good ones
    if ( dirtyBytes > 0 )
    {
        BufferLock lock;
    
        // Lock the requested number of bytes
        if ( NULL != mpIDirectSoundBuffer )
        {
            hr = mpIDirectSoundBuffer->Lock( 
                mWrite,                    // lock position
                dirtyBytes,                // num bytes to lock
                &lock.pBuffer1,       // returns address of write buffer 1
                &lock.numBytes1,      // returns size of write buffer 1
                &lock.pBuffer2,       // returns address of write buffer 2
                &lock.numBytes2,      // returns size of write buffer 2
                NULL );                    // optionally specify flags
            ASSERT( SUCCEEDED( hr ) );  
        }
        
        // Write sound data from each channel into the buffers
        // If the data is stereo, then the left and right channels
        // are interleaved: LRLRLRLRLRLRLR.
        // Note that the number of bytes to write will always be a 
        // multiple of the sample size
        unsigned int bytesWritten = 0;
        unsigned int bytesToWrite = lock.numBytes1;
        unsigned int bytesPerSample = mpFile->GetFormat( ).wBitsPerSample / 8;
        ASSERT( ( bytesToWrite % bytesPerSample ) == 0 ); 
        
        while( bytesWritten < bytesToWrite )        
        {
            if ( NULL != lock.pBuffer1 )
            {
                // File will clear buffer if it has no more data
                mpFile->Read( ( char* ) lock.pBuffer1 + bytesWritten, bytesPerSample );
            }     
            
            bytesWritten += bytesPerSample;
        } 
        
        // Update our trackers
        mWriteCount += bytesToWrite;
        mWrite = ( mWrite + bytesToWrite ) % mBufferSize;

        // Unlock the buffers
        if ( NULL != mpIDirectSoundBuffer )
        {
            // Unlock the buffer
            hr = mpIDirectSoundBuffer->Unlock( 
                lock.pBuffer1,
                lock.numBytes1,
                lock.pBuffer2,
                lock.numBytes2 );
            ASSERT( SUCCEEDED( hr ) );                
        }
    }        
}       

//----------------------------------------------------------------------------
// This formula will maintain a constant power between the channels
//----------------------------------------------------------------------------
int SoundStreamer::CalculateSecondaryVolume( int primaryVolume )
{
    // Clamp value to something we can work with.  
    if ( primaryVolume >= DSBVOLUME_MAX )
    {
        return DSBVOLUME_MIN;
    }

    // Get out of hundreths of deciBels (milliBels), which is what DirectSound
    // works in.
    float primaryPower = pow( 10.0f, primaryVolume / 1000.0f );

    // We can maintain contant power by ensure both values add to 1
    float secondaryPower = 1.0f - primaryPower;

    // Now go back to milliBels
    int secondaryVolume = ( int ) (1000.0f * log10( secondaryPower ));

    // Clamp value
    if ( secondaryVolume < DSBVOLUME_MIN )
    {
        secondaryVolume = DSBVOLUME_MIN;
    }
    if ( secondaryVolume > DSBVOLUME_MAX )
    {
         secondaryVolume = DSBVOLUME_MAX;
    }


    return (int) secondaryVolume;
}
    
//----------------------------------------------------------------------------
// This formula will the next volume value while fading int
//----------------------------------------------------------------------------
int SoundStreamer::IncrementFadeInVolume( int volume )
{
    if ( volume >= DSBVOLUME_MAX )
    {
        return volume;
    }

    if ( volume < DSBVOLUME_MIN )
    {
        volume = DSBVOLUME_MIN;
    }

    // We map the current volume to a sine curve and then 
    // increment to the next value in the curve.
    static float incrementValue = 0.08f;
    float y = volume / ( float ) DSBVOLUME_MIN;
    float x = pow( y, 0.5f );
    x = x - incrementValue;
    if ( x < 0.0f )
    {
        x = 0.0f;
    }     
    y = x * x;
    volume = ( int )( y * DSBVOLUME_MIN );
    return ( int ) volume;
}



