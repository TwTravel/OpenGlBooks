//----------------------------------------------------------------------------
// File: demo.cpp
// Copyright (C) 2007, Robert Sparks
//----------------------------------------------------------------------------
#include "demo.hpp"
#include "soundsystem.hpp"

//----------------------------------------------------------------------------
static const unsigned int SOUND_STREAMER_BUFFER_SIZE = 128 * 1024;

static const char* FILE_NAMES[] =
{
    "ambience.wav",
    "guns.wav",
    "footsteps.wav",
    "birds.wav",
    "cars.wav",
    "dialogue.wav"
};

//----------------------------------------------------------------------------
Demo::Demo( void )
{
    // Set the files to NULL
    for ( unsigned int i = 0; i < MixingCategory_Count; i++ )
    {
        mpSoundFiles[ i ] = NULL;
    }        

    // Create the sound streamers
    for ( unsigned int i = 0; i < MixingCategory_Count; i++ )
    {
        mpSoundStreamers[ i ] = new SoundStreamer( ( MixingCategory ) i );
    }        
    
    // Create the mixing snapshots with some initial values
    MixingSnapshots snapshot = MixingSnapshots_PreMix0; 
    mpMixingSnapshots[ snapshot ] = new MixingSnapshot( MixingLayer_PreMix );
    mpMixingSnapshots[ snapshot ]->SetVolume( MixingCategory_Ambience, 1.00f );
    mpMixingSnapshots[ snapshot ]->SetVolume( MixingCategory_Guns, 0.95f );
    mpMixingSnapshots[ snapshot ]->SetVolume( MixingCategory_Footsteps, 0.99f );
    mpMixingSnapshots[ snapshot ]->SetVolume( MixingCategory_Birds, 1.00f );
    mpMixingSnapshots[ snapshot ]->SetVolume( MixingCategory_Cars, 0.98f );
    mpMixingSnapshots[ snapshot ]->SetVolume( MixingCategory_Dialogue, 1.00f );
    mpMixingSnapshots[ snapshot ]->SetIsInstalled( true );

    snapshot = MixingSnapshots_Base0; 
    mpMixingSnapshots[ snapshot ] = new MixingSnapshot( MixingLayer_Base );
    mpMixingSnapshots[ snapshot ]->SetVolume( MixingCategory_Ambience, 0.20f );
    mpMixingSnapshots[ snapshot ]->SetVolume( MixingCategory_Guns, 0.00f );
    mpMixingSnapshots[ snapshot ]->SetVolume( MixingCategory_Footsteps, 0.40f );
    mpMixingSnapshots[ snapshot ]->SetVolume( MixingCategory_Birds, 1.00f );
    mpMixingSnapshots[ snapshot ]->SetVolume( MixingCategory_Cars, 0.20f );
    mpMixingSnapshots[ snapshot ]->SetVolume( MixingCategory_Dialogue, 1.00f );
    mpMixingSnapshots[ snapshot ]->SetIsInstalled( true );

    snapshot = MixingSnapshots_Base1; 
    mpMixingSnapshots[ snapshot ] = new MixingSnapshot( MixingLayer_Base );
    mpMixingSnapshots[ snapshot ]->SetVolume( MixingCategory_Ambience, 0.85f );
    mpMixingSnapshots[ snapshot ]->SetVolume( MixingCategory_Guns, 0.00f );
    mpMixingSnapshots[ snapshot ]->SetVolume( MixingCategory_Footsteps, 0.12f );
    mpMixingSnapshots[ snapshot ]->SetVolume( MixingCategory_Birds, 0.17f );
    mpMixingSnapshots[ snapshot ]->SetVolume( MixingCategory_Cars, 0.92f );
    mpMixingSnapshots[ snapshot ]->SetVolume( MixingCategory_Dialogue, 0.84f );
    mpMixingSnapshots[ snapshot ]->SetIsInstalled( false );

    snapshot = MixingSnapshots_Base2; 
    mpMixingSnapshots[ snapshot ] = new MixingSnapshot( MixingLayer_Base );
    mpMixingSnapshots[ snapshot ]->SetVolume( MixingCategory_Ambience, 0.00f );
    mpMixingSnapshots[ snapshot ]->SetVolume( MixingCategory_Guns, 0.96f );
    mpMixingSnapshots[ snapshot ]->SetVolume( MixingCategory_Footsteps, 0.92f );
    mpMixingSnapshots[ snapshot ]->SetVolume( MixingCategory_Birds, 0.24f );
    mpMixingSnapshots[ snapshot ]->SetVolume( MixingCategory_Cars, 1.0f );
    mpMixingSnapshots[ snapshot ]->SetVolume( MixingCategory_Dialogue, 0.80f );
    mpMixingSnapshots[ snapshot ]->SetIsInstalled( false );

    snapshot = MixingSnapshots_Modifying0; 
    mpMixingSnapshots[ snapshot ] = new MixingSnapshot( MixingLayer_Modifying );
    mpMixingSnapshots[ snapshot ]->SetVolume( MixingCategory_Ambience, 0.38f );
    mpMixingSnapshots[ snapshot ]->SetVolume( MixingCategory_Guns, 0.40f );
    mpMixingSnapshots[ snapshot ]->SetVolume( MixingCategory_Footsteps, 0.48f );
    mpMixingSnapshots[ snapshot ]->SetVolume( MixingCategory_Birds, 0.55f );
    mpMixingSnapshots[ snapshot ]->SetVolume( MixingCategory_Cars, 0.41f );
    mpMixingSnapshots[ snapshot ]->SetVolume( MixingCategory_Dialogue, 1.00f );
    mpMixingSnapshots[ snapshot ]->SetIsInstalled( false );

    snapshot = MixingSnapshots_Modifying1; 
    mpMixingSnapshots[ snapshot ] = new MixingSnapshot( MixingLayer_Modifying );
    mpMixingSnapshots[ snapshot ]->SetVolume( MixingCategory_Ambience, 1.00f );
    mpMixingSnapshots[ snapshot ]->SetVolume( MixingCategory_Guns, 0.40f );
    mpMixingSnapshots[ snapshot ]->SetVolume( MixingCategory_Footsteps, 0.00f );
    mpMixingSnapshots[ snapshot ]->SetVolume( MixingCategory_Birds, 0.08f );
    mpMixingSnapshots[ snapshot ]->SetVolume( MixingCategory_Cars, 0.12f );
    mpMixingSnapshots[ snapshot ]->SetVolume( MixingCategory_Dialogue, 1.00f );
    mpMixingSnapshots[ snapshot ]->SetIsInstalled( false );

    snapshot = MixingSnapshots_Modifying2; 
    mpMixingSnapshots[ snapshot ] = new MixingSnapshot( MixingLayer_Modifying );
    mpMixingSnapshots[ snapshot ]->SetVolume( MixingCategory_Ambience, 0.00f );
    mpMixingSnapshots[ snapshot ]->SetVolume( MixingCategory_Guns, 0.00f );
    mpMixingSnapshots[ snapshot ]->SetVolume( MixingCategory_Footsteps, 0.00f );
    mpMixingSnapshots[ snapshot ]->SetVolume( MixingCategory_Birds, 0.00f );
    mpMixingSnapshots[ snapshot ]->SetVolume( MixingCategory_Cars, 0.00f );
    mpMixingSnapshots[ snapshot ]->SetVolume( MixingCategory_Dialogue, 0.00f );
    mpMixingSnapshots[ snapshot ]->SetIsInstalled( false );

    snapshot = MixingSnapshots_Modifying3; 
    mpMixingSnapshots[ snapshot ] = new MixingSnapshot( MixingLayer_Modifying );
    mpMixingSnapshots[ snapshot ]->SetVolume( MixingCategory_Ambience, 0.35f );
    mpMixingSnapshots[ snapshot ]->SetVolume( MixingCategory_Guns, 0.31f );
    mpMixingSnapshots[ snapshot ]->SetVolume( MixingCategory_Footsteps, 1.00f );
    mpMixingSnapshots[ snapshot ]->SetVolume( MixingCategory_Birds, 0.22f );
    mpMixingSnapshots[ snapshot ]->SetVolume( MixingCategory_Cars, 0.35f );
    mpMixingSnapshots[ snapshot ]->SetVolume( MixingCategory_Dialogue, 0.29f );
    mpMixingSnapshots[ snapshot ]->SetIsInstalled( false );
}

