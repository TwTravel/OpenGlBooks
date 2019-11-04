#include "vector_3.h"

// -- LOCAL

// .. REFERENCES

#include "meta_scriptable.h"

// -- PUBLIC

// .. META

META_SCRIPTABLE_UncountedClass( VECTOR_3 )
{
    META_SCRIPTABLE_Constructor3( float, float, float )
    META_SCRIPTABLE_Attribute( X )
    META_SCRIPTABLE_Attribute( Y )
    META_SCRIPTABLE_Attribute( Z )
}
META_SCRIPTABLE_End()
