#include "entity.h"

#include "meta_scriptable.h"


META_SCRIPTABLE_Class( ENTITY )
{
    META_SCRIPTABLE_ResultMethod( GetHealth, float )
    META_SCRIPTABLE_ResultMethod( GetPosition, VECTOR_3 )
    META_SCRIPTABLE_VoidMethod1( SetPosition, VECTOR_3 )
}
META_SCRIPTABLE_End()

ENTITY::ENTITY(
    void
    ) :
    COUNTED_OBJECT()
{
}

// ~~

ENTITY::~ENTITY(
    void
    )
{
}
