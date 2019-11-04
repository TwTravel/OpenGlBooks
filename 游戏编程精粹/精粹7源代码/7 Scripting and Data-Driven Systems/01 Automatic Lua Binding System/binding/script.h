#ifndef __SCRIPT__
    #define __SCRIPT__

    // -- LOCAL

    // .. REFERENCES

    #include "meta_scriptable.h"
    #include "counted_object.h"
    #include "counted_link_to_.h"
    #include "counted_ref_to_.h"
    class SCRIPT_MANAGER;
    #include <lua.h>

    // -- GLOBAL

    // .. TYPES

    class SCRIPT :
        public COUNTED_OBJECT
    {
        // -- PUBLIC

    public :

        // .. META

        META_SCRIPTABLE_DefineClass( SCRIPT )

        // .. CONSTRUCTORS

        SCRIPT(
            void
            );

        // ~~

        SCRIPT(
            const SCRIPT & other
            );

        // ~~

        SCRIPT(
            SCRIPT_MANAGER & script_manager
            );

        // ~~

        virtual ~SCRIPT(
            void
            );

        // .. OPERATORS

        SCRIPT & operator=(
            const SCRIPT & other
            );

        // .. ACCESSORS

        lua_State * GetThreadState(
            void
            )
        {
            return ThreadState;
        }

        // .. INQUIRIES

        SCRIPT_MANAGER & GetManager(
            void
            ) const
        {
            return * Manager;
        }

        // ~~

        bool DoesFunctionExist(
            const char * function_name
            ) const;

        // .. OPERATIONS

        void Update(
            const float time_step
            );

        // ~~

        bool LoadFile(
            const char * script_file_path
            );

        // ~~

        bool ExecuteFile(
            const char * script_file_path
            );

        // ~~

        bool LoadText(
            const char * script,
            const int script_length,
            const char * comment = NULL
            );

        // ~~

        bool LoadText(
            const char * script,
            const char * comment = NULL
            );

        // ~~

        bool Execute(
            const char * comment = NULL
            );

        // ~~

        bool ExecuteText(
            const char * script,
            const int script_length,
            const char * comment = NULL
            )
        {
            return LoadText( script, script_length, comment ) && Execute( comment );
        }

        // ~~

        bool ExecuteText(
            const char * script,
            const char * comment = NULL
            )
        {
            return LoadText( script, comment ) && Execute( comment );
        }

        // ~~

        template< class _TYPE_ >
        void GetValue(
            _TYPE_ & value,
            const char * variable_name
            )
        {
            #if META_SCRIPTABLE_PLATFORM_Lua
                if ( LuaPushLocalAtName( variable_name ) )
                {
                    value = META_SCRIPTABLE_GetValue( ThreadState, -1, ( _TYPE_* )0 );
                    lua_pop( ThreadState, 1 );    // pop local
                }
            #endif
        }

        // ~~

        template< class _TYPE_ >
        void SetValue(
            const _TYPE_ & value,
            const char * variable_name
            )
        {
            #if META_SCRIPTABLE_PLATFORM_Lua
                META_SCRIPTABLE_PushValue( value, ThreadState, ( _TYPE_* )0 );
                lua_setglobal( ThreadState, variable_name );
                //LuaSetLocalAtName( variable_name );
            #endif
        }

        // ~~

        template< class _TYPE_ >
        void GetReturnValue(
            _TYPE_ & value
            )
        {
            #if META_SCRIPTABLE_PLATFORM_Lua
                value = META_SCRIPTABLE_GetValue( ThreadState, -1, ( _TYPE_* )0 );
                lua_pop( ThreadState, 1 );
            #endif
        }

        // ~~
/*
        template< class _TYPE_ >
        void GetReturnValue(
            META_REF_TO_<_TYPE_> & value
            )
        {
            #if META_SCRIPTABLE_PLATFORM_Lua
                value = &META_SCRIPTABLE_GetValue( ThreadState, -1, ( _TYPE_* )0 );
                lua_pop( ThreadState, 1 );
            #endif
        } */

        // ~~

        void CallFunction(
            const char * function_name
            )
        {
            lua_getglobal( ThreadState, function_name );

            LuaCall( 0 );
        }

        // ~~

        void CallObjectFunction(
            char * object,
            char * function_name
            )
        {
            lua_getglobal( ThreadState, object );
            lua_pushstring( ThreadState, function_name );
            lua_gettable( ThreadState, -2 );
            lua_pushvalue( ThreadState, -2 );

            LuaCall( 1 );
            lua_pop( ThreadState, 1 );
        }

        // ~~

        template< class _PARAMETER_1_ >
        void CallFunction(
            char * function_name,
            const _PARAMETER_1_& parameter_1
            )
        {
            lua_getglobal( ThreadState, function_name );
            META_SCRIPTABLE_PushValue( parameter_1, ThreadState, ( _PARAMETER_1_* ) 0 );

            LuaCall( 1 );
        }

        // ~~

        template< class _PARAMETER_1_ >
        void CallObjectFunction(
            char * object,
            char * function_name,
            const _PARAMETER_1_& parameter_1
            )
        {
            lua_getglobal( ThreadState, object );
            lua_pushstring( ThreadState, function_name );
            lua_gettable( ThreadState, -2 );
            lua_pushvalue( ThreadState, -2 );
            META_SCRIPTABLE_PushValue( parameter_1, ThreadState, ( _PARAMETER_1_* ) 0 );

            LuaCall( 2 );
            lua_pop( ThreadState, 1 );
        }

        // ~~

        template< class _PARAMETER_1_, class _PARAMETER_2_ >
        void CallFunction(
            char * function_name,
            const _PARAMETER_1_& parameter_1,
            const _PARAMETER_2_& parameter_2
            )
        {
            lua_getglobal( ThreadState, function_name );
            META_SCRIPTABLE_PushValue( parameter_1, ThreadState, ( _PARAMETER_1_* ) 0 );
            META_SCRIPTABLE_PushValue( parameter_2, ThreadState, ( _PARAMETER_2_* ) 0 );

            LuaCall( 2 );
        }

        // ~~

        template< class _PARAMETER_1_, class _PARAMETER_2_ >
        void CallObjectFunction(
            char * object,
            char * function_name,
            const _PARAMETER_1_& parameter_1,
            const _PARAMETER_2_& parameter_2
            )
        {
            lua_getglobal( ThreadState, object );
            lua_pushstring( ThreadState, function_name );
            lua_gettable( ThreadState, -2 );
            lua_pushvalue( ThreadState, -2 );
            META_SCRIPTABLE_PushValue( parameter_1, ThreadState, ( _PARAMETER_1_* ) 0 );
            META_SCRIPTABLE_PushValue( parameter_2, ThreadState, ( _PARAMETER_2_* ) 0 );

            LuaCall( 3 );
            lua_pop( ThreadState, 1 );
        }

        // ~~

        template< class _PARAMETER_1_, class _PARAMETER_2_, class _PARAMETER_3_ >
        void CallFunction(
            char * function_name,
            const _PARAMETER_1_& parameter_1,
            const _PARAMETER_2_& parameter_2,
            const _PARAMETER_3_& parameter_3
            )
        {
            lua_getglobal( ThreadState, function_name );
            META_SCRIPTABLE_PushValue( parameter_1, ThreadState, ( _PARAMETER_1_* ) 0 );
            META_SCRIPTABLE_PushValue( parameter_2, ThreadState, ( _PARAMETER_2_* ) 0 );
            META_SCRIPTABLE_PushValue( parameter_3, ThreadState, ( _PARAMETER_3_* ) 0 );

            LuaCall( 3 );
        }

        // ~~

        template< class _PARAMETER_1_, class _PARAMETER_2_, class _PARAMETER_3_ >
        void CallObjectFunction(
            char * object,
            char * function_name,
            const _PARAMETER_1_& parameter_1,
            const _PARAMETER_2_& parameter_2,
            const _PARAMETER_3_& parameter_3
            )
        {
            lua_getglobal( ThreadState, object );
            lua_pushstring( ThreadState, function_name );
            lua_gettable( ThreadState, -2 );
            lua_pushvalue( ThreadState, -2 );
            META_SCRIPTABLE_PushValue( parameter_1, ThreadState, ( _PARAMETER_1_* ) 0 );
            META_SCRIPTABLE_PushValue( parameter_2, ThreadState, ( _PARAMETER_2_* ) 0 );
            META_SCRIPTABLE_PushValue( parameter_3, ThreadState, ( _PARAMETER_3_* ) 0 );

            LuaCall( 4 );
            lua_pop( ThreadState, 1 );
        }

        // ~~

        template< class _PARAMETER_1_, class _PARAMETER_2_, class _PARAMETER_3_, class _PARAMETER_4_ >
        void CallFunction(
            char * function_name,
            const _PARAMETER_1_& parameter_1,
            const _PARAMETER_2_& parameter_2,
            const _PARAMETER_3_& parameter_3,
            const _PARAMETER_4_& parameter_4
            )
        {
            lua_getglobal( ThreadState, function_name );
            META_SCRIPTABLE_PushValue( parameter_1, ThreadState, ( _PARAMETER_1_* ) 0 );
            META_SCRIPTABLE_PushValue( parameter_2, ThreadState, ( _PARAMETER_2_* ) 0 );
            META_SCRIPTABLE_PushValue( parameter_3, ThreadState, ( _PARAMETER_3_* ) 0 );
            META_SCRIPTABLE_PushValue( parameter_4, ThreadState, ( _PARAMETER_4_* ) 0 );

            LuaCall( 4 );
        }

        // ~~

        template< class _PARAMETER_1_, class _PARAMETER_2_, class _PARAMETER_3_, class _PARAMETER_4_ >
        void CallObjectFunction(
            char * object,
            char * function_name,
            const _PARAMETER_1_& parameter_1,
            const _PARAMETER_2_& parameter_2,
            const _PARAMETER_3_& parameter_3,
            const _PARAMETER_4_& parameter_4
            )
        {
            lua_getglobal( ThreadState, object );
            lua_pushstring( ThreadState, function_name );
            lua_gettable( ThreadState, -2 );
            lua_pushvalue( ThreadState, -2 );
            META_SCRIPTABLE_PushValue( parameter_1, ThreadState, ( _PARAMETER_1_* ) 0 );
            META_SCRIPTABLE_PushValue( parameter_2, ThreadState, ( _PARAMETER_2_* ) 0 );
            META_SCRIPTABLE_PushValue( parameter_3, ThreadState, ( _PARAMETER_3_* ) 0 );
            META_SCRIPTABLE_PushValue( parameter_4, ThreadState, ( _PARAMETER_4_* ) 0 );

            LuaCall( 5 );
            lua_pop( ThreadState, 1 );
        }

        // ~~

        template< class _PARAMETER_1_, class _PARAMETER_2_, class _PARAMETER_3_, class _PARAMETER_4_, class _PARAMETER_5_ >
        void CallFunction(
            char * function_name,
            const _PARAMETER_1_& parameter_1,
            const _PARAMETER_2_& parameter_2,
            const _PARAMETER_3_& parameter_3,
            const _PARAMETER_4_& parameter_4,
            const _PARAMETER_5_& parameter_5
            )
        {
            lua_getglobal( ThreadState, function_name );
            META_SCRIPTABLE_PushValue( parameter_1, ThreadState, ( _PARAMETER_1_* ) 0 );
            META_SCRIPTABLE_PushValue( parameter_2, ThreadState, ( _PARAMETER_2_* ) 0 );
            META_SCRIPTABLE_PushValue( parameter_3, ThreadState, ( _PARAMETER_3_* ) 0 );
            META_SCRIPTABLE_PushValue( parameter_4, ThreadState, ( _PARAMETER_4_* ) 0 );
            META_SCRIPTABLE_PushValue( parameter_5, ThreadState, ( _PARAMETER_5_* ) 0 );

            LuaCall( 5 );
        }

        // ~~

        template< class _PARAMETER_1_, class _PARAMETER_2_, class _PARAMETER_3_, class _PARAMETER_4_, class _PARAMETER_5_ >
        void CallObjectFunction(
            char * object,
            char * function_name,
            const _PARAMETER_1_& parameter_1,
            const _PARAMETER_2_& parameter_2,
            const _PARAMETER_3_& parameter_3,
            const _PARAMETER_4_& parameter_4,
            const _PARAMETER_5_& parameter_5
            )
        {
            lua_getglobal( ThreadState, object );
            lua_pushstring( ThreadState, function_name );
            lua_gettable( ThreadState, -2 );
            lua_pushvalue( ThreadState, -2 );
            META_SCRIPTABLE_PushValue( parameter_1, ThreadState, ( _PARAMETER_1_* ) 0 );
            META_SCRIPTABLE_PushValue( parameter_2, ThreadState, ( _PARAMETER_2_* ) 0 );
            META_SCRIPTABLE_PushValue( parameter_3, ThreadState, ( _PARAMETER_3_* ) 0 );
            META_SCRIPTABLE_PushValue( parameter_4, ThreadState, ( _PARAMETER_4_* ) 0 );
            META_SCRIPTABLE_PushValue( parameter_5, ThreadState, ( _PARAMETER_5_* ) 0 );

            LuaCall( 6 );
            lua_pop( ThreadState, 1 );
        }

        // ~~

        template< class _RESULT_ >
        void CallResultFunction(
            char * function_name,
            _RESULT_ &result
            )
        {
            lua_getglobal( ThreadState, function_name );
            LuaCall( 0, 1 );
            GetReturnValue( result );
        }

        // ~~

        /*
        template< class _RESULT_ >
        void CallResultFunction(
            char * function_name,
            META_REF_TO_<_RESULT_> &result
            )
        {
            lua_getglobal( ThreadState, function_name );
            LuaCall( 0, 1 );
            GetReturnValue( result );
        }
        */

        // ~~

        template< class _RESULT_, class _PARAMETER_1_ >
        void CallResultFunction(
            char * function_name,
            _RESULT_ &result,
            const _PARAMETER_1_& parameter_1
            )
        {
            lua_getglobal( ThreadState, function_name );
            META_SCRIPTABLE_PushValue( parameter_1, ThreadState, ( _PARAMETER_1_* ) 0 );

            LuaCall( 1, 1 );
            GetReturnValue( result );
        }

        // ~~

        template< class _RESULT_, class _PARAMETER_1_, class _PARAMETER_2_ >
        void CallResultFunction(
            char * function_name,
            _RESULT_ &result,
            const _PARAMETER_1_& parameter_1,
            const _PARAMETER_2_& parameter_2
            )
        {
            lua_getglobal( ThreadState, function_name );
            META_SCRIPTABLE_PushValue( parameter_1, ThreadState, ( _PARAMETER_1_* ) 0 );
            META_SCRIPTABLE_PushValue( parameter_2, ThreadState, ( _PARAMETER_2_* ) 0 );

            LuaCall( 2, 1 );
            GetReturnValue( result );
        }

        // ~~

        template< class _RESULT_, class _PARAMETER_1_, class _PARAMETER_2_, class _PARAMETER_3_ >
        void CallResultFunction(
            char * function_name,
            _RESULT_ &result,
            const _PARAMETER_1_& parameter_1,
            const _PARAMETER_2_& parameter_2,
            const _PARAMETER_3_& parameter_3
            )
        {
            lua_getglobal( ThreadState, function_name );
            META_SCRIPTABLE_PushValue( parameter_1, ThreadState, ( _PARAMETER_1_* ) 0 );
            META_SCRIPTABLE_PushValue( parameter_2, ThreadState, ( _PARAMETER_2_* ) 0 );
            META_SCRIPTABLE_PushValue( parameter_3, ThreadState, ( _PARAMETER_3_* ) 0 );

            LuaCall( 3, 1 );
            GetReturnValue( result );
        }

        // ~~

        template< class _RESULT_, class _PARAMETER_1_, class _PARAMETER_2_, class _PARAMETER_3_ , class _PARAMETER_4_ >
        void CallResultFunction(
            char * function_name,
            _RESULT_ &result,
            const _PARAMETER_1_& parameter_1,
            const _PARAMETER_2_& parameter_2,
            const _PARAMETER_3_& parameter_3,
            const _PARAMETER_4_& parameter_4
            )
        {
            lua_getglobal( ThreadState, function_name );
            META_SCRIPTABLE_PushValue( parameter_1, ThreadState, ( _PARAMETER_1_* ) 0 );
            META_SCRIPTABLE_PushValue( parameter_2, ThreadState, ( _PARAMETER_2_* ) 0 );
            META_SCRIPTABLE_PushValue( parameter_3, ThreadState, ( _PARAMETER_3_* ) 0 );
            META_SCRIPTABLE_PushValue( parameter_4, ThreadState, ( _PARAMETER_4_* ) 0 );

            LuaCall( 4, 1 );
            GetReturnValue( result );
        }

        // ~~

        template< class _RESULT_, class _PARAMETER_1_, class _PARAMETER_2_, class _PARAMETER_3_ , class _PARAMETER_4_ , class _PARAMETER_5_ >
        void CallResultFunction(
            char * function_name,
            _RESULT_ &result,
            const _PARAMETER_1_& parameter_1,
            const _PARAMETER_2_& parameter_2,
            const _PARAMETER_3_& parameter_3,
            const _PARAMETER_4_& parameter_4,
            const _PARAMETER_5_& parameter_5
            )
        {
            lua_getglobal( ThreadState, function_name );
            META_SCRIPTABLE_PushValue( parameter_1, ThreadState, ( _PARAMETER_1_* ) 0 );
            META_SCRIPTABLE_PushValue( parameter_2, ThreadState, ( _PARAMETER_2_* ) 0 );
            META_SCRIPTABLE_PushValue( parameter_3, ThreadState, ( _PARAMETER_3_* ) 0 );
            META_SCRIPTABLE_PushValue( parameter_4, ThreadState, ( _PARAMETER_4_* ) 0 );
            META_SCRIPTABLE_PushValue( parameter_5, ThreadState, ( _PARAMETER_5_* ) 0 );

            LuaCall( 5, 1 );
            GetReturnValue( result );
        }

        // -- PRIVATE

        void CollectGarbage(
            void
            );

    private :

        // .. OPERATIONS

        // ~~

        void LuaCall(
            int parameter_number,
            int result_count = 0
            );

        // .. ATTRIBUTES

        lua_State
            * ThreadState;
        COUNTED_LINK_TO_<SCRIPT_MANAGER>
            Manager;
    };

    typedef COUNTED_REF_TO_<SCRIPT> 
        SCRIPT_REF;
#endif
