//----------------------------------------------------------------------------
// File: mixingsnapshot.cpp
// Copyright (C) 2007, Robert Sparks
//----------------------------------------------------------------------------
#include "mixingsnapshot.hpp"
#include <windows.h>

//----------------------------------------------------------------------------
// Hard coded for this example.  In a game this
// would be calculated dynamically.
static const float FADER_DELTA = 0.033f;  

//----------------------------------------------------------------------------
MixingSnapshot* MixingSnapshot::spHead = NULL;
MixingSnapshot* MixingSnapshot::spBaseLayerSnapshot = NULL;

//----------------------------------------------------------------------------
MixingSnapshot::MixingSnapshot( MixingLayer layer )
    :
    mFadeinMs( 1000.0f ),
    mFadeoutMs( 1000.0f ),
    mFader( 0.0f ),
    mLayer( layer ),
    mIsInstalled( false ),
    mpNext( NULL ),
    mpPrev( NULL )
{
    for ( unsigned int i = 0; i < MixingCategory_Count; i++ )
    {
        mVolumes[ i ] = 1.0f;
    }
    
    if ( NULL != spHead )
    {
        mpNext = spHead;
        spHead->mpPrev = this;
    }
    spHead = this;
}

//----------------------------------------------------------------------------
MixingSnapshot::~MixingSnapshot( void )
{   
    if ( this == spBaseLayerSnapshot )
    {
        spBaseLayerSnapshot = NULL;
    }

    if ( NULL != mpNext )
    {
        mpNext->mpPrev = mpPrev;
    }
    if ( NULL != mpPrev )
    {
        mpPrev->mpNext = mpNext;
    }
    else
    {
        spHead = mpNext;
    }
    mpNext = NULL;
    mpPrev = NULL;
}

//----------------------------------------------------------------------------
void MixingSnapshot::CleanBaseLayer( void )
{
    // Clear the pointer if it's out of date.
    if ( NULL != spBaseLayerSnapshot && !spBaseLayerSnapshot->GetIsInstalled( ) )
    {
        spBaseLayerSnapshot;
    }

    // See if a new one has been selected.
    MixingSnapshot* pNewBase = spBaseLayerSnapshot;
    MixingSnapshot* pSearch = GetHead( );
    while( NULL != pSearch )
    {
        if ( MixingLayer_Base == pSearch->GetMixingLayer( ) && pSearch->GetIsInstalled( ) )
        {
            if ( spBaseLayerSnapshot != pSearch )
            {
                pNewBase = pSearch;                
            }            
        }
        
        pSearch = pSearch->GetNext( );
    }
    spBaseLayerSnapshot = pNewBase;
    
    // Clear remaining snapshots
    pSearch = GetHead( );
    while( NULL != pSearch )
    {
        if ( MixingLayer_Base == pSearch->GetMixingLayer( ) && pSearch != spBaseLayerSnapshot )
        {
            pSearch->SetIsInstalled( false );
        }
        pSearch = pSearch->GetNext( );
    }
}

//----------------------------------------------------------------------------
float MixingSnapshot::CalculateCurrentMixingVolume( MixingCategory category )
{
    // Use the fader value to move between 1.0f and the value
    // specified for this snapshot.
    float inverseFader = 1.0f - mFader;
    return mVolumes[ category ] + inverseFader * ( 1.0f - mVolumes[ category ] );
}

//----------------------------------------------------------------------------
float MixingSnapshot::CalculateTargetMixingVolume( MixingCategory category )
{
    if ( mIsInstalled )
    {
        return mVolumes[ category ];
    }
    else
    {
        return 1.0f;
    }
}

//----------------------------------------------------------------------------
void MixingSnapshot::Update( void )
{
    if ( mIsInstalled )
    {
        mFader += FADER_DELTA;
        if ( mFader > 1.0f )
        {
            mFader = 1.0f;
        }
    }
    else
    {
        mFader -= FADER_DELTA;
        if ( mFader < 0.0f )
        {
            mFader = 0.0f;
        }
    }        
}
