#include "script_manager.h"

// -- LOCAL

// .. REFERENCES

#include "counted_object.h"
#include "counted_ref_to_.h"
#include "meta_scriptable.h"
#include "meta_scriptable_type_table.h"
#include "script.h"
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

// -- PUBLIC

// .. CONSTRUCTORS

SCRIPT_MANAGER::SCRIPT_MANAGER(
    void
    ) :
    COUNTED_OBJECT()
{
    META_SCRIPTABLE_TYPE_TABLE::Register( BindingData );
}

// ~~

SCRIPT_MANAGER::~SCRIPT_MANAGER(
    void
    )
{
}

// .. OPERATIONS

COUNTED_REF_TO_<SCRIPT> SCRIPT_MANAGER::CreateScript(
    void
    )
{
    SCRIPT
        * script;

    script = new SCRIPT( *this );

    return script;
}

