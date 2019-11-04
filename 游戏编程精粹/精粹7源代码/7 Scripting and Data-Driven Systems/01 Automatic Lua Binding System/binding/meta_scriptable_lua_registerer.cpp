#include "meta_scriptable_lua_registerer.h"

// -- LOCAL

// .. REFERENCES

#include "meta_scriptable_binding_data.h"
#include "meta_scriptable_type.h"
#include <lua.h>
#include <limits.h>
#include <assert.h>

void META_SCRIPTABLE_LUA_REGISTERER::Init(
    lua_State * lua_state
    )
{
    lua_newtable( lua_state );    // meta table
    lua_pushvalue( lua_state, -1 );
    lua_pushstring( lua_state, "v" );
    lua_setfield( lua_state, -2, "__mode" );    // set as weak-value table
    lua_setmetatable( lua_state, -2 );
    lua_setfield( lua_state, LUA_GLOBALSINDEX, "_objects" );

    // Create a link to binding __index metamethod

    lua_pushvalue( lua_state, LUA_GLOBALSINDEX );
    lua_setmetatable( lua_state, LUA_GLOBALSINDEX );
    lua_pushvalue( lua_state, LUA_GLOBALSINDEX );
    lua_pushcfunction( lua_state, &META_SCRIPTABLE_LUA_REGISTERER::GlobalIndexEventHandler);
    lua_setfield( lua_state, -2, "__index" );
    lua_pop( lua_state, 1 );

	// Create the metamethod that will be used by all C++ object
	lua_pushstring( lua_state, "metatable" );
	lua_newtable( lua_state );
	lua_pushcfunction( lua_state, &META_SCRIPTABLE_LUA_REGISTERER::IndexEventHandler );
	lua_setfield( lua_state, -2, "__index" );
	lua_pushcfunction( lua_state, &META_SCRIPTABLE_LUA_REGISTERER::GarbageCollectionEventHandler );\
	lua_setfield( lua_state, -2, "__gc" );
	lua_rawset( lua_state, LUA_REGISTRYINDEX );
}

// ~~

void META_SCRIPTABLE_LUA_REGISTERER::BeginClass(
    META_SCRIPTABLE_BINDING_DATA & binding,
    int ( *delete_function ) ( lua_State* ),
    META_SCRIPTABLE_TYPE & type
    )
{
    assert( CurrentClass == 0 );
    assert( type.GetIndex() == -1 );

    CurrentClass = &type;
    type.GetIndex() = binding.GetClassIndex( type.GetName() );
	binding.SetDeleteFunction( type.GetIndex(), delete_function );
}

// ~~

void META_SCRIPTABLE_LUA_REGISTERER::EndClass(
    META_SCRIPTABLE_BINDING_DATA & binding,
    const char * class_name,
    const char * base_class_name
    )
{
    int
        parent_class_index;

    if( base_class_name )
    {
        parent_class_index = binding.GetClassIndex( base_class_name );
        binding.SetParentClass( CurrentClass->GetIndex(), parent_class_index );
    }
    else
    {
        binding.SetNoParentClass( CurrentClass->GetIndex() );
    }

    CurrentClass = 0;
}

// ~~

void META_SCRIPTABLE_LUA_REGISTERER::RegisterFunction(
    META_SCRIPTABLE_BINDING_DATA & binding,
    int ( *function ) ( lua_State* ),
    const char * function_name
    )
{
    binding.AddMethodToClass(
        CurrentClass->GetIndex(),
        function_name,
        function
        );
}

// ~~

int META_SCRIPTABLE_LUA_REGISTERER::IndexEventHandler(
    lua_State* lua_state
    )
{

    META_SCRIPTABLE_BINDING_DATA
        * binding;
    META_SCRIPTABLE_BINDING_DATA::BINDING_FUNCTION
        function;
    const char
        * method_name;
    META_SCRIPTABLE_BINDING_HELPER
        * binding_data;
    
    binding = *(META_SCRIPTABLE_BINDING_DATA**)lua_getuserdata( lua_state );

    binding_data = (META_SCRIPTABLE_BINDING_HELPER*)lua_touserdata( lua_state, 1 );
    method_name = lua_tostring( lua_state, 2 );

    function = binding->GetFunction( binding_data->ClassIndex, method_name );
    
    if( !function )
    {
        printf( "META_SCRIPTABLE_LUA_REGISTERER: Method %s is not bound\n", method_name );
        return 0;
    }
    else
    {
        lua_pushcfunction( lua_state, function );
    }

    return 1;
}

// ~~

int META_SCRIPTABLE_LUA_REGISTERER::GarbageCollectionEventHandler(
	lua_State* lua_state
	)
{
	META_SCRIPTABLE_BINDING_DATA
		* binding;
	META_SCRIPTABLE_BINDING_DATA::BINDING_FUNCTION
		function;
	META_SCRIPTABLE_BINDING_HELPER
		* binding_data;

	binding = *(META_SCRIPTABLE_BINDING_DATA**)lua_getuserdata( lua_state );

	binding_data = (META_SCRIPTABLE_BINDING_HELPER*)lua_touserdata( lua_state, 1 );

	function = binding->GetDeleteFunction( binding_data->ClassIndex );
	function( lua_state );
	return 0;
}

// ~~

int META_SCRIPTABLE_LUA_REGISTERER::GlobalIndexEventHandler(
    lua_State* lua_state
    )
{
    META_SCRIPTABLE_BINDING_DATA
        * binding;
    const char
        * class_name;
    int
        class_index;

    binding = *(META_SCRIPTABLE_BINDING_DATA**)lua_getuserdata( lua_state );

    class_name = lua_tostring( lua_state, 2 );

    if( !class_name || !binding->FindClassIndex( class_index, class_name ) )
    {
        lua_pushnil( lua_state );
    }
    else
    {
        META_SCRIPTABLE_BINDING_HELPER
            * binding_data = (META_SCRIPTABLE_BINDING_HELPER*)lua_newuserdata( lua_state, sizeof( META_SCRIPTABLE_BINDING_HELPER ) );

        lua_newtable( lua_state ); // Create Metatable
        lua_pushcfunction( lua_state, &META_SCRIPTABLE_LUA_REGISTERER::IndexEventHandler );
        lua_setfield( lua_state, -2, "__index" );

        lua_setmetatable( lua_state, -2 );

        binding_data->Object = NULL;
        binding_data->ClassIndex = class_index;

        // put a copy in the global table
        lua_pushvalue( lua_state, 2 );
        lua_pushvalue( lua_state, -2 );
        lua_rawset( lua_state, LUA_GLOBALSINDEX );
    }

    return 1;
}

// ~~

void META_SCRIPTABLE_LUA_REGISTERER::PushMetatable(
	lua_State * lua_state 
	)
{
	lua_pushstring( lua_state, "metatable" );
	lua_rawget( lua_state, LUA_REGISTRYINDEX );
}

// .. VARIABLE

META_SCRIPTABLE_TYPE 
    *META_SCRIPTABLE_LUA_REGISTERER::CurrentClass = 0;