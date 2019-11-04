//----------------------------------------------------------------------------
// File: mixingsystem.hpp
// Copyright (C) 2007, Robert Sparks
//----------------------------------------------------------------------------
#include "mixingsystem.hpp"
#include <windows.h>
#include "util.hpp"

//----------------------------------------------------------------------------
MixingSystem* MixingSystem::spMixingSystem = NULL;

//----------------------------------------------------------------------------
void MixingSystem::Initialize( void )
{
    ASSERT( NULL == spMixingSystem );
    spMixingSystem = new MixingSystem( );
}

//----------------------------------------------------------------------------
void MixingSystem::Terminate( void )
{
    if ( NULL != spMixingSystem )
    {
        delete spMixingSystem;
        spMixingSystem = NULL;
    }
}    

//----------------------------------------------------------------------------
MixingSystem::MixingSystem( void )
{
    for ( unsigned int i = 0; i < MixingCategory_Count; i++ )
    {
        mCentralMix[ i ] = 0.0f;
    }
}

//----------------------------------------------------------------------------
MixingSystem::~MixingSystem( void )
{
}   

//----------------------------------------------------------------------------
void MixingSystem::Update( void )
{
    // Set up some working mixes.
    float nextMix[ MixingCategory_Count ];
    float targetMix[ MixingCategory_Count ];
    for ( unsigned int i = 0; i < MixingCategory_Count; i++ )
    {
        nextMix[ i ] = 1.0f;
        targetMix[ i ] = 1.0f;
    }

    // Update the snapshots and calculate new mix values
    MixingSnapshot* pSearch = MixingSnapshot::GetHead( );
    while( NULL != pSearch )
    {
        pSearch->Update( );
        
        for ( unsigned int i = 0; i < MixingCategory_Count; i++ )
        {
            nextMix[ i ] *= pSearch->CalculateCurrentMixingVolume( ( MixingCategory ) i );
            targetMix[ i ] *= pSearch->CalculateTargetMixingVolume( ( MixingCategory ) i );
        }
        
        pSearch = pSearch->GetNext( );
    }
    
    // Now pass on the values to the central mix.  Make sure that 
    // the faders only move in a direction towards their eventual
    // target values.
    for ( unsigned int i = 0; i < MixingCategory_Count; i++ )
    {
        if ( targetMix[ i ] > mCentralMix[ i ] )
        {
            if ( targetMix[ i ] >= nextMix[ i ] && nextMix[ i ] > mCentralMix[ i ] )
            {
                mCentralMix[ i ] = nextMix[ i ];
            }
        }
        else if ( targetMix[ i ] < mCentralMix[ i ] )
        {
            if ( targetMix[ i ] <= nextMix[ i ] && nextMix[ i ] < mCentralMix[ i ] )
            {
                mCentralMix[ i ] = nextMix[ i ];
            }
        }
        else
        {
            mCentralMix[ i ] = targetMix[ i ];
        }
    }
    
}

