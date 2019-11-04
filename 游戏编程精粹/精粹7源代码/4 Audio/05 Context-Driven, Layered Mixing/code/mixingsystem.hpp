//----------------------------------------------------------------------------
// File: mixingsystem.hpp
// Copyright (C) 2007, Robert Sparks
//----------------------------------------------------------------------------
#ifndef MIXINGSYSTEM_HPP
#define MIXINGSYSTEM_HPP

#include "mixingsnapshot.hpp"

//----------------------------------------------------------------------------
class MixingSystem
{
    public:
        static void Initialize( void );
        static void Terminate( void );
        static MixingSystem* Get( void ) { return spMixingSystem; }
        
        // Access the volume levels of the central mix
        float GetCategoryVolume( MixingCategory category ) { return mCentralMix[ category ]; }
        
        void Update( void );

    private:

        MixingSystem( void );
        ~MixingSystem( void );
        
        static MixingSystem* spMixingSystem;
        
        float mCentralMix[ MixingCategory_Count ];
};

#endif // MIXINGSYSTEM_HPP
