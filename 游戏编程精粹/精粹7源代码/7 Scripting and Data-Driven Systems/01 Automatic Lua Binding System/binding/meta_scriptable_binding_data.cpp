#include "meta_scriptable_binding_data.h"

// -- LOCAL

// .. REFERENCES

#include "counted_object.h"

// -- PUBLIC

// .. CONSTRUCTORS

META_SCRIPTABLE_BINDING_DATA::META_SCRIPTABLE_BINDING_DATA(
    void
    )
{
    SetNoParentClass( GetClassIndex( COUNTED_OBJECT::META_SCRIPT::GetClassName() ) );
}

// ~~

META_SCRIPTABLE_BINDING_DATA::~META_SCRIPTABLE_BINDING_DATA(
    void
    )
{
}

// .. OPERATIONS

// ~~

bool META_SCRIPTABLE_BINDING_DATA::CheckType(
	const int class_index,
	const int base_class_index
	)
{
	int
		current_class_index = class_index;

	while( current_class_index != -1 )
	{
		if( current_class_index == base_class_index )
			return true;

		current_class_index = ParentIndexTable[ current_class_index ];
	}

	return false;
}