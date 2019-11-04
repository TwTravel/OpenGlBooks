#include "world.h"

// -- LOCAL

// .. REFERENCES

#include "meta_scriptable.h"

// -- PUBLIC

// .. CONSTRUCTORS

META_SCRIPTABLE_Class( WORLD )
{
    META_SCRIPTABLE_RenamedResultSingletonMethod1( GetEntityAtName, GetEntity, ENTITY, std::string )
    META_SCRIPTABLE_VoidSingletonMethod2( AddEntity, ENTITY, std::string )
}
META_SCRIPTABLE_End()

WORLD::WORLD(
    void
    ) :
    COUNTED_OBJECT()
{
}

// ~~

WORLD::~WORLD(
    void
    )
{
}

// .. OPERATIONS

WORLD
    *WORLD::Instance = NULL;