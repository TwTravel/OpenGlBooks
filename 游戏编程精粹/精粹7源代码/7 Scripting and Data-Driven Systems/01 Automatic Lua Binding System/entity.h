#ifndef __ENTITY__
    #define __ENTITY__

    // -- LOCAL

    // .. REFERENCES

    #include "counted_object.h"
    #include "meta_scriptable.h"
    #include "counted_ref_to_.h"
    #include "vector_3.h"

    // -- GLOBAL

    // .. TYPES

    class ENTITY :
        public COUNTED_OBJECT
    {

    public :

        META_SCRIPTABLE_DefineClass( ENTITY )

        ENTITY(
            void
            );

        virtual ~ENTITY(
            void
            );

        virtual float GetHealth() = 0;

        const VECTOR_3 & GetPosition(
            void
            )
        {
            return Position;
        }

        // ~~

        void SetPosition(
            const VECTOR_3 & position
            )
        {
            Position = position;
        }

        // -- PRIVATE

    private:

        // .. ATTRIBUTES

        VECTOR_3
            Position;
    };

    typedef COUNTED_REF_TO_<ENTITY> ENTITY_REF;
#endif
