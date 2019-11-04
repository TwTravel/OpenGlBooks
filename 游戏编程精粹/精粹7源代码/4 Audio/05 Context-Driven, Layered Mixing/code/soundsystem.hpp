//----------------------------------------------------------------------------
// File: soundsystem.hpp
// Copyright (C) 2007, Robert Sparks
//----------------------------------------------------------------------------
#ifndef SOUNDSYSTEM_HPP
#define SOUNDSYSTEM_HPP

#include <dsound.h>

//----------------------------------------------------------------------------
class SoundSystem
{
    public:
        void CreateDirectSoundBuffer( IDirectSoundBuffer** ppIDirectSoundBuffer, WAVEFORMATEX* pWfx, unsigned int bufferBytes, bool isPositional );

        static void Initialize( HWND hWnd );
        static void Terminate( void );
        static SoundSystem* Get( void ) { return spSoundSystem; }

    private:

        SoundSystem( HWND hWnd );
        ~SoundSystem( void );
        
        static SoundSystem* spSoundSystem;
    
        IDirectSound8* mpDirectSound;
        IDirectSoundBuffer* mpPrimaryDirectSoundBuffer;
};

#endif // SOUNDSYSTEM_HPP
