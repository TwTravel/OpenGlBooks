//----------------------------------------------------------------------------
// File: mixingsnapshot.hpp
// Copyright (C) 2007, Robert Sparks
//----------------------------------------------------------------------------
#ifndef MIXINGSNAPSHOT_HPP
#define MIXINGSNAPSHOT_HPP

//----------------------------------------------------------------------------
enum MixingCategory
{
    MixingCategory_Ambience = 0,
    MixingCategory_Guns,
    MixingCategory_Footsteps,
    MixingCategory_Birds,
    MixingCategory_Cars,
    MixingCategory_Dialogue,
    MixingCategory_Count   
};

enum MixingLayer
{
    MixingLayer_PreMix = 0,
    MixingLayer_Base,
    MixingLayer_Modifying,
    MixingLayer_Count
};

//----------------------------------------------------------------------------
class MixingSnapshot
{
    public:
    
        MixingSnapshot( MixingLayer layer );
        ~MixingSnapshot( void );
        
        void SetVolume( MixingCategory category, float volume ) { mVolumes[ category ] = volume; }
        float GetVolume( MixingCategory category ) { return mVolumes[ category ]; }
        
        void SetFadeinMs( float fadeinMs ) { mFadeinMs = fadeinMs; }
        float GetFadeinMs( void ) { return mFadeinMs; }
        
        void SetFadeoutMs( float fadeoutMs ) { mFadeoutMs = fadeoutMs; }
        float GetFadeoutMs( void ) { return mFadeoutMs; }
        
        MixingLayer GetMixingLayer( void ) { return mLayer; }
        
        bool GetIsInstalled( void ) { return mIsInstalled; }
        void SetIsInstalled( bool isInstalled ) { mIsInstalled = isInstalled; }
        
        // Calculate the current contibution to the central mix.
        // This takes into account fading in and out.
        float CalculateCurrentMixingVolume( MixingCategory category );
        
        // Calculate what the volume will be once fading
        // has completed.
        float CalculateTargetMixingVolume( MixingCategory category );
        
        void Update( void );

        MixingSnapshot* GetNext( void ) { return mpNext; }
        MixingSnapshot* GetPrev( void ) { return mpPrev; }
        static MixingSnapshot* GetHead( void ) { return spHead; }
        
        // Only one base layer snapshot can be installed at a time.
        // After new snapshots are installed, this function clears out
        // any out of date snapshots.
        static void CleanBaseLayer( void );
        
    private:
        
        static MixingSnapshot* spHead;
        static MixingSnapshot* spBaseLayerSnapshot;
    
        float mVolumes[ MixingCategory_Count ];
        float mFadeinMs;
        float mFadeoutMs;
        float mFader;
        MixingLayer mLayer;
        bool mIsInstalled;
        MixingSnapshot* mpNext;
        MixingSnapshot* mpPrev;
};

#endif // MIXINGSNAPSHOT_HPP
