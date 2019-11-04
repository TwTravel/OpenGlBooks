//----------------------------------------------------------------------------
// File: demo.hpp
// Copyright (C) 2007, Robert Sparks
//----------------------------------------------------------------------------
#ifndef DEMO_HPP
#define DEMO_HPP

//----------------------------------------------------------------------------
#include "soundstreamer.hpp"

//----------------------------------------------------------------------------
// The demo consists of a set of mixing snapshots. 
//----------------------------------------------------------------------------
class Demo
{
    public:

        enum MixingSnapshots
        {
            MixingSnapshots_PreMix0 = 0,
            MixingSnapshots_Base0,
            MixingSnapshots_Base1,
            MixingSnapshots_Base2,
            MixingSnapshots_Modifying0,
            MixingSnapshots_Modifying1,
            MixingSnapshots_Modifying2,
            MixingSnapshots_Modifying3,
            MixingSnapshots_Count
        };

        // Construction/Destruction
        Demo( void );
        ~Demo( void );

        // Access the mixing snapshots
        MixingSnapshot* GetMixingSnapshot( MixingSnapshots snapshot ) { return mpMixingSnapshots[ snapshot ]; }

        // Update the demo and keep it going.  
        void Update( void );
        
    private:     
    
        // Update the streamer and keep it filled with fresh data
        void UpdateSoundStreamer( MixingCategory mixingCategory );
        
        // Sound streamers
        SoundStreamer* mpSoundStreamers[ MixingCategory_Count ];
        
        // Sound files
        SoundFile* mpSoundFiles[ MixingCategory_Count ];

        // The demo maintains a set of mixing snapshots.  These will be 
        // edited and installed by the user.
        MixingSnapshot* mpMixingSnapshots[ MixingSnapshots_Count ];
};

#endif // DEMO_HPP