//----------------------------------------------------------------------------
Demo::~Demo( void )
{
    for ( unsigned int i = 0; i < MixingCategory_Count; i++ )
    {
        if ( NULL != mpSoundStreamers[ i ] )
        {
            delete mpSoundStreamers[ i ];
            mpSoundStreamers[ i ] = NULL;
        }
    }

    for ( unsigned int i = 0; i < MixingCategory_Count; i++ )
    {
        if ( NULL != mpSoundFiles[ i ] )
        {
            delete mpSoundFiles[ i ];
            mpSoundFiles[ i ] = NULL;
        }
    }

    for ( unsigned int i = 0; i < MixingSnapshots_Count; i++ )
    {
        delete mpMixingSnapshots[ i ];
        mpMixingSnapshots[ i ] = NULL;
    }
}

//----------------------------------------------------------------------------
void Demo::Update( void )
{
    // Update the streamers
    for ( unsigned int i = 0; i < MixingCategory_Count; i++ )
    {
        UpdateSoundStreamer( ( MixingCategory ) i );
    }
        
    // Update the mixing system
    MixingSystem::Get( )->Update( );
}

//----------------------------------------------------------------------------
void Demo::UpdateSoundStreamer( MixingCategory mixingCategory )
{
    SoundStreamer* pStreamer = mpSoundStreamers[ mixingCategory ];
    
    if ( NULL == pStreamer )
    {
        return;
    }

    if ( !pStreamer->GetIsPlaying( ) )
    {
        if ( NULL != mpSoundFiles[ mixingCategory ] )
        {
            delete mpSoundFiles[ mixingCategory ];
        }
        mpSoundFiles[ mixingCategory ] = new SoundFile( );
        mpSoundFiles[ mixingCategory ]->Open( FILE_NAMES[ mixingCategory ] );
        pStreamer->SetFile( mpSoundFiles[ mixingCategory ], SOUND_STREAMER_BUFFER_SIZE );
        pStreamer->Play( );
    }

    pStreamer->Update( );
}
