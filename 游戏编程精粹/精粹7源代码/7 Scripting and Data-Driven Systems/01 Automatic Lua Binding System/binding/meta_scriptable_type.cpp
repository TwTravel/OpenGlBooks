#include "meta_scriptable_type.h"

// -- LOCAL

// .. REFERENCES

#include "meta_scriptable_type_table.h"

// -- PUBLIC

// .. CONSTRUCTORS

META_SCRIPTABLE_TYPE::META_SCRIPTABLE_TYPE(
    void
    )
{
    META_SCRIPTABLE_TYPE_TABLE::AddType( *this );
}

// ~~

META_SCRIPTABLE_TYPE::~META_SCRIPTABLE_TYPE(
    void
    )
{
}