#ifndef __META_SCRIPTABLE_TYPE_TABLE__
    #define __META_SCRIPTABLE_TYPE_TABLE__

    // -- LOCAL

    // .. REFERENCES

    #include "meta_scriptable_type.h"

    // -- GLOBAL

    // .. CONSTANTS

    #define META_SCRIPTABLE_TYPE_TABLE_MaximumTypeCount    1024

    // .. TYPES

    class META_SCRIPTABLE_TYPE_TABLE
    {
        // -- PUBLIC

    public :

        // .. CONSTRUCTORS

        META_SCRIPTABLE_TYPE_TABLE(
            void
            );

        // ~~

        META_SCRIPTABLE_TYPE_TABLE(
            const META_SCRIPTABLE_TYPE_TABLE & other
            );

        // ~~

        ~META_SCRIPTABLE_TYPE_TABLE(
            void
            );

        // .. OPERATORS

        META_SCRIPTABLE_TYPE_TABLE & operator=(
            const META_SCRIPTABLE_TYPE_TABLE & other
            );

        // .. FUNCTIONS

        static void AddType(
            META_SCRIPTABLE_TYPE & type
            );

        // .. 

        static void Register( 
            META_SCRIPTABLE_BINDING_DATA & binding
            );

        // -- PRIVATE

    private :

        // .. VARIABLES

        static int
            TypeCount;
        static META_SCRIPTABLE_TYPE
            * TypeArray[ META_SCRIPTABLE_TYPE_TABLE_MaximumTypeCount ];
    };
#endif
