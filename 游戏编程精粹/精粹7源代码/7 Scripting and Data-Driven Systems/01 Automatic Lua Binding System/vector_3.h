#ifndef __VECTOR_3__
    #define __VECTOR_3__

    // -- LOCAL

    // .. REFERENCES

    #include "meta_scriptable.h"

    // -- GLOBAL

    // .. TYPES

    class VECTOR_3
    {
        // -- PUBLIC

    public :

        // .. META

        META_SCRIPTABLE_DefineRawClass( VECTOR_3 )

        // .. CONSTRUCTORS

        VECTOR_3(
            void
            )
        {
        }

        // ~~

        VECTOR_3(
            float x,
            float y,
            float z
            ) :
            X( x ),
            Y( y ),
            Z( z )
        {
        }

        // ~~

        ~VECTOR_3(
            void
            )
        {
        }

        // .. ATTRIBUTES

        float
            X,
            Y,
            Z;
    };
#endif
