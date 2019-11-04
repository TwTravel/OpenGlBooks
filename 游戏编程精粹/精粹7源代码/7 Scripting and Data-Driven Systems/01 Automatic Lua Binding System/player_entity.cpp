#include "player_entity.h"

// -- LOCAL

// .. REFERENCES

// -- PUBLIC

META_SCRIPTABLE_InheritedClass( PLAYER_ENTITY, ENTITY )
{
    META_SCRIPTABLE_Constructor1( float )
    META_SCRIPTABLE_ResultMethod( GetHealth, float )
}
META_SCRIPTABLE_End()


