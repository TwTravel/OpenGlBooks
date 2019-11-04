#ifndef __META_SCRIPTABLE_TYPE__
    #define __META_SCRIPTABLE_TYPE__

    // -- LOCAL

    // .. REFERENCES

    #include <lua.h>
    #include "meta_scriptable_binding_data.h"

    // -- GLOBAL

    // .. TYPES

    class META_SCRIPTABLE_TYPE
    {
        // -- PUBLIC

    public :

        // .. CONSTRUCTORS

        META_SCRIPTABLE_TYPE(
            void
            );

        // ~~

        META_SCRIPTABLE_TYPE(
            const META_SCRIPTABLE_TYPE & other
            );

        // ~~

        ~META_SCRIPTABLE_TYPE(
            void
            );

        // .. OPERATORS

        META_SCRIPTABLE_TYPE & operator=(
            const META_SCRIPTABLE_TYPE & other
            );

        // .. OPERATIONS

        virtual const char * GetName(
            void
            ) const = 0;

        // ~~

        virtual int & GetIndex(
            void
            ) = 0;

        // ~~

        virtual void Register( 
            META_SCRIPTABLE_BINDING_DATA & binding
            ) = 0;
    };
#endif
