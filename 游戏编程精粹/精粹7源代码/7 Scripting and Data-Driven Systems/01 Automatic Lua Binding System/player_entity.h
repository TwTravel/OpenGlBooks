#ifndef __PLAYER_ENTITY__
    #define __PLAYER_ENTITY__

    // -- LOCAL

    // .. REFERENCES
        
    #include "entity.h"

    // -- GLOBAL

    // .. TYPES

    class PLAYER_ENTITY :
        public ENTITY
    {
    public:

        META_SCRIPTABLE_DefineClass( PLAYER_ENTITY )

        PLAYER_ENTITY( 
            float health
            )
        {
            Health = health;
        }

        virtual float GetHealth()
        {
            return Health;
        }

    private:

        float
            Health;
    };
#endif
