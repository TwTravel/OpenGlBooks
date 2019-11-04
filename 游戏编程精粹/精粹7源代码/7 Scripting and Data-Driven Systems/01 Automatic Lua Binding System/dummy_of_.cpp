#include "dummy_of_.h"

// -- LOCAL

// .. REFERENCES

#include "meta_scriptable.h"
#include "entity.h"

// -- PUBLIC

// .. META

META_SCRIPTABLE_UncountedTemplateClass( DUMMY_OF_ )
{
    META_SCRIPTABLE_Constructor1( _TEMPLATE_ )
}
META_SCRIPTABLE_End()

// .. CONSTRUCTORS

template< typename _ITEM_>
DUMMY_OF_<_ITEM_>::DUMMY_OF_(
    void
    )
{
}

// ~~

template< typename _ITEM_>
DUMMY_OF_<_ITEM_>::~DUMMY_OF_(
    void
    )
{
}

META_SCRIPTABLE_InstantiateTemplateClass( DUMMY_OF_, ENTITY );