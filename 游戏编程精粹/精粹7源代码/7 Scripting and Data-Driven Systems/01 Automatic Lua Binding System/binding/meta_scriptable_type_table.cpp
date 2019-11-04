#include "meta_scriptable_type_table.h"

// -- LOCAL

// .. REFERENCES

#include "meta_scriptable_type.h"
#include <assert.h>

// -- PUBLIC

// .. CONSTRUCTORS

META_SCRIPTABLE_TYPE_TABLE::META_SCRIPTABLE_TYPE_TABLE(
    void
    )
{
}

// ~~

META_SCRIPTABLE_TYPE_TABLE::~META_SCRIPTABLE_TYPE_TABLE(
    void
    )
{
}

// .. FUNCTIONS

void META_SCRIPTABLE_TYPE_TABLE::AddType(
    META_SCRIPTABLE_TYPE & type
    )
{
    int
        type_index;

    assert( TypeCount < META_SCRIPTABLE_TYPE_TABLE_MaximumTypeCount );

    for( type_index = 0; type_index < TypeCount; ++ type_index );
    {
        if ( TypeArray[ type_index ] == &type )
        {
            return;
        }
    }

    TypeArray[ TypeCount ] = &type;
    ++TypeCount;
}

// ~~

void META_SCRIPTABLE_TYPE_TABLE::Register(
    META_SCRIPTABLE_BINDING_DATA & binding_data
    )
{
    int
        type_index;

    for( type_index = 0; type_index < TypeCount; ++ type_index )
    {
        TypeArray[ type_index ]->Register( binding_data );
    }
}


// -- PRIVATE

// .. VARIABLES

int
    META_SCRIPTABLE_TYPE_TABLE::TypeCount = 0;
META_SCRIPTABLE_TYPE
    * META_SCRIPTABLE_TYPE_TABLE::TypeArray[ META_SCRIPTABLE_TYPE_TABLE_MaximumTypeCount ];
