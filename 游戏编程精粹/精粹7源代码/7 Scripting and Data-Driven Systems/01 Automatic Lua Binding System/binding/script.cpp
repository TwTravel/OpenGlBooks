#include "script.h"

// -- LOCAL

// .. REFERENCES

#include "meta_scriptable.h"
#include "meta_scriptable_binding_data.h"
#include "meta_scriptable_type_table.h"
#include "counted_object.h"
#include "script_manager.h"
#include <lua.h>
#include <luaconf.h>
#include <lualib.h>
#include <lauxlib.h>

// .. CONSTANTS

// -- PUBLIC

// .. META

META_SCRIPTABLE_InheritedClass( SCRIPT, COUNTED_OBJECT )
{
    META_SCRIPTABLE_RenamedVoidMethod2( SetValue, SetReal, float, META_SCRIPTABLE_CHAR_POINTER )
    META_SCRIPTABLE_RenamedVoidMethod2( SetValue, SetInteger, int, META_SCRIPTABLE_CHAR_POINTER )
    META_SCRIPTABLE_RenamedVoidMethod2( SetValue, SetBoolean, bool, META_SCRIPTABLE_CHAR_POINTER )
    META_SCRIPTABLE_RenamedVoidMethod2( SetValue, SetText, META_SCRIPTABLE_CHAR_POINTER, META_SCRIPTABLE_CHAR_POINTER )
}
META_SCRIPTABLE_End()

// .. CONSTRUCTORS

SCRIPT::SCRIPT(
    SCRIPT_MANAGER & script_manager
    ) :
    COUNTED_OBJECT(),
    Manager( &script_manager )
{
    ThreadState = luaL_newstate();

    *( ( const META_SCRIPTABLE_BINDING_DATA** )lua_getuserdata( ThreadState ) ) = &script_manager.GetBindingData();

    lua_pop( ThreadState, luaopen_base( ThreadState ) );
    lua_pop( ThreadState, luaopen_math( ThreadState ) );
    lua_pop( ThreadState, 1 );

    // Save a pointer to the thread Manager object in the global table
    // using the new thread script vm pointer as a key.

    lua_pushlightuserdata( ThreadState, ThreadState );
    lua_pushlightuserdata( ThreadState, this );
    lua_settable( ThreadState, LUA_GLOBALSINDEX );

    META_SCRIPTABLE_LUA_REGISTERER::Init( ThreadState );
}

// ~~

SCRIPT::~SCRIPT(
    void
    )
{
    lua_close( ThreadState );
}

// .. INQUIRIES

bool SCRIPT::DoesFunctionExist(
    const char * function_name
    ) const
{
    bool
        it_is_function;

    lua_getglobal( ThreadState, function_name );
    it_is_function = !lua_isnil( ThreadState, -1 ) && lua_isfunction( ThreadState, -1 );
    lua_pop( ThreadState, 1 );

    return it_is_function;
}

// .. OPERATIONS

bool SCRIPT::LoadFile(
    const char * script_file_path
    )
{
    return luaL_loadfile(ThreadState, script_file_path ) == 0;
}

// ~~

bool SCRIPT::ExecuteFile(
    const char * script_file_path
    )
{
    if ( LoadFile( script_file_path ) )
    {
        return Execute( script_file_path );
    }

    return false;
}

// ~~

bool SCRIPT::LoadText(
    const char * script,
    const int script_length,
    const char * comment
    )
{
    int
        status;

    status = luaL_loadbuffer( ThreadState, script, script_length, comment );

    if ( status != 0 )
    {
        return false;
    }

    return true;
}

// ~~

bool SCRIPT::LoadText(
    const char * script,
    const char * comment
    )
{
    return LoadText( script, int( strlen( script ) ), comment );
}

// ~~

bool SCRIPT::Execute(
    const char * comment
    )
{
    int
        status;

    status = lua_pcall( ThreadState, lua_gettop( ThreadState ) - 1, 0, 0 );

    return status == 0;
}

/*
// ~~

void SCRIPT::ResumeScript(
    const REAL return_value
    )
{
    INT
        status;

    if ( State != SCRIPT_STATE_Error )
    {
        State = SCRIPT_STATE_Running;

        lua_pushnumber( ThreadState, return_value );
        status = lua_resume( ThreadState, 1 );

        // if ( status ) // for LUA 5.0

        if ( status && ( status != LUA_YIELD ) )    // FIXME : for LUA 5.1
        {
            State = SCRIPT_STATE_Error;
            SetLastError();
        }
        else if ( status == 0 )
        {
            State = SCRIPT_STATE_Done;
        }
    }
}
*/

// ~~

void SCRIPT::CollectGarbage(
    void
    )
{
    lua_gc( ThreadState, LUA_GCCOLLECT, 0 );
}

// -- PRIVATE

void SCRIPT::LuaCall(
    int parameter_number,
    int result_count
    )
{
    lua_pcall( ThreadState, parameter_number, result_count, 0 );
}
