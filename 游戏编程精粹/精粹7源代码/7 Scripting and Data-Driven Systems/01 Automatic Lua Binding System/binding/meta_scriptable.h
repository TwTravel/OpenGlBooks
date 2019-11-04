#ifndef __META_SCRIPTABLE_H__
    #define __META_SCRIPTABLE_H__
    // -- LOCAL

    // .. REFERENCES

    #include "meta_scriptable_lua_registerer.h"
    #include "meta_scriptable_type.h"
    #include "meta_scriptable_type_table.h"
    #include "meta_scriptable_binding_data.h"

    // ~~

    #include <lua.h>
    #include <lualib.h>
    #include <lauxlib.h>
	#include <string>

    // -- GLOBAL

    typedef char* META_SCRIPTABLE_CHAR_POINTER;

    // .. FUNCTIONS

    #define META_SCRIPTABLE_DeclareScriptableTypeName( _TYPE_ ) \
        template<> \
        inline const char* META_SCRIPTABLE_GetClassName<_TYPE_>( void )\
        { \
            return #_TYPE_; \
        }

    // ~~

    template< typename _CLASS_>
    inline const char* META_SCRIPTABLE_GetClassName(
        void
        )
    {
        return _CLASS_::META_SCRIPT::GetClassName();
    }

    // ~~

    META_SCRIPTABLE_DeclareScriptableTypeName( int )
    META_SCRIPTABLE_DeclareScriptableTypeName( float )
    META_SCRIPTABLE_DeclareScriptableTypeName( bool )

    // ~~

    #define META_SCRIPTABLE_CommonClass( _CLASS_ ) \
        \
        class META_SCRIPT; \
        friend class META_SCRIPT; \
        class META_SCRIPT : \
            public META_SCRIPTABLE_TYPE \
        { \
        public : \
            typedef _CLASS_ CLASS; \
            META_SCRIPT( void ); \
            static const char * GetClassName() { return #_CLASS_; } \
            static int & GetClassIndex()\
            { \
                static int index = -1;\
                return index;\
            }\
            static int Delete( lua_State * lua_state ); \
            virtual const char * GetName() const { return GetClassName(); } \
            virtual int & GetIndex(){ return GetClassIndex(); } \
            virtual void Register( META_SCRIPTABLE_BINDING_DATA & binding ); \
        };

    // ~~ 

    #define META_SCRIPTABLE_CommonTemplateClass( _CLASS_, _TEMPLATE_ ) \
        \
        class META_SCRIPT; \
        friend class META_SCRIPT; \
        class META_SCRIPT : \
            public META_SCRIPTABLE_TYPE \
        { \
        public : \
            typedef _CLASS_<_TEMPLATE_> CLASS; \
            META_SCRIPT( void ); \
            static const char * GetClassName() \
            { \
                static std::string name = std::string( #_CLASS_ ) + std::string( META_SCRIPTABLE_GetClassName<_TEMPLATE_>() );\
                return name.c_str(); \
            } \
            static int & GetClassIndex()\
            { \
                static int index = -1;\
                return index;\
            }\
            static int Delete( lua_State * lua_state ); \
            virtual const char * GetName() const { return GetClassName(); } \
            virtual int & GetIndex(){ return GetClassIndex(); } \
            virtual void Register( META_SCRIPTABLE_BINDING_DATA & binding ); \
        };

    // ~~

    #define META_SCRIPTABLE_DefineClass( _CLASS_ ) \
        META_SCRIPTABLE_CommonClass( _CLASS_ ) \
        virtual void LuaPushValue( lua_State * lua_state ) const;

    // ~~

    #define META_SCRIPTABLE_DefineRawClass( _CLASS_ ) \
        META_SCRIPTABLE_CommonClass( _CLASS_ ) \
        void LuaPushValue( lua_State * lua_state ) const;

    // ~~

    #define META_SCRIPTABLE_DefineTemplateClass( _CLASS_, _TEMPLATE_ ) \
        META_SCRIPTABLE_CommonTemplateClass( _CLASS_, _TEMPLATE_ ) \
        virtual void LuaPushValue( lua_State * lua_state ) const;

    // ~~

    #define META_SCRIPTABLE_DefineRawTemplateClass( _CLASS_, _TEMPLATE_ ) \
        META_SCRIPTABLE_CommonTemplateClass( _CLASS_, _TEMPLATE_ ) \
        void LuaPushValue( lua_State * lua_state ) const;

    // ~~

    #define META_SCRIPTABLE_ClassPrefix( _CLASS_ ) \
        \
        static _CLASS_::META_SCRIPT META_SCRIPTABLE_Type##_CLASS_; \
        _CLASS_::META_SCRIPT::META_SCRIPT( void ) \
        { \
        }

    // ~~

    #define META_SCRIPTABLE_ClassSuffix( _CLASS_, _PARENT_NAME_ ) \
        \
        void _CLASS_::META_SCRIPT::Register( META_SCRIPTABLE_BINDING_DATA & binding ) \
        { \
            const char * class_name = #_CLASS_; \
            const char * parent_name = _PARENT_NAME_; \
            META_SCRIPTABLE_LUA_REGISTERER::BeginClass( binding, META_SCRIPT::Delete, *this );

    // ~~

    #define META_SCRIPTABLE_UncountedImplementationCommonClass( _CLASS_ ) \
        \
        META_SCRIPTABLE_ClassPrefix( _CLASS_ ) \
        int _CLASS_::META_SCRIPT::Delete( lua_State * lua_state ) \
        { \
            META_SCRIPTABLE_GetSelf(); \
            delete self; \
            return 0; \
        } \
        void _CLASS_::LuaPushValue( lua_State * lua_state ) const \
        { \
            META_SCRIPTABLE_BINDING_HELPER* user_box; \
            _CLASS_ * object_copy; \
            const char* class_name = META_SCRIPT::GetClassName(); \
            META_SCRIPTABLE_BINDING_DATA * binding; \
            object_copy = new _CLASS_( *this ); \
            user_box = ( META_SCRIPTABLE_BINDING_HELPER * ) lua_newuserdata( lua_state, sizeof( META_SCRIPTABLE_BINDING_HELPER ) ); \
            user_box->Object = object_copy; \
             \
            binding = *(META_SCRIPTABLE_BINDING_DATA**)lua_getuserdata( lua_state ); \
            if( META_SCRIPT::GetClassIndex() == -1 ) \
            { \
                luaL_error( lua_state, "No binding found for class %s\n", class_name ); \
            } \
            else \
            { \
                user_box->ClassIndex = META_SCRIPT::GetClassIndex(); \
				META_SCRIPTABLE_LUA_REGISTERER::PushMetatable( lua_state );\
				lua_setmetatable( lua_state, -2 ); \
            }\
        } \

    // ~~

    #define META_SCRIPTABLE_ImplementationCommonClass( _CLASS_ ) \
        \
        META_SCRIPTABLE_ClassPrefix( _CLASS_ ) \
        int _CLASS_::META_SCRIPT::Delete( lua_State * lua_state ) \
        { \
            META_SCRIPTABLE_GetSelf(); \
            self->RemoveRef(); \
            return 0; \
        } \
        void _CLASS_::LuaPushValue( lua_State * lua_state ) const \
        { \
            META_SCRIPTABLE_BINDING_HELPER * user_box; \
            lua_getfield( lua_state, LUA_GLOBALSINDEX, "_objects" ); \
            lua_pushlightuserdata( lua_state, ( void * ) this ); \
            lua_rawget( lua_state, -2 ); \
            if ( lua_isnil( lua_state, -1 ) ) \
            { \
                const char * class_name = _CLASS_::META_SCRIPT::GetClassName(); \
                META_SCRIPTABLE_BINDING_DATA * binding; \
                lua_pop( lua_state, 1 ); \
                const_cast<_CLASS_*>( this )->AddRef(); \
                user_box = ( META_SCRIPTABLE_BINDING_HELPER* ) lua_newuserdata( lua_state, sizeof( META_SCRIPTABLE_BINDING_HELPER ) ); \
                user_box->Object = (void*) this; \
                binding = *(META_SCRIPTABLE_BINDING_DATA**)lua_getuserdata( lua_state ); \
                if( META_SCRIPT::GetClassIndex() == -1 ) \
                { \
                    luaL_error( lua_state, "No binding found for class %s\n", class_name ); \
                } \
                else \
                { \
                    user_box->ClassIndex = META_SCRIPT::GetClassIndex(); \
					META_SCRIPTABLE_LUA_REGISTERER::PushMetatable( lua_state );\
					lua_setmetatable( lua_state, -2 ); \
                }\
                lua_pushlightuserdata( lua_state, (void*)this ); \
                lua_pushvalue( lua_state, -2 ); \
                lua_rawset( lua_state, -4 ); \
            } \
            lua_remove( lua_state, -2 ); \
        }

    // ~~

    #define META_SCRIPTABLE_Class( _CLASS_ ) \
        \
        META_SCRIPTABLE_ImplementationCommonClass( _CLASS_ )\
        META_SCRIPTABLE_ClassSuffix( _CLASS_, NULL )

    // ~~

    #define META_SCRIPTABLE_UncountedClass( _CLASS_ ) \
        \
        META_SCRIPTABLE_UncountedImplementationCommonClass( _CLASS_ )\
        META_SCRIPTABLE_ClassSuffix( _CLASS_, 0 )

    // ~~

    #define META_SCRIPTABLE_InheritedClass( _CLASS_, _PARENT_ ) \
        \
        META_SCRIPTABLE_ImplementationCommonClass( _CLASS_ )\
        META_SCRIPTABLE_ClassSuffix( _CLASS_, _PARENT_::META_SCRIPT::GetClassName() )

    // ~~

    #define META_SCRIPTABLE_UncountedInheritedClass( _CLASS_, _PARENT_ ) \
        \
        META_SCRIPTABLE_UncountedImplementationCommonClass( _CLASS_ )\
        META_SCRIPTABLE_ClassSuffix( _CLASS_, _PARENT_::META_SCRIPT::GetClassName() )

    // ~~ Template implementation

    #define META_SCRIPTABLE_TemplateClassSuffix( _CLASS_, _PARENT_NAME_ ) \
        \
        template<typename _TEMPLATE_> inline _CLASS_<_TEMPLATE_>::META_SCRIPT::META_SCRIPT( void ) \
        { \
        } \
        \
        template<typename _TEMPLATE_> inline void _CLASS_<_TEMPLATE_>::META_SCRIPT::Register( META_SCRIPTABLE_BINDING_DATA & binding ) \
        { \
            const char * class_name = META_SCRIPT::GetClassName(); \
            const char * parent_name = _PARENT_NAME_; \
            META_SCRIPTABLE_LUA_REGISTERER::BeginClass( binding, META_SCRIPT::Delete, *this );

    // ~~

    #define META_SCRIPTABLE_UncountedImplementationCommonTemplateClass( _CLASS_ ) \
        \
        template<typename _TEMPLATE_> inline int _CLASS_<_TEMPLATE_>::META_SCRIPT::Delete( lua_State * lua_state ) \
        { \
            META_SCRIPTABLE_GetSelf(); \
            delete self; \
            return 0; \
        } \
        template<typename _TEMPLATE_> inline void _CLASS_<_TEMPLATE_>::LuaPushValue( lua_State * lua_state ) const \
        { \
            META_SCRIPTABLE_BINDING_HELPER* user_box; \
            _CLASS_<_TEMPLATE_> * object_copy; \
            const char* class_name = META_SCRIPT::GetClassName(); \
            META_SCRIPTABLE_BINDING_DATA * binding; \
            \
            object_copy = new _CLASS_<_TEMPLATE_>( *this ); \
            \
            user_box = ( META_SCRIPTABLE_BINDING_HELPER* ) lua_newuserdata( lua_state, sizeof( META_SCRIPTABLE_BINDING_HELPER ) ); \
            user_box->Object = object_copy; \
            \
            binding = *(META_SCRIPTABLE_BINDING_DATA**)lua_getuserdata( lua_state ); \
            if( META_SCRIPT::GetClassIndex() == -1 ) \
            { \
                luaL_error( lua_state, "No binding found for class %s\n", class_name ); \
            } \
            else \
            { \
                user_box->ClassIndex = META_SCRIPT::GetClassIndex(); \
                META_SCRIPTABLE_LUA_REGISTERER::PushMetatable( lua_state );\
                lua_setmetatable( lua_state, -2 ); \
            }\
            \
        } 

    // ~~

    #define META_SCRIPTABLE_ImplementationCommonTemplateClass( _CLASS_ ) \
        \
        template<typename _TEMPLATE_> inline int _CLASS_<_TEMPLATE_>::META_SCRIPT::Delete( lua_State * lua_state ) \
        { \
            META_SCRIPTABLE_GetSelf(); \
            self->RemoveRef(); \
            return 0; \
        } \
        template<typename _TEMPLATE_> inline void _CLASS_<_TEMPLATE_>::LuaPushValue( lua_State * lua_state ) const \
        { \
            META_SCRIPTABLE_BINDING_HELPER* user_box; \
            lua_getfield( lua_state, LUA_GLOBALSINDEX, "_objects" ); \
            lua_pushlightuserdata( lua_state, ( void * ) this ); \
            lua_rawget( lua_state, -2 ); \
            if ( lua_isnil( lua_state, -1 ) ) \
            { \
                const char * class_name = _CLASS_::META_SCRIPT::GetClassName(); \
                META_SCRIPTABLE_BINDING_DATA * binding; \
                lua_pop( lua_state, 1 ); \
                const_cast<_CLASS_<_TEMPLATE_>* >( this )->AddRef(); \
                user_box = ( META_SCRIPTABLE_BINDING_HELPER* ) lua_newuserdata( lua_state, sizeof( META_SCRIPTABLE_BINDING_HELPER ) ); \
                user_box->Object = (void *)this; \
                binding = *(META_SCRIPTABLE_BINDING_DATA**)lua_getuserdata( lua_state ); \
                if( META_SCRIPT::GetClassIndex() == -1 ) \
                { \
                    luaL_error( lua_state, "No binding found for class %s\n", class_name ); \
                } \
                else \
                { \
                    user_box->ClassIndex = META_SCRIPT::GetClassIndex(); \
					META_SCRIPTABLE_LUA_REGISTERER::PushMetatable( lua_state );\
					lua_setmetatable( lua_state, -2 ); \
                }\
                lua_pushlightuserdata( lua_state, ( void* ) this ); \
                lua_pushvalue( lua_state, -2 ); \
                lua_rawset( lua_state, -4 ); \
            } \
            lua_remove( lua_state, -2 ); \
        }

    // ~~

    #define META_SCRIPTABLE_TemplateClass( _CLASS_ ) \
        \
        META_SCRIPTABLE_ImplementationCommonTemplateClass( _CLASS_ )\
        META_SCRIPTABLE_TemplateClassSuffix( _CLASS_, 0 )

        // ~~

    #define META_SCRIPTABLE_UncountedTemplateClass( _CLASS_ ) \
        \
        META_SCRIPTABLE_UncountedImplementationCommonTemplateClass( _CLASS_ )\
        META_SCRIPTABLE_TemplateClassSuffix( _CLASS_, 0 )

        // ~~

    #define META_SCRIPTABLE_InheritedTemplateClass( _CLASS_, _PARENT_ ) \
        \
        META_SCRIPTABLE_ImplementationCommonTemplateClass( _CLASS_ )\
        META_SCRIPTABLE_TemplateClassSuffix( _CLASS_, _PARENT_::META_SCRIPT::GetClassName() )

        // ~~

    #define META_SCRIPTABLE_UncountedInheritedTemplateClass( _CLASS_, _PARENT_ ) \
        \
        META_SCRIPTABLE_UncountedImplementationCommonTemplateClass( _CLASS_ )\
        META_SCRIPTABLE_TemplateClassSuffix( _CLASS_, _PARENT_::META_SCRIPT::GetClassName() )


    // ~~

    #define META_SCRIPTABLE_InstantiateTemplateClass( _CLASS_, _TEMPLATE_ ) \
        static _CLASS_<_TEMPLATE_>::META_SCRIPT instantiate_META_SCRIPT##_CLASS_##_TEMPLATE_;
    // ~~

    #define META_SCRIPTABLE_End() \
        \
            META_SCRIPTABLE_LUA_REGISTERER::EndClass( binding, class_name, parent_name ); \
        };

    // ~~

    template <class _VALUE_>
    inline void META_SCRIPTABLE_PushValue(
        const _VALUE_ & value,
        lua_State * lua_state,
        const _VALUE_ * dummy
        );

    // ~~

    inline void META_SCRIPTABLE_PushValue(
        const bool & value,
        lua_State * lua_state,
        const bool * dummy
        )
    {
        lua_pushboolean( lua_state, ( bool )value );
    }

    // ~~

    inline void META_SCRIPTABLE_PushValue(
        const char & value,
        lua_State * lua_state,
        const char * dummy
        )
    {
        lua_pushnumber( lua_state, ( lua_Number )value );
    }

    // ~~

    inline void META_SCRIPTABLE_PushValue(
        const int & value,
        lua_State * lua_state,
        const int * dummy
        )
    {
        lua_pushnumber( lua_state, ( lua_Number )value );
    }

    // ~~

    inline void META_SCRIPTABLE_PushValue(
        const float & value,
        lua_State * lua_state,
        const float * dummy
        )
    {
        lua_pushnumber( lua_state, ( lua_Number )value );
    }

    // ~~

    inline void META_SCRIPTABLE_PushValue(
        const char * & value,
        lua_State * lua_state,
        const char * * dummy
        )
    {
        lua_pushstring( lua_state, value );
    }

    // ~~

    inline void META_SCRIPTABLE_PushValue(
        const META_SCRIPTABLE_CHAR_POINTER & value,
        lua_State * lua_state,
        const META_SCRIPTABLE_CHAR_POINTER * dummy
        )
    {
        lua_pushstring( lua_state, value );
    }

    // ~~

    inline void META_SCRIPTABLE_PushValue(
        const std::string & value,
        lua_State * lua_state,
        const std::string * dummy
        )
    {
        lua_pushstring( lua_state, value.c_str() );
    }

    // ~~

    template <class _VALUE_>
    inline void META_SCRIPTABLE_PushValue(
        const _VALUE_ & value,
        lua_State * lua_state,
        const _VALUE_ * dummy
        )
    {
        value.LuaPushValue( lua_state );
    }

    // ~~

    template <class _VALUE_>
    inline _VALUE_ & META_SCRIPTABLE_GetValue(
        lua_State * lua_state,
        int value_index,
        const _VALUE_ * dummy
        );

    // ~~

    inline bool META_SCRIPTABLE_GetValue(
        lua_State * lua_state,
        int value_index,
        const bool * dummy
        )
    {
        #ifdef _LUA_DEBUG_
            if( !lua_isboolean( lua_state, value_index ) )
            {
                luaL_typerror( lua_state, value_index, "boolean" );
            }
        #endif

        return !!lua_toboolean( lua_state, value_index );
    }

    // ~~

    inline char META_SCRIPTABLE_GetValue(
        lua_State * lua_state,
        int value_index,
        const char * dummy
        )
    {

        #ifdef _LUA_DEBUG_
            if( !lua_isnumber( lua_state, value_index ) )
            {
                luaL_typerror( lua_state, value_index, "string" );
            }
        #endif

        return char( lua_tonumber( lua_state, value_index ) );
    }

    // ~~

    inline int META_SCRIPTABLE_GetValue(
        lua_State * lua_state,
        int value_index,
        const int * dummy
        )
    {
        #ifdef _LUA_DEBUG_
            if( !lua_isnumber( lua_state, value_index ) )
            {
                luaL_typerror( lua_state, value_index, "number" );
            }
        #endif

        return int( lua_tonumber( lua_state, value_index ) );
    }

    // ~~

    inline float META_SCRIPTABLE_GetValue(
        lua_State * lua_state,
        int value_index,
        const float * dummy
        )
    {
        #ifdef _LUA_DEBUG_
            if( !lua_isnumber( lua_state, value_index ) )
            {
                luaL_typerror( lua_state, value_index, "number" );
            }
        #endif

        return float( lua_tonumber( lua_state, value_index ) );
    }

    // ~~

    inline const META_SCRIPTABLE_CHAR_POINTER META_SCRIPTABLE_GetValue(
        lua_State * lua_state,
        int value_index,
        const META_SCRIPTABLE_CHAR_POINTER * dummy
        )
    {
        META_SCRIPTABLE_CHAR_POINTER
            value;

        #ifdef _LUA_DEBUG_
            if( !lua_isstring( lua_state, value_index ) )
            {
                luaL_typerror( lua_state, value_index, "string" );
            }
        #endif

        value = ( META_SCRIPTABLE_CHAR_POINTER )lua_tostring( lua_state, value_index );

        return value;
    }

    // ~~

    inline std::string META_SCRIPTABLE_GetValue(
        lua_State * lua_state,
        int value_index,
        const std::string * dummy
        )
    {
        #ifdef _LUA_DEBUG_
            if( !lua_isstring( lua_state, value_index ) )
            {
                luaL_typerror( lua_state, value_index, "string" );
            }
        #endif

        return lua_tostring( lua_state, value_index );
    }

    // ~~

    template <class _VALUE_>
    inline _VALUE_ & META_SCRIPTABLE_GetValue(
        lua_State * lua_state,
        int value_index,
        const _VALUE_ * dummy
        )
    {
		META_SCRIPTABLE_BINDING_HELPER
			* binding_helper = ( META_SCRIPTABLE_BINDING_HELPER* )lua_touserdata( lua_state, value_index );

        #ifdef _LUA_DEBUG_
			
			META_SCRIPTABLE_BINDING_DATA
				* binding_data = *( META_SCRIPTABLE_BINDING_DATA **) lua_getuserdata( lua_state );

			if( !binding_data->CheckType( binding_helper->ClassIndex, _VALUE_::META_SCRIPT::GetClassIndex() ) )
				luaL_typerror( lua_state, value_index, _VALUE_::META_SCRIPT::GetClassName() );

        #endif
            
        return *( _VALUE_* ) binding_helper->Object;
    }

    // ~~

    #ifdef _LUA_DEBUG_

        inline bool META_SCRIPTABLE_CheckArgumentCount(
            lua_State* lua_state,
            const int expected_argument
            )
        {
            if( lua_gettop( lua_state ) != expected_argument )
            {
                luaL_error( 
                    lua_state, 
                    "Wrong argument count; got %d expected %d\n", 
                    lua_gettop( lua_state ),
                    expected_argument
                    );
                return false;
            }

            return true;
        }
    #else
        #define META_SCRIPTABLE_CheckArgumentCount( lua_state, expected_argument )
    #endif

    // ~~

    #define META_SCRIPTABLE_ConvertValue( _VALUE_, _CONVERTED_VALUE_, _GetConvertedValue_, _SetFromConvertedValue_ ) \
        \
        inline void META_SCRIPTABLE_PushValue( const _VALUE_ & value, lua_State * lua_state, const _VALUE_ * dummy ) \
        { \
            _CONVERTED_VALUE_ converted_value = value _GetConvertedValue_; \
            META_SCRIPTABLE_PushValue( converted_value, lua_state, &converted_value ); \
        } \
        inline const _VALUE_ META_SCRIPTABLE_GetValue( lua_State * lua_state, int value_index, const _VALUE_ * dummy ) \
        { \
            _CONVERTED_VALUE_ converted_value; \
            _VALUE_ value; \
            converted_value = META_SCRIPTABLE_GetValue( lua_state, value_index, &converted_value ); \
            value _SetFromConvertedValue_( converted_value ); \
            return value; \
        }

    // ~~

    #define META_SCRIPTABLE_CastValue( _VALUE_, _CONVERTED_VALUE_ ) \
        \
        inline void META_SCRIPTABLE_PushValue( const _VALUE_ & value, lua_State * lua_state, const _VALUE_ * dummy ) \
        { \
            _CONVERTED_VALUE_ converted_value = _CONVERTED_VALUE_( value ); \
            META_SCRIPTABLE_PushValue( converted_value, lua_state, &converted_value ); \
        } \
        inline const _VALUE_ META_SCRIPTABLE_GetValue( lua_State * lua_state, INDEX value_index, const _VALUE_ * dummy ) \
        { \
            _CONVERTED_VALUE_ converted_value; \
            converted_value = META_SCRIPTABLE_GetValue( lua_state, value_index, &converted_value ); \
            return _VALUE_( converted_value ); \
        }

    // ~~

    #define META_SCRIPTABLE_GetSelf() \
        \
        CLASS * self = const_cast<CLASS*>( &META_SCRIPTABLE_GetValue( lua_state, 1, ( CLASS * ) 0 ) );

    // ~~

    #define META_SCRIPTABLE_Attribute( _Attribute_ ) \
        \
        class ATTRIBUTE_##_Attribute_ \
        { \
        public : \
            static int Get( lua_State * lua_state ) \
            { \
                META_SCRIPTABLE_CheckArgumentCount( lua_state, 1 ); \
                META_SCRIPTABLE_GetSelf(); \
                META_SCRIPTABLE_PushValue( self->_Attribute_, lua_state, &self->_Attribute_ ); \
                return 1; \
            } \
            static int Set( lua_State * lua_state ) \
            { \
                META_SCRIPTABLE_CheckArgumentCount( lua_state, 2 ); \
                META_SCRIPTABLE_GetSelf(); \
                self->_Attribute_ = META_SCRIPTABLE_GetValue( lua_state, 2, &self->_Attribute_ ); \
                return 0; \
            } \
        }; \
        META_SCRIPTABLE_LUA_REGISTERER::RegisterFunction( binding, ATTRIBUTE_##_Attribute_::Get, "Get"#_Attribute_ ); \
        META_SCRIPTABLE_LUA_REGISTERER::RegisterFunction( binding, ATTRIBUTE_##_Attribute_::Set, "Set"#_Attribute_ );

    // ~~

    // ~~

    #define META_SCRIPTABLE_SingletonAttribute( _Attribute_ ) \
        \
        class SINGLETON_ATTRIBUTE_##_Attribute_ \
        { \
        public : \
            static int Get( lua_State * lua_state ) \
            { \
                META_SCRIPTABLE_CheckArgumentCount( lua_state, 1 ); \
                META_SCRIPTABLE_PushValue( CLASS::GetInstance()._Attribute_, lua_state, &CLASS::GetInstance()._Attribute_ ); \
                return 1; \
            } \
            static int Set( lua_State * lua_state ) \
            { \
                META_SCRIPTABLE_CheckArgumentCount( lua_state, 2 ); \
                CLASS::GetInstance()._Attribute_ = META_SCRIPTABLE_GetValue( lua_state, 2, &CLASS::GetInstance()._Attribute_ ); \
                return 0; \
            } \
        }; \
        META_SCRIPTABLE_LUA_REGISTERER::RegisterFunction( binding, SINGLETON_ATTRIBUTE_##_Attribute_::Get, "Get"#_Attribute_ ); \
        META_SCRIPTABLE_LUA_REGISTERER::RegisterFunction( binding, SINGLETON_ATTRIBUTE_##_Attribute_::Set, "Set"#_Attribute_ );

    // ~~

    #define META_SCRIPTABLE_RefAttribute( _Attribute_ ) \
        \
        class ATTRIBUTE_##_Attribute_ \
        { \
        public : \
            static int Get( lua_State * lua_state ) \
            { \
                META_SCRIPTABLE_CheckArgumentCount( lua_state, 1 ); \
                META_SCRIPTABLE_GetSelf(); \
                META_SCRIPTABLE_PushValue( *self->_Attribute_, lua_state, self->_Attribute_.GetPointer() ); \
                return 1; \
            } \
            static int Set( lua_State * lua_state ) \
            { \
                META_SCRIPTABLE_CheckArgumentCount( lua_state, 2 ); \
                META_SCRIPTABLE_GetSelf(); \
                self->_Attribute_ = & META_SCRIPTABLE_GetValue( lua_state, 2, self->_Attribute_.GetPointer() ); \
                return 0; \
            } \
        }; \
        META_SCRIPTABLE_LUA_REGISTERER::RegisterFunction( binding, ATTRIBUTE_##_Attribute_::Get, "Get"#_Attribute_ ); \
        META_SCRIPTABLE_LUA_REGISTERER::RegisterFunction( binding, ATTRIBUTE_##_Attribute_::Set, "Set"#_Attribute_ );

    // ~~

    #define META_SCRIPTABLE_Variable( _Variable_ ) \
        \
        class VARIABLE_##_Variable_ \
        { \
        public : \
            static int Get( lua_State * lua_state ) \
            { \
                META_SCRIPTABLE_CheckArgumentCount( lua_state, 1 ); \
                META_SCRIPTABLE_PushValue( CLASS::_Variable_, lua_state, &CLASS::_Variable_ ); \
                return 1; \
            } \
            static int Set( lua_State * lua_state ) \
            { \
                META_SCRIPTABLE_CheckArgumentCount( lua_state, 2 ); \
                CLASS::_Variable_ = META_SCRIPTABLE_GetValue( lua_state, 2, &CLASS::_Variable_ ); \
                return 0; \
            } \
        }; \
        META_SCRIPTABLE_LUA_REGISTERER::RegisterFunction( binding, VARIABLE_##_Variable_::Get, "Get"#_Variable_ ); \
        META_SCRIPTABLE_LUA_REGISTERER::RegisterFunction( binding, VARIABLE_##_Variable_::Set, "Set"#_Variable_ );

    // ~~

    #define META_SCRIPTABLE_ConstVariable( _Variable_ ) \
        \
        class VARIABLE_##_Variable_ \
        { \
        public : \
            static int Get( lua_State * lua_state ) \
            { \
                META_SCRIPTABLE_CheckArgumentCount( lua_state, 1 ); \
                META_SCRIPTABLE_PushValue( CLASS::_Variable_, lua_state, &CLASS::_Variable_ ); \
                return 1; \
            } \
        }; \
        META_SCRIPTABLE_LUA_REGISTERER::RegisterFunction( binding, VARIABLE_##_Variable_::Get, "Get"#_Variable_ );

    // ~~

    #define META_SCRIPTABLE_Constructor() \
        META_SCRIPTABLE_RenamedConstructor( new )

    // ~~

    #define META_SCRIPTABLE_RenamedConstructor( _Name_ ) \
        \
        class CONSTRUCTOR_##_Name_ \
        { \
        public : \
            static int Call( lua_State * lua_state ) \
            { \
                META_SCRIPTABLE_CheckArgumentCount( lua_state, 1 ); \
                CLASS * self = new CLASS; \
                META_SCRIPTABLE_PushValue( *self, lua_state, ( CLASS * ) 0 ); \
                return 1; \
            } \
        }; \
        META_SCRIPTABLE_LUA_REGISTERER::RegisterFunction( binding, CONSTRUCTOR_##_Name_::Call, #_Name_ );

    // ~~

    #define META_SCRIPTABLE_Constructor1(  _TEMPLATE_1_ ) \
        META_SCRIPTABLE_RenamedConstructor1( new, _TEMPLATE_1_ )

    // ~~

    #define META_SCRIPTABLE_RenamedConstructor1( _Name_, _TEMPLATE_1_ ) \
        \
        class CONSTRUCTOR_1_##_Name_ \
        { \
        public : \
            static int Call( lua_State * lua_state ) \
            { \
                META_SCRIPTABLE_CheckArgumentCount( lua_state, 2 ); \
                CLASS * self = new CLASS( \
                    META_SCRIPTABLE_GetValue( lua_state, 2, ( _TEMPLATE_1_ * )0 ) \
                    ); \
                META_SCRIPTABLE_PushValue( *self, lua_state, ( CLASS * ) 0 ); \
                return 1; \
            } \
        }; \
        META_SCRIPTABLE_LUA_REGISTERER::RegisterFunction( binding, CONSTRUCTOR_1_##_Name_::Call, #_Name_ );

    // ~~

    #define META_SCRIPTABLE_Constructor2( _TEMPLATE_1_, _TEMPLATE_2_ ) \
        META_SCRIPTABLE_RenamedConstructor2( new, _TEMPLATE_1_, _TEMPLATE_2_ )

    // ~~

    #define META_SCRIPTABLE_RenamedConstructor2( _Name_, _TEMPLATE_1_, _TEMPLATE_2_ ) \
        \
    class CONSTRUCTOR_2_##_Name_ \
        { \
        public : \
            static int Call( lua_State * lua_state ) \
            { \
                META_SCRIPTABLE_CheckArgumentCount( lua_state, 3 ); \
                CLASS * self = new CLASS( \
                    META_SCRIPTABLE_GetValue( lua_state, 2, ( _TEMPLATE_1_ * )0 ), \
                    META_SCRIPTABLE_GetValue( lua_state, 3, ( _TEMPLATE_2_ * )0 ) \
                    ); \
                META_SCRIPTABLE_PushValue( *self, lua_state, ( CLASS * ) 0 ); \
                return 1; \
            } \
        }; \
        META_SCRIPTABLE_LUA_REGISTERER::RegisterFunction( binding, CONSTRUCTOR_2_##_Name_::Call, #_Name_ );

    // ~~

    #define META_SCRIPTABLE_Constructor3( _TEMPLATE_1_, _TEMPLATE_2_, _TEMPLATE_3_ ) \
        META_SCRIPTABLE_RenamedConstructor3( new, _TEMPLATE_1_, _TEMPLATE_2_, _TEMPLATE_3_ )

    // ~~

    #define META_SCRIPTABLE_RenamedConstructor3( _Name_, _TEMPLATE_1_, _TEMPLATE_2_, _TEMPLATE_3_ ) \
        \
        class CONSTRUCTOR_3_##_Name_ \
        { \
        public : \
            static int Call( lua_State * lua_state ) \
            { \
                META_SCRIPTABLE_CheckArgumentCount( lua_state, 4 ); \
                CLASS * self = new CLASS( \
                    META_SCRIPTABLE_GetValue( lua_state, 2, ( _TEMPLATE_1_ * )0 ), \
                    META_SCRIPTABLE_GetValue( lua_state, 3, ( _TEMPLATE_2_ * )0 ), \
                    META_SCRIPTABLE_GetValue( lua_state, 4, ( _TEMPLATE_3_ * )0 ) \
                    ); \
                META_SCRIPTABLE_PushValue( *self, lua_state, ( CLASS * ) 0 ); \
                return 1; \
            } \
        }; \
        META_SCRIPTABLE_LUA_REGISTERER::RegisterFunction( binding, CONSTRUCTOR_3_##_Name_::Call, #_Name_ );

    // ~~

    #define META_SCRIPTABLE_Constructor4( _TEMPLATE_1_, _TEMPLATE_2_, _TEMPLATE_3_, _TEMPLATE_4_ ) \
        META_SCRIPTABLE_RenamedConstructor4( new, _TEMPLATE_1_, _TEMPLATE_2_, _TEMPLATE_3_, _TEMPLATE_4_ )

    // ~~

    #define META_SCRIPTABLE_RenamedConstructor4( _Name_, _TEMPLATE_1_, _TEMPLATE_2_, _TEMPLATE_3_, _TEMPLATE_4_ ) \
        \
    class CONSTRUCTOR_4_##Name_ \
        { \
        public : \
            static int Call( lua_State * lua_state ) \
            { \
                META_SCRIPTABLE_CheckArgumentCount( lua_state, 5 ); \
                CLASS * self = new CLASS( \
                    META_SCRIPTABLE_GetValue( lua_state, 2, ( _TEMPLATE_1_ * )0 ), \
                    META_SCRIPTABLE_GetValue( lua_state, 3, ( _TEMPLATE_2_ * )0 ), \
                    META_SCRIPTABLE_GetValue( lua_state, 4, ( _TEMPLATE_3_ * )0 ), \
                    META_SCRIPTABLE_GetValue( lua_state, 5, ( _TEMPLATE_4_ * )0 ) \
                    ); \
                META_SCRIPTABLE_PushValue( *self, lua_state, ( CLASS * ) 0 ); \
                return 1; \
            } \
        }; \
        META_SCRIPTABLE_LUA_REGISTERER::RegisterFunction( binding, CONSTRUCTOR_4_##Name_::Call, #_Name_ );

    // ~~

    #define META_SCRIPTABLE_Constructor5( _TEMPLATE_1_, _TEMPLATE_2_, _TEMPLATE_3_, _TEMPLATE_4_, _TEMPLATE_5_ ) \
        META_SCRIPTABLE_RenamedConstructor5( new, _TEMPLATE_1_, _TEMPLATE_2_, _TEMPLATE_3_, _TEMPLATE_4_, _TEMPLATE_5_ )

    // ~~

    #define META_SCRIPTABLE_RenamedConstructor5( _Name_, _TEMPLATE_1_, _TEMPLATE_2_, _TEMPLATE_3_, _TEMPLATE_4_, _TEMPLATE_5_  ) \
        \
    class CONSTRUCTOR_5_##_Name_ \
        { \
        public : \
        static int Call( lua_State * lua_state ) \
            { \
            META_SCRIPTABLE_CheckArgumentCount( lua_state, 6 ); \
            CLASS * self = new CLASS( \
            META_SCRIPTABLE_GetValue( lua_state, 2, ( _TEMPLATE_1_ * )0 ), \
            META_SCRIPTABLE_GetValue( lua_state, 3, ( _TEMPLATE_2_ * )0 ), \
            META_SCRIPTABLE_GetValue( lua_state, 4, ( _TEMPLATE_3_ * )0 ), \
            META_SCRIPTABLE_GetValue( lua_state, 5, ( _TEMPLATE_4_ * )0 ), \
            META_SCRIPTABLE_GetValue( lua_state, 6, ( _TEMPLATE_5_ * )0 ) \
            ); \
            META_SCRIPTABLE_PushValue( *self, lua_state, ( CLASS * ) 0 ); \
            return 1; \
            } \
        }; \
        META_SCRIPTABLE_LUA_REGISTERER::RegisterFunction( binding, CONSTRUCTOR_5_##_Name_::Call, #_Name_ );

    // ~~

    #define META_SCRIPTABLE_Constructor6( _TEMPLATE_1_, _TEMPLATE_2_, _TEMPLATE_3_, _TEMPLATE_4_, _TEMPLATE_5_, _TEMPLATE_6_ ) \
        META_SCRIPTABLE_RenamedConstructor6( new, _TEMPLATE_1_, _TEMPLATE_2_, _TEMPLATE_3_, _TEMPLATE_4_, _TEMPLATE_5_, _TEMPLATE_6_ )

    // ~~

    #define META_SCRIPTABLE_RenamedConstructor6( _Name_, _TEMPLATE_1_, _TEMPLATE_2_, _TEMPLATE_3_, _TEMPLATE_4_, _TEMPLATE_5_, _TEMPLATE_6_  ) \
        \
    class CONSTRUCTOR_6_##_Name_ \
        { \
        public : \
        static int Call( lua_State * lua_state ) \
            { \
            META_SCRIPTABLE_CheckArgumentCount( lua_state, 7 ); \
            CLASS * self = new CLASS( \
            META_SCRIPTABLE_GetValue( lua_state, 2, ( _TEMPLATE_1_ * )0 ), \
            META_SCRIPTABLE_GetValue( lua_state, 3, ( _TEMPLATE_2_ * )0 ), \
            META_SCRIPTABLE_GetValue( lua_state, 4, ( _TEMPLATE_3_ * )0 ), \
            META_SCRIPTABLE_GetValue( lua_state, 5, ( _TEMPLATE_4_ * )0 ), \
            META_SCRIPTABLE_GetValue( lua_state, 6, ( _TEMPLATE_5_ * )0 ), \
            META_SCRIPTABLE_GetValue( lua_state, 7, ( _TEMPLATE_6_ * )0 ) \
            ); \
            META_SCRIPTABLE_PushValue( *self, lua_state, ( CLASS * ) 0 ); \
            return 1; \
            } \
        }; \
        META_SCRIPTABLE_LUA_REGISTERER::RegisterFunction( binding, CONSTRUCTOR_6_##_Name_::Call, #_Name_ );

    // ~~

    #define META_SCRIPTABLE_Constructor7( _TEMPLATE_1_, _TEMPLATE_2_, _TEMPLATE_3_, _TEMPLATE_4_, _TEMPLATE_5_, _TEMPLATE_6_, _TEMPLATE_7_ ) \
        META_SCRIPTABLE_RenamedConstructor7( new, _TEMPLATE_1_, _TEMPLATE_2_, _TEMPLATE_3_, _TEMPLATE_4_, _TEMPLATE_5_, _TEMPLATE_6_, _TEMPLATE_7_ )

    // ~~

    #define META_SCRIPTABLE_RenamedConstructor7( _Name_, _TEMPLATE_1_, _TEMPLATE_2_, _TEMPLATE_3_, _TEMPLATE_4_, _TEMPLATE_5_, _TEMPLATE_6_, _TEMPLATE_7_  ) \
        \
    class CONSTRUCTOR_7_##_Name_ \
        { \
        public : \
        static int Call( lua_State * lua_state ) \
            { \
            META_SCRIPTABLE_CheckArgumentCount( lua_state, 8 ); \
            CLASS * self = new CLASS( \
            META_SCRIPTABLE_GetValue( lua_state, 2, ( _TEMPLATE_1_ * )0 ), \
            META_SCRIPTABLE_GetValue( lua_state, 3, ( _TEMPLATE_2_ * )0 ), \
            META_SCRIPTABLE_GetValue( lua_state, 4, ( _TEMPLATE_3_ * )0 ), \
            META_SCRIPTABLE_GetValue( lua_state, 5, ( _TEMPLATE_4_ * )0 ), \
            META_SCRIPTABLE_GetValue( lua_state, 6, ( _TEMPLATE_5_ * )0 ), \
            META_SCRIPTABLE_GetValue( lua_state, 7, ( _TEMPLATE_6_ * )0 ), \
            META_SCRIPTABLE_GetValue( lua_state, 8, ( _TEMPLATE_7_ * )0 ) \
            ); \
            META_SCRIPTABLE_PushValue( *self, lua_state, ( CLASS * ) 0 ); \
            return 1; \
            } \
        }; \
        META_SCRIPTABLE_LUA_REGISTERER::RegisterFunction( binding, CONSTRUCTOR_7_##_Name_::Call, #_Name_ );

    // ~~

    #define META_SCRIPTABLE_Constructor8( _TEMPLATE_1_, _TEMPLATE_2_, _TEMPLATE_3_, _TEMPLATE_4_, _TEMPLATE_5_, _TEMPLATE_6_, _TEMPLATE_7_, _TEMPLATE_8_ ) \
        META_SCRIPTABLE_RenamedConstructor8( new, _TEMPLATE_1_, _TEMPLATE_2_, _TEMPLATE_3_, _TEMPLATE_4_, _TEMPLATE_5_, _TEMPLATE_6_, _TEMPLATE_7_, _TEMPLATE_8_ )

    // ~~

    #define META_SCRIPTABLE_RenamedConstructor8( _Name_, _TEMPLATE_1_, _TEMPLATE_2_, _TEMPLATE_3_, _TEMPLATE_4_, _TEMPLATE_5_, _TEMPLATE_6_, _TEMPLATE_7_, _TEMPLATE_8_  ) \
        \
    class CONSTRUCTOR_8_##_Name_ \
        { \
        public : \
        static int Call( lua_State * lua_state ) \
            { \
            META_SCRIPTABLE_CheckArgumentCount( lua_state, 9 ); \
            CLASS * self = new CLASS( \
            META_SCRIPTABLE_GetValue( lua_state, 2, ( _TEMPLATE_1_ * )0 ), \
            META_SCRIPTABLE_GetValue( lua_state, 3, ( _TEMPLATE_2_ * )0 ), \
            META_SCRIPTABLE_GetValue( lua_state, 4, ( _TEMPLATE_3_ * )0 ), \
            META_SCRIPTABLE_GetValue( lua_state, 5, ( _TEMPLATE_4_ * )0 ), \
            META_SCRIPTABLE_GetValue( lua_state, 6, ( _TEMPLATE_5_ * )0 ), \
            META_SCRIPTABLE_GetValue( lua_state, 7, ( _TEMPLATE_6_ * )0 ), \
            META_SCRIPTABLE_GetValue( lua_state, 8, ( _TEMPLATE_7_ * )0 ), \
            META_SCRIPTABLE_GetValue( lua_state, 9, ( _TEMPLATE_8_ * )0 ) \
            ); \
            META_SCRIPTABLE_PushValue( *self, lua_state, ( CLASS * ) 0 ); \
            return 1; \
            } \
        }; \
        META_SCRIPTABLE_LUA_REGISTERER::RegisterFunction( binding, CONSTRUCTOR_8_##_Name_::Call, #_Name_ );

    // ~~

    #define META_SCRIPTABLE_EqualOperator( _TEMPLATE_1_, _TEMPLATE_2_ ) \
        \
        class EQUAL_OPERATOR \
        { \
        public : \
            static int Call( lua_State * lua_state ) \
            { \
                META_SCRIPTABLE_CheckArgumentCount( lua_state, 2 ); \
                META_SCRIPTABLE_GetSelf(); \
                META_SCRIPTABLE_PushValue( \
                    *( self ) == META_SCRIPTABLE_GetValue( lua_state, 2, ( _TEMPLATE_2_ * )0 ) , \
                    lua_state, \
                    ( BOOL * )0 \
                    ); \
                return 1; \
            } \
        }; \
        META_SCRIPTABLE_LUA_REGISTERER::RegisterFunction( binding, EQUAL_OPERATOR::Call, ".Equal" );

    // ~~

    #define META_SCRIPTABLE_LessEqualOperator( _TEMPLATE_1_, _TEMPLATE_2_ ) \
        \
        class LESS_EQUAL_OPERATOR \
            { \
            public : \
            static int Call( lua_State * lua_state ) \
                { \
                META_SCRIPTABLE_CheckArgumentCount( lua_state, 2 ); \
                META_SCRIPTABLE_GetSelf(); \
                META_SCRIPTABLE_PushValue( \
                *( self ) == META_SCRIPTABLE_GetValue( lua_state, 2, ( _TEMPLATE_2_ * )0 ) , \
                lua_state, \
                ( BOOL * )0 \
                ); \
                return 1; \
                } \
            }; \
            META_SCRIPTABLE_LUA_REGISTERER::RegisterFunction( binding, LESS_EQUAL_OPERATOR::Call, ".LessEqual" );

    // ~~

    #define META_SCRIPTABLE_LessThanOperator( _TEMPLATE_1_, _TEMPLATE_2_ ) \
        \
        class LESS_THAN_OPERATOR \
            { \
            public : \
            static int Call( lua_State * lua_state ) \
                { \
                META_SCRIPTABLE_CheckArgumentCount( lua_state, 2 ); \
                META_SCRIPTABLE_GetSelf(); \
                META_SCRIPTABLE_PushValue( \
                *( self ) == META_SCRIPTABLE_GetValue( lua_state, 2, ( _TEMPLATE_2_ * )0 ) , \
                lua_state, \
                ( BOOL * )0 \
                ); \
                return 1; \
                } \
            }; \
            META_SCRIPTABLE_LUA_REGISTERER::RegisterFunction( binding, LESS_THAN_OPERATOR::Call, ".LessThan" );

    // ~~

    #define META_SCRIPTABLE_VoidMethod( _Method_ ) \
        META_SCRIPTABLE_RenamedVoidMethod( _Method_, _Method_ ) 

    // ~~

    #define META_SCRIPTABLE_RenamedVoidMethod( _Method_, _Name_ ) \
        \
        class void_METHOD_##_Method_##_Name_ \
        { \
        public : \
            static int Call( lua_State * lua_state ) \
            { \
                META_SCRIPTABLE_CheckArgumentCount( lua_state, 1 ); \
                META_SCRIPTABLE_GetSelf(); \
                self->_Method_(); \
                return 0; \
            } \
        }; \
        META_SCRIPTABLE_LUA_REGISTERER::RegisterFunction( binding, void_METHOD_##_Method_##_Name_::Call, #_Name_ );

    // ~~

    #define META_SCRIPTABLE_VoidMethod1( _Method_, _TEMPLATE_1_ ) \
        META_SCRIPTABLE_RenamedVoidMethod1( _Method_, _Method_,  _TEMPLATE_1_ ) \

    // ~~

    #define META_SCRIPTABLE_RenamedVoidMethod1( _Method_, _Name_,  _TEMPLATE_1_ ) \
        \
        class void_METHOD_1_##_Method_##_Name_ \
        { \
        public : \
            static int Call( lua_State * lua_state ) \
            { \
                META_SCRIPTABLE_CheckArgumentCount( lua_state, 2 ); \
                META_SCRIPTABLE_GetSelf(); \
                self->_Method_( \
                    META_SCRIPTABLE_GetValue( lua_state, 2, ( _TEMPLATE_1_ * )0 ) \
                    ); \
                return 0; \
            } \
        }; \
        META_SCRIPTABLE_LUA_REGISTERER::RegisterFunction( binding, void_METHOD_1_##_Method_##_Name_::Call, #_Name_ );

    // ~~

    #define META_SCRIPTABLE_VoidMethod2( _Method_, _TEMPLATE_1_, _TEMPLATE_2_ ) \
        META_SCRIPTABLE_RenamedVoidMethod2( _Method_, _Method_, _TEMPLATE_1_, _TEMPLATE_2_ ) \

    // ~~

    #define META_SCRIPTABLE_RenamedVoidMethod2( _Method_, _Name_, _TEMPLATE_1_, _TEMPLATE_2_ ) \
        \
        class void_METHOD_2_##_Method_##_Name_ \
        { \
        public : \
            static int Call( lua_State * lua_state ) \
            { \
                META_SCRIPTABLE_CheckArgumentCount( lua_state, 3 ); \
                META_SCRIPTABLE_GetSelf(); \
                self->_Method_( \
                    META_SCRIPTABLE_GetValue( lua_state, 2, ( _TEMPLATE_1_ * )0 ), \
                    META_SCRIPTABLE_GetValue( lua_state, 3, ( _TEMPLATE_2_ * )0 ) \
                    ); \
                return 0; \
            } \
        }; \
        META_SCRIPTABLE_LUA_REGISTERER::RegisterFunction( binding, void_METHOD_2_##_Method_##_Name_::Call, #_Name_ );

    // ~~

    #define META_SCRIPTABLE_VoidMethod3( _Method_, _TEMPLATE_1_, _TEMPLATE_2_, _TEMPLATE_3_ ) \
        META_SCRIPTABLE_RenamedVoidMethod3( _Method_, _Method_, _TEMPLATE_1_, _TEMPLATE_2_, _TEMPLATE_3_ )

    // ~~

    #define META_SCRIPTABLE_RenamedVoidMethod3( _Method_, _Name_, _TEMPLATE_1_, _TEMPLATE_2_, _TEMPLATE_3_ ) \
        \
        class void_METHOD_3_##_Method_##_Name_ \
        { \
        public : \
            static int Call( lua_State * lua_state ) \
            { \
                META_SCRIPTABLE_CheckArgumentCount( lua_state, 4 ); \
                META_SCRIPTABLE_GetSelf(); \
                self->_Method_( \
                    META_SCRIPTABLE_GetValue( lua_state, 2, ( _TEMPLATE_1_ * )0 ), \
                    META_SCRIPTABLE_GetValue( lua_state, 3, ( _TEMPLATE_2_ * )0 ), \
                    META_SCRIPTABLE_GetValue( lua_state, 4, ( _TEMPLATE_3_ * )0 ) \
                    ); \
                return 0; \
            } \
        }; \
        META_SCRIPTABLE_LUA_REGISTERER::RegisterFunction( binding, void_METHOD_3_##_Method_##_Name_::Call, #_Name_ );

    // ~~

    #define META_SCRIPTABLE_VoidMethod4( _Method_, _TEMPLATE_1_, _TEMPLATE_2_, _TEMPLATE_3_, _TEMPLATE_4_ ) \
        META_SCRIPTABLE_RenamedVoidMethod4( _Method_, _Method_, _TEMPLATE_1_, _TEMPLATE_2_, _TEMPLATE_3_, _TEMPLATE_4_ )

    // ~~

    #define META_SCRIPTABLE_RenamedVoidMethod4( _Method_, _Name_, _TEMPLATE_1_, _TEMPLATE_2_, _TEMPLATE_3_, _TEMPLATE_4_  ) \
        \
        class void_METHOD_4_##_Method_##_Name_ \
        { \
        public : \
            static int Call( lua_State * lua_state ) \
            { \
                META_SCRIPTABLE_CheckArgumentCount( lua_state, 5 ); \
                META_SCRIPTABLE_GetSelf(); \
                self->_Method_( \
                    META_SCRIPTABLE_GetValue( lua_state, 2, ( _TEMPLATE_1_ * )0 ), \
                    META_SCRIPTABLE_GetValue( lua_state, 3, ( _TEMPLATE_2_ * )0 ), \
                    META_SCRIPTABLE_GetValue( lua_state, 4, ( _TEMPLATE_3_ * )0 ), \
                    META_SCRIPTABLE_GetValue( lua_state, 5, ( _TEMPLATE_4_ * )0 ) \
                    ); \
                return 0; \
            } \
        }; \
        META_SCRIPTABLE_LUA_REGISTERER::RegisterFunction( binding, void_METHOD_4_##_Method_##_Name_::Call, #_Name_ );

    // ~~

    #define META_SCRIPTABLE_VoidMethod5( _Method_, _TEMPLATE_1_, _TEMPLATE_2_, _TEMPLATE_3_, _TEMPLATE_4_, _TEMPLATE_5_ ) \
        META_SCRIPTABLE_RenamedVoidMethod5( _Method_, _Method_, _TEMPLATE_1_, _TEMPLATE_2_, _TEMPLATE_3_, _TEMPLATE_4_, _TEMPLATE_5_ )

    // ~~

    #define META_SCRIPTABLE_RenamedVoidMethod5( _Method_, _Name_ , _TEMPLATE_1_, _TEMPLATE_2_, _TEMPLATE_3_, _TEMPLATE_4_, _TEMPLATE_5_  ) \
        \
        class void_METHOD_5_##_Method_##_Name_ \
        { \
        public : \
            static int Call( lua_State * lua_state ) \
            { \
                META_SCRIPTABLE_CheckArgumentCount( lua_state, 6 ); \
                META_SCRIPTABLE_GetSelf(); \
                self->_Method_( \
                    META_SCRIPTABLE_GetValue( lua_state, 2, ( _TEMPLATE_1_ * )0 ), \
                    META_SCRIPTABLE_GetValue( lua_state, 3, ( _TEMPLATE_2_ * )0 ), \
                    META_SCRIPTABLE_GetValue( lua_state, 4, ( _TEMPLATE_3_ * )0 ), \
                    META_SCRIPTABLE_GetValue( lua_state, 5, ( _TEMPLATE_4_ * )0 ), \
                    META_SCRIPTABLE_GetValue( lua_state, 6, ( _TEMPLATE_5_ * )0 ) \
                    ); \
                return 0; \
            } \
        }; \
        META_SCRIPTABLE_LUA_REGISTERER::RegisterFunction( binding, void_METHOD_5_##_Method_##_Name_::Call, #_Name_ );

    // ~~

    #define META_SCRIPTABLE_ResultMethod( _Method_, _RESULT_ ) \
        META_SCRIPTABLE_RenamedResultMethod( _Method_, _Method_, _RESULT_ )

    // ~~

    #define META_SCRIPTABLE_RenamedResultMethod( _Method_, _Name_, _RESULT_ ) \
        \
        class RESULT_METHOD_##_Method_##_Name_ \
        { \
        public : \
            static int Call( lua_State * lua_state ) \
            { \
                META_SCRIPTABLE_CheckArgumentCount( lua_state, 1 ); \
                META_SCRIPTABLE_GetSelf(); \
                META_SCRIPTABLE_PushValue( \
                    self->_Method_(), \
                    lua_state, \
                    ( _RESULT_ * )0 \
                    ); \
                return 1; \
            } \
        }; \
        META_SCRIPTABLE_LUA_REGISTERER::RegisterFunction( binding, RESULT_METHOD_##_Method_##_Name_::Call, #_Name_ );

    // ~~

    #define META_SCRIPTABLE_ResultMethod1( _Method_, _RESULT_, _TEMPLATE_1_ ) \
        META_SCRIPTABLE_RenamedResultMethod1( _Method_, _Method_, _RESULT_, _TEMPLATE_1_ )

    // ~~

    #define META_SCRIPTABLE_RenamedResultMethod1( _Method_, _Name_, _RESULT_, _TEMPLATE_1_ ) \
        \
        class RESULT_METHOD_1_##_Method_##_Name_ \
        { \
        public : \
            static int Call( lua_State * lua_state ) \
            { \
                META_SCRIPTABLE_CheckArgumentCount( lua_state, 2 ); \
                META_SCRIPTABLE_GetSelf(); \
                META_SCRIPTABLE_PushValue( \
                    self->_Method_( \
                        META_SCRIPTABLE_GetValue( lua_state, 2, ( _TEMPLATE_1_ * )0 ) \
                        ), \
                    lua_state, \
                    ( _RESULT_ * )0 \
                    ); \
                return 1; \
            } \
        }; \
        META_SCRIPTABLE_LUA_REGISTERER::RegisterFunction( binding, RESULT_METHOD_1_##_Method_##_Name_::Call, #_Name_ );

    // ~~

    #define META_SCRIPTABLE_ResultMethod2( _Method_, _RESULT_, _TEMPLATE_1_, _TEMPLATE_2_ ) \
        META_SCRIPTABLE_RenamedResultMethod2( _Method_, _Method_, _RESULT_, _TEMPLATE_1_, _TEMPLATE_2_ )

    // ~~

    #define META_SCRIPTABLE_RenamedResultMethod2( _Method_, _Name_, _RESULT_, _TEMPLATE_1_, _TEMPLATE_2_ ) \
        \
        class RESULT_METHOD_2_##_Method_##_Name_ \
        { \
        public : \
            static int Call( lua_State * lua_state ) \
            { \
                META_SCRIPTABLE_CheckArgumentCount( lua_state, 3 ); \
                META_SCRIPTABLE_GetSelf(); \
                META_SCRIPTABLE_PushValue( \
                    self->_Method_( \
                        META_SCRIPTABLE_GetValue( lua_state, 2, ( _TEMPLATE_1_ * )0 ), \
                        META_SCRIPTABLE_GetValue( lua_state, 3, ( _TEMPLATE_2_ * )0 ) \
                        ), \
                    lua_state, \
                    ( _RESULT_ * )0 \
                    ); \
                return 1; \
            } \
        }; \
        META_SCRIPTABLE_LUA_REGISTERER::RegisterFunction( binding, RESULT_METHOD_2_##_Method_##_Name_::Call, #_Name_ );

    // ~~

    #define META_SCRIPTABLE_ResultMethod3( _Method_, _RESULT_, _TEMPLATE_1_, _TEMPLATE_2_, _TEMPLATE_3_ ) \
        META_SCRIPTABLE_RenamedResultMethod3( _Method_, _Method_, _RESULT_, _TEMPLATE_1_, _TEMPLATE_2_, _TEMPLATE_3_ ) \

    // ~~

    #define META_SCRIPTABLE_RenamedResultMethod3( _Method_, _Name_, _RESULT_, _TEMPLATE_1_, _TEMPLATE_2_, _TEMPLATE_3_ ) \
        \
        class RESULT_METHOD_3_##_Method_##_Name_ \
        { \
        public : \
            static int Call( lua_State * lua_state ) \
            { \
                META_SCRIPTABLE_CheckArgumentCount( lua_state, 4 ); \
                META_SCRIPTABLE_GetSelf(); \
                META_SCRIPTABLE_PushValue( \
                    self->_Method_( \
                        META_SCRIPTABLE_GetValue( lua_state, 2, ( _TEMPLATE_1_ * )0 ), \
                        META_SCRIPTABLE_GetValue( lua_state, 3, ( _TEMPLATE_2_ * )0 ), \
                        META_SCRIPTABLE_GetValue( lua_state, 4, ( _TEMPLATE_3_ * )0 ) \
                        ), \
                    lua_state, \
                    ( _RESULT_ * )0 \
                    ); \
                return 1; \
            } \
        }; \
        META_SCRIPTABLE_LUA_REGISTERER::RegisterFunction( binding, RESULT_METHOD_3_##_Method_##_Name_::Call, #_Name_ );

    // ~~

    #define META_SCRIPTABLE_VoidFunction( _Function_ ) \
        META_SCRIPTABLE_RenamedVoidFunction( _Function_, _Function_ )

    // ~~

    #define META_SCRIPTABLE_RenamedVoidFunction( _Function_, _Name_ ) \
        \
        class void_FUNCTION_##_Function_##_Name_ \
        { \
        public : \
            static int Call( lua_State * lua_state ) \
            { \
                CLASS::_Function_(); \
                return 0; \
            } \
        }; \
        META_SCRIPTABLE_LUA_REGISTERER::RegisterFunction( binding, void_FUNCTION_##_Function_##_Name_::Call, #_Name_ );

    // ~~

    #define META_SCRIPTABLE_VoidFunction1( _Function_, _TEMPLATE_1_ ) \
        META_SCRIPTABLE_RenamedVoidFunction1( _Function_, _Function_, _TEMPLATE_1_ )

    // ~~

    #define META_SCRIPTABLE_RenamedVoidFunction1( _Function_, _Name_, _TEMPLATE_1_ ) \
        \
        class void_FUNCTION_1_##_Function_##_Name_ \
        { \
        public : \
            static int Call( lua_State * lua_state ) \
            { \
                META_SCRIPTABLE_CheckArgumentCount( lua_state, 2 ); \
                CLASS::_Function_( \
                    META_SCRIPTABLE_GetValue( lua_state, 2, ( _TEMPLATE_1_ * )0 ) \
                    ); \
                return 0; \
            } \
        }; \
        META_SCRIPTABLE_LUA_REGISTERER::RegisterFunction( binding, void_FUNCTION_1_##_Function_##_Name_::Call, #_Name_ );

    // ~~

    #define META_SCRIPTABLE_VoidFunction2( _Function_, _TEMPLATE_1_, _TEMPLATE_2_ ) \
        META_SCRIPTABLE_RenamedVoidFunction2( _Function_, _Function_, _TEMPLATE_1_, _TEMPLATE_2_ )

    // ~~

    #define META_SCRIPTABLE_RenamedVoidFunction2( _Function_, _Name_, _TEMPLATE_1_, _TEMPLATE_2_ ) \
        \
        class void_FUNCTION_2_##_Function_##_Name_ \
        { \
        public : \
            static int Call( lua_State * lua_state ) \
            { \
                META_SCRIPTABLE_CheckArgumentCount( lua_state, 3 ); \
                CLASS::_Function_( \
                    META_SCRIPTABLE_GetValue( lua_state, 2, ( _TEMPLATE_1_ * )0 ), \
                    META_SCRIPTABLE_GetValue( lua_state, 3, ( _TEMPLATE_2_ * )0 ) \
                    ); \
                return 0; \
            } \
        }; \
        META_SCRIPTABLE_LUA_REGISTERER::RegisterFunction( binding, void_FUNCTION_2_##_Function_##_Name_::Call, #_Name_ );

    // ~~

    #define META_SCRIPTABLE_VoidFunction3( _Function_, _TEMPLATE_1_, _TEMPLATE_2_, _TEMPLATE_3_ ) \
        META_SCRIPTABLE_RenamedVoidFunction3( _Function_, _Function_, _TEMPLATE_1_, _TEMPLATE_2_, _TEMPLATE_3_ )

    // ~~

    #define META_SCRIPTABLE_RenamedVoidFunction3( _Function_, _Name_, _TEMPLATE_1_, _TEMPLATE_2_, _TEMPLATE_3_ ) \
        \
        class void_FUNCTION_3_##_Function_##_Name_ \
        { \
        public : \
            static int Call( lua_State * lua_state ) \
            { \
                META_SCRIPTABLE_CheckArgumentCount( lua_state, 4 ); \
                CLASS::_Function_( \
                    META_SCRIPTABLE_GetValue( lua_state, 2, ( _TEMPLATE_1_ * )0 ), \
                    META_SCRIPTABLE_GetValue( lua_state, 3, ( _TEMPLATE_2_ * )0 ), \
                    META_SCRIPTABLE_GetValue( lua_state, 4, ( _TEMPLATE_3_ * )0 ) \
                    ); \
                return 0; \
            } \
        }; \
        META_SCRIPTABLE_LUA_REGISTERER::RegisterFunction( binding, void_FUNCTION_3_##_Function_##_Name_::Call, #_Name_ );

    // ~~

    #define META_SCRIPTABLE_VoidFunction4( _Function_, _TEMPLATE_1_, _TEMPLATE_2_, _TEMPLATE_3_, _TEMPLATE_4_ ) \
        META_SCRIPTABLE_RenamedVoidFunction4( _Function_, _Function_, _TEMPLATE_1_, _TEMPLATE_2_, _TEMPLATE_3_, _TEMPLATE_4_ )

    // ~~

    #define META_SCRIPTABLE_RenamedVoidFunction4( _Function_, _Name_, _TEMPLATE_1_, _TEMPLATE_2_, _TEMPLATE_3_, _TEMPLATE_4_ ) \
        \
        class void_FUNCTION_4_##_Function_##_Name_ \
        { \
        public : \
            static int Call( lua_State * lua_state ) \
            { \
                META_SCRIPTABLE_CheckArgumentCount( lua_state, 5 ); \
                CLASS::_Function_( \
                    META_SCRIPTABLE_GetValue( lua_state, 2, ( _TEMPLATE_1_ * )0 ), \
                    META_SCRIPTABLE_GetValue( lua_state, 3, ( _TEMPLATE_2_ * )0 ), \
                    META_SCRIPTABLE_GetValue( lua_state, 4, ( _TEMPLATE_3_ * )0 ), \
                    META_SCRIPTABLE_GetValue( lua_state, 5, ( _TEMPLATE_4_ * )0 ) \
                    ); \
                return 0; \
            } \
        }; \
        META_SCRIPTABLE_LUA_REGISTERER::RegisterFunction( binding, void_FUNCTION_4_##_Function_##_Name_::Call, #_Name_ );

    // ~~

    #define META_SCRIPTABLE_VoidFunction5( _Function_, _TEMPLATE_1_, _TEMPLATE_2_, _TEMPLATE_3_, _TEMPLATE_4_, _TEMPLATE_5_ ) \
        META_SCRIPTABLE_RenamedVoidFunction5( _Function_, _Function_, _TEMPLATE_1_, _TEMPLATE_2_, _TEMPLATE_3_, _TEMPLATE_4_, _TEMPLATE_5_ )

    // ~~

    #define META_SCRIPTABLE_RenamedVoidFunction5( _Function_, _Name_, _TEMPLATE_1_, _TEMPLATE_2_, _TEMPLATE_3_, _TEMPLATE_4_, _TEMPLATE_5_ ) \
        \
        class void_FUNCTION_5_##_Function_##_Name_ \
        { \
        public : \
            static int Call( lua_State * lua_state ) \
            { \
                META_SCRIPTABLE_CheckArgumentCount( lua_state, 6 ); \
                CLASS::_Function_( \
                    META_SCRIPTABLE_GetValue( lua_state, 2, ( _TEMPLATE_1_ * )0 ), \
                    META_SCRIPTABLE_GetValue( lua_state, 3, ( _TEMPLATE_2_ * )0 ), \
                    META_SCRIPTABLE_GetValue( lua_state, 4, ( _TEMPLATE_3_ * )0 ), \
                    META_SCRIPTABLE_GetValue( lua_state, 5, ( _TEMPLATE_4_ * )0 ), \
                    META_SCRIPTABLE_GetValue( lua_state, 6, ( _TEMPLATE_5_ * )0 ) \
                    ); \
                return 0; \
            } \
        }; \
        META_SCRIPTABLE_LUA_REGISTERER::RegisterFunction( binding, void_FUNCTION_5_##_Function_##_Name_::Call, #_Name_ );

    // ~~

    #define META_SCRIPTABLE_VoidCustomFunction( _Function_ ) \
        \
        META_SCRIPTABLE_LUA_REGISTERER::RegisterFunction( binding, &CLASS::_Function_, #_Function_ );

    // ~~

    #define META_SCRIPTABLE_ResultFunction( _Function_, _RESULT_ ) \
        META_SCRIPTABLE_RenamedResultFunction( _Function_, _Function_, _RESULT_ )

    // ~~

    #define META_SCRIPTABLE_RenamedResultFunction( _Function_, _Name_, _RESULT_ ) \
        \
        class RESULT_FUNCTION_##_Function_##_Name_ \
        { \
        public : \
            static int Call( lua_State * lua_state ) \
            { \
                META_SCRIPTABLE_PushValue( \
                    CLASS::_Function_(), \
                    lua_state, \
                    ( _RESULT_ * )0 \
                    ); \
                return 1; \
            } \
        }; \
        META_SCRIPTABLE_LUA_REGISTERER::RegisterFunction( binding, RESULT_FUNCTION_##_Function_##_Name_::Call, #_Name_ );

    // ~~

    #define META_SCRIPTABLE_ResultFunction1( _Function_, _RESULT_, _TEMPLATE_1_ ) \
        META_SCRIPTABLE_RenamedResultFunction1( _Function_, _Function_, _RESULT_, _TEMPLATE_1_ )

    // ~~

    #define META_SCRIPTABLE_RenamedResultFunction1( _Function_, _Name_, _RESULT_, _TEMPLATE_1_ ) \
        \
        class RESULT_FUNCTION_1_##_Function_##_Name_ \
        { \
        public : \
            static int Call( lua_State * lua_state ) \
            { \
                META_SCRIPTABLE_CheckArgumentCount( lua_state, 2 ); \
                META_SCRIPTABLE_PushValue( \
                    CLASS::_Function_( \
                        META_SCRIPTABLE_GetValue( lua_state, 2, ( _TEMPLATE_1_ * )0 ) \
                        ), \
                    lua_state, \
                    ( _RESULT_ * )0 \
                    ); \
                return 1; \
            } \
        }; \
        META_SCRIPTABLE_LUA_REGISTERER::RegisterFunction( binding, RESULT_FUNCTION_1_##_Function_##_Name_::Call, #_Name_ );

    // ~~

    #define META_SCRIPTABLE_ResultFunction2( _Function_, _RESULT_, _TEMPLATE_1_, _TEMPLATE_2_ ) \
        META_SCRIPTABLE_RenamedResultFunction2( _Function_, _Function_, _RESULT_, _TEMPLATE_1_, _TEMPLATE_2_ )

    // ~~

    #define META_SCRIPTABLE_RenamedResultFunction2( _Function_, _Name_, _RESULT_, _TEMPLATE_1_, _TEMPLATE_2_ ) \
        \
        class RESULT_FUNCTION_2_##_Function_##_Name_ \
        { \
        public : \
            static int Call( lua_State * lua_state ) \
            { \
                META_SCRIPTABLE_CheckArgumentCount( lua_state, 3 ); \
                META_SCRIPTABLE_PushValue( \
                    CLASS::_Function_( \
                        META_SCRIPTABLE_GetValue( lua_state, 2, ( _TEMPLATE_1_ * )0 ), \
                        META_SCRIPTABLE_GetValue( lua_state, 3, ( _TEMPLATE_2_ * )0 ) \
                        ), \
                    lua_state, \
                    ( _RESULT_ * )0 \
                    ); \
                return 1; \
            } \
        }; \
        META_SCRIPTABLE_LUA_REGISTERER::RegisterFunction( binding, RESULT_FUNCTION_2_##_Function_##_Name_::Call, #_Name_ );

    // ~~

    #define META_SCRIPTABLE_ResultFunction3( _Function_, _RESULT_, _TEMPLATE_1_, _TEMPLATE_2_, _TEMPLATE_3_ ) \
        META_SCRIPTABLE_RenamedResultFunction3( _Function_, _Function_, _RESULT_, _TEMPLATE_1_, _TEMPLATE_2_, _TEMPLATE_3_ )

    // ~~

    #define META_SCRIPTABLE_RenamedResultFunction3( _Function_, _Name_, _RESULT_, _TEMPLATE_1_, _TEMPLATE_2_, _TEMPLATE_3_ ) \
        \
        class RESULT_FUNCTION_3_##_Function_##_Name_ \
        { \
        public : \
            static int Call( lua_State * lua_state ) \
            { \
                META_SCRIPTABLE_CheckArgumentCount( lua_state, 4 ); \
                META_SCRIPTABLE_PushValue( \
                    CLASS::_Function_( \
                        META_SCRIPTABLE_GetValue( lua_state, 2, ( _TEMPLATE_1_ * )0 ), \
                        META_SCRIPTABLE_GetValue( lua_state, 3, ( _TEMPLATE_2_ * )0 ), \
                        META_SCRIPTABLE_GetValue( lua_state, 4, ( _TEMPLATE_3_ * )0 ) \
                        ), \
                    lua_state, \
                    ( _RESULT_ * )0 \
                    ); \
                return 1; \
            } \
        }; \
        META_SCRIPTABLE_LUA_REGISTERER::RegisterFunction( binding, RESULT_FUNCTION_3_##_Function_##_Name_::Call, #_Name_ );

    // ~~

    #define META_SCRIPTABLE_LuaFunction( _Function_ ) \
        \
        META_SCRIPTABLE_LUA_REGISTERER::RegisterFunction( binding, CLASS::_Function_, #_Function_ );

    // ~~

    #define META_SCRIPTABLE_VoidSingletonMethod( _Method_ ) \
        META_SCRIPTABLE_RenamedVoidSingletonMethod( _Method_, _Method_ )

    // ~~

    #define META_SCRIPTABLE_RenamedVoidSingletonMethod( _Method_, _Name_ ) \
        \
    class void_SINGLETON_METHOD_##_Method_##_Name_ \
        { \
        public : \
            static int Call( lua_State * lua_state ) \
            { \
                CLASS::GetInstance()._Method_(); \
                return 0; \
            } \
        }; \
        META_SCRIPTABLE_LUA_REGISTERER::RegisterFunction( binding, void_SINGLETON_METHOD_##_Method_##_Name_::Call, #_Name_ );

    // ~~

    #define META_SCRIPTABLE_VoidSingletonMethod1( _Method_, _TEMPLATE_1_ ) \
        META_SCRIPTABLE_RenamedVoidSingletonMethod1( _Method_, _Method_, _TEMPLATE_1_ )

    // ~~

    #define META_SCRIPTABLE_RenamedVoidSingletonMethod1( _Method_, _Name_, _TEMPLATE_1_ ) \
        \
    class void_SINGLETON_METHOD_1_##_Method_##_Name_ \
        { \
        public : \
        static int Call( lua_State * lua_state ) \
            { \
                META_SCRIPTABLE_CheckArgumentCount( lua_state, 2 ); \
                CLASS::GetInstance()._Method_( \
                META_SCRIPTABLE_GetValue( lua_state, 2, ( _TEMPLATE_1_ * )0 ) \
                ); \
                return 0; \
            } \
        }; \
        META_SCRIPTABLE_LUA_REGISTERER::RegisterFunction( binding, void_SINGLETON_METHOD_1_##_Method_##_Name_::Call, #_Name_ );

    // ~~

    #define META_SCRIPTABLE_VoidSingletonMethod2( _Method_, _TEMPLATE_1_, _TEMPLATE_2_ ) \
        META_SCRIPTABLE_RenamedVoidSingletonMethod2( _Method_, _Method_, _TEMPLATE_1_, _TEMPLATE_2_ )

    // ~~

    #define META_SCRIPTABLE_RenamedVoidSingletonMethod2( _Method_, _Name_, _TEMPLATE_1_, _TEMPLATE_2_ ) \
        \
    class void_SINGLETON_METHOD_2_##_Method_##_Name_ \
        { \
        public : \
        static int Call( lua_State * lua_state ) \
            { \
                META_SCRIPTABLE_CheckArgumentCount( lua_state, 3 ); \
                CLASS::GetInstance()._Method_( \
                META_SCRIPTABLE_GetValue( lua_state, 2, ( _TEMPLATE_1_ * )0 ), \
                META_SCRIPTABLE_GetValue( lua_state, 3, ( _TEMPLATE_2_ * )0 ) \
                ); \
                return 0; \
            } \
        }; \
        META_SCRIPTABLE_LUA_REGISTERER::RegisterFunction( binding, void_SINGLETON_METHOD_2_##_Method_##_Name_::Call, #_Name_ );

    // ~~

    #define META_SCRIPTABLE_VoidSingletonMethod3( _Method_, _TEMPLATE_1_, _TEMPLATE_2_, _TEMPLATE_3_ ) \
        META_SCRIPTABLE_RenamedVoidSingletonMethod3( _Method_, _Method_, _TEMPLATE_1_, _TEMPLATE_2_, _TEMPLATE_3_ )

    // ~~

    #define META_SCRIPTABLE_RenamedVoidSingletonMethod3( _Method_, _Name_, _TEMPLATE_1_, _TEMPLATE_2_, _TEMPLATE_3_ ) \
        \
    class void_SINGLETON_METHOD_3_##_Method_##_Name_ \
        { \
        public : \
        static int Call( lua_State * lua_state ) \
            { \
                META_SCRIPTABLE_CheckArgumentCount( lua_state, 4 ); \
                CLASS::GetInstance()._Method_( \
                    META_SCRIPTABLE_GetValue( lua_state, 2, ( _TEMPLATE_1_ * )0 ), \
                    META_SCRIPTABLE_GetValue( lua_state, 3, ( _TEMPLATE_2_ * )0 ), \
                    META_SCRIPTABLE_GetValue( lua_state, 4, ( _TEMPLATE_3_ * )0 ) \
                ); \
                return 0; \
            } \
        }; \
        META_SCRIPTABLE_LUA_REGISTERER::RegisterFunction( binding, void_SINGLETON_METHOD_3_##_Method_##_Name_::Call, #_Name_ );

    // ~~

    #define META_SCRIPTABLE_VoidSingletonMethod4( _Method_, _TEMPLATE_1_, _TEMPLATE_2_, _TEMPLATE_3_, _TEMPLATE_4_ ) \
        META_SCRIPTABLE_RenamedVoidSingletonMethod4( _Method_, _Method_, _TEMPLATE_1_, _TEMPLATE_2_, _TEMPLATE_3_, _TEMPLATE_4_ )

    // ~~

    #define META_SCRIPTABLE_RenamedVoidSingletonMethod4( _Method_, _Name_, _TEMPLATE_1_, _TEMPLATE_2_, _TEMPLATE_3_, _TEMPLATE_4_ ) \
        \
    class void_SINGLETON_METHOD_4_##_Method_##_Name_ \
        { \
        public : \
        static int Call( lua_State * lua_state ) \
            { \
                META_SCRIPTABLE_CheckArgumentCount( lua_state, 5 ); \
                    CLASS::GetInstance()._Method_( \
                    META_SCRIPTABLE_GetValue( lua_state, 2, ( _TEMPLATE_1_ * )0 ), \
                    META_SCRIPTABLE_GetValue( lua_state, 3, ( _TEMPLATE_2_ * )0 ), \
                    META_SCRIPTABLE_GetValue( lua_state, 4, ( _TEMPLATE_3_ * )0 ), \
                    META_SCRIPTABLE_GetValue( lua_state, 5, ( _TEMPLATE_4_ * )0 ) \
                ); \
                return 0; \
            } \
        }; \
        META_SCRIPTABLE_LUA_REGISTERER::RegisterFunction( binding, void_SINGLETON_METHOD_4_##_Method_##_Name_::Call, #_Name_ );

    // ~~

    #define META_SCRIPTABLE_VoidSingletonMethod5( _Method_, _TEMPLATE_1_, _TEMPLATE_2_, _TEMPLATE_3_, _TEMPLATE_4_, _TEMPLATE_5_ ) \
        META_SCRIPTABLE_RenamedVoidSingletonMethod5( _Method_, _Method_, _TEMPLATE_1_, _TEMPLATE_2_, _TEMPLATE_3_, _TEMPLATE_4_, _TEMPLATE_5_ )

    // ~~

    #define META_SCRIPTABLE_RenamedVoidSingletonMethod5( _Method_, _Name_, _TEMPLATE_1_, _TEMPLATE_2_, _TEMPLATE_3_, _TEMPLATE_4_, _TEMPLATE_5_ ) \
        \
    class void_SINGLETON_METHOD_5_##_Method_##_Name_ \
        { \
        public : \
        static int Call( lua_State * lua_state ) \
            { \
                META_SCRIPTABLE_CheckArgumentCount( lua_state, 6 ); \
                    CLASS::GetInstance()._Method_( \
                    META_SCRIPTABLE_GetValue( lua_state, 2, ( _TEMPLATE_1_ * )0 ), \
                    META_SCRIPTABLE_GetValue( lua_state, 3, ( _TEMPLATE_2_ * )0 ), \
                    META_SCRIPTABLE_GetValue( lua_state, 4, ( _TEMPLATE_3_ * )0 ), \
                    META_SCRIPTABLE_GetValue( lua_state, 5, ( _TEMPLATE_4_ * )0 ), \
                    META_SCRIPTABLE_GetValue( lua_state, 6, ( _TEMPLATE_5_ * )0 ) \
                ); \
                return 0; \
            } \
        }; \
        META_SCRIPTABLE_LUA_REGISTERER::RegisterFunction( binding, void_SINGLETON_METHOD_5_##_Method_##_Name_::Call, #_Name_ );

    // ~~

    #define META_SCRIPTABLE_ResultSingletonMethod( _Method_, _RESULT_ ) \
        META_SCRIPTABLE_RenamedResultSingletonMethod( _Method_, _Method_, _RESULT_ )

    // ~~

    #define META_SCRIPTABLE_RenamedResultSingletonMethod( _Method_, _Name_, _RESULT_ ) \
        \
        class RESULT_METHOD_##_Method_##_Name_ \
        { \
        public : \
            static int Call( lua_State * lua_state ) \
            { \
                META_SCRIPTABLE_PushValue( \
                    CLASS::GetInstance()._Method_(), \
                    lua_state, \
                    ( _RESULT_ * )0 \
                    ); \
                return 1; \
            } \
        }; \
        META_SCRIPTABLE_LUA_REGISTERER::RegisterFunction( binding, RESULT_METHOD_##_Method_##_Name_::Call, #_Name_ );

    // ~~

    #define META_SCRIPTABLE_ResultSingletonMethod1( _Method_, _RESULT_, _TEMPLATE_1_ ) \
        META_SCRIPTABLE_RenamedResultSingletonMethod1( _Method_, _Method_, _RESULT_, _TEMPLATE_1_ )

    // ~~

    #define META_SCRIPTABLE_RenamedResultSingletonMethod1( _Method_, _Name_, _RESULT_, _TEMPLATE_1_ ) \
        \
        class RESULT_METHOD_1_##_Method_##_Name_ \
        { \
        public : \
            static int Call( lua_State * lua_state ) \
            { \
                META_SCRIPTABLE_CheckArgumentCount( lua_state, 2 ); \
                META_SCRIPTABLE_PushValue( \
                    CLASS::GetInstance()._Method_( \
                        META_SCRIPTABLE_GetValue( lua_state, 2, ( _TEMPLATE_1_ * )0 ) \
                        ), \
                    lua_state, \
                    ( _RESULT_ * )0 \
                    ); \
                return 1; \
            } \
        }; \
        META_SCRIPTABLE_LUA_REGISTERER::RegisterFunction( binding, RESULT_METHOD_1_##_Method_##_Name_::Call, #_Name_ );

    // ~~

    #define META_SCRIPTABLE_ResultSingletonMethod2( _Method_, _RESULT_, _TEMPLATE_1_, _TEMPLATE_2_ ) \
        META_SCRIPTABLE_RenamedResultSingletonMethod2( _Method_, _Method_, _RESULT_, _TEMPLATE_1_, _TEMPLATE_2_ )

    // ~~

    #define META_SCRIPTABLE_RenamedResultSingletonMethod2( _Method_, _Name_, _RESULT_, _TEMPLATE_1_, _TEMPLATE_2_ ) \
        \
        class RESULT_METHOD_2_##_Method_##_Name_ \
        { \
        public : \
            static int Call( lua_State * lua_state ) \
            { \
                META_SCRIPTABLE_CheckArgumentCount( lua_state, 3 ); \
                META_SCRIPTABLE_PushValue( \
                    CLASS::GetInstance()._Method_( \
                        META_SCRIPTABLE_GetValue( lua_state, 2, ( _TEMPLATE_1_ * )0 ), \
                        META_SCRIPTABLE_GetValue( lua_state, 3, ( _TEMPLATE_2_ * )0 ) \
                        ), \
                    lua_state, \
                    ( _RESULT_ * )0 \
                    ); \
                return 1; \
            } \
        }; \
        META_SCRIPTABLE_LUA_REGISTERER::RegisterFunction( binding, RESULT_METHOD_2_##_Method_##_Name_::Call, #_Name_ );

    // ~~

    #define META_SCRIPTABLE_ResultSingletonMethod3( _Method_, _RESULT_, _TEMPLATE_1_, _TEMPLATE_2_, _TEMPLATE_3_ ) \
        META_SCRIPTABLE_RenamedResultSingletonMethod3( _Method_, _Method_, _RESULT_, _TEMPLATE_1_, _TEMPLATE_2_, _TEMPLATE_3_ )

    // ~~

    #define META_SCRIPTABLE_RenamedResultSingletonMethod3( _Method_, _Name_, _RESULT_, _TEMPLATE_1_, _TEMPLATE_2_, _TEMPLATE_3_ ) \
        \
        class RESULT_METHOD_3_##_Method_##_Name_ \
        { \
        public : \
            static int Call( lua_State * lua_state ) \
            { \
                META_SCRIPTABLE_CheckArgumentCount( lua_state, 4 ); \
                META_SCRIPTABLE_PushValue( \
                    CLASS::GetInstance()._Method_( \
                        META_SCRIPTABLE_GetValue( lua_state, 2, ( _TEMPLATE_1_ * )0 ), \
                        META_SCRIPTABLE_GetValue( lua_state, 3, ( _TEMPLATE_2_ * )0 ), \
                        META_SCRIPTABLE_GetValue( lua_state, 4, ( _TEMPLATE_3_ * )0 ) \
                        ), \
                    lua_state, \
                    ( _RESULT_ * )0 \
                    ); \
                return 1; \
            } \
        }; \
        META_SCRIPTABLE_LUA_REGISTERER::RegisterFunction( binding, RESULT_METHOD_3_##_Method_##_Name_::Call, #_Name_ );

    // ~~

    #define META_SCRIPTABLE_ResultSingletonMethod4( _Method_, _RESULT_, _TEMPLATE_1_, _TEMPLATE_2_, _TEMPLATE_3_, _TEMPLATE_4_ ) \
        META_SCRIPTABLE_RenamedResultSingletonMethod4( _Method_, _Method_, _RESULT_, _TEMPLATE_1_, _TEMPLATE_2_, _TEMPLATE_3_, _TEMPLATE_4_ )

    // ~~

    #define META_SCRIPTABLE_RenamedResultSingletonMethod4( _Method_, _Name_, _RESULT_, _TEMPLATE_1_, _TEMPLATE_2_, _TEMPLATE_3_, _TEMPLATE_4_ ) \
        \
    class RESULT_METHOD_4_##_Method_##_Name_ \
        { \
        public : \
        static int Call( lua_State * lua_state ) \
            { \
            META_SCRIPTABLE_CheckArgumentCount( lua_state, 5 ); \
            META_SCRIPTABLE_PushValue( \
            CLASS::GetInstance()._Method_( \
            META_SCRIPTABLE_GetValue( lua_state, 2, ( _TEMPLATE_1_ * )0 ), \
            META_SCRIPTABLE_GetValue( lua_state, 3, ( _TEMPLATE_2_ * )0 ), \
            META_SCRIPTABLE_GetValue( lua_state, 4, ( _TEMPLATE_3_ * )0 ), \
            META_SCRIPTABLE_GetValue( lua_state, 5, ( _TEMPLATE_4_ * )0 ) \
            ), \
            lua_state, \
            ( _RESULT_ * )0 \
            ); \
            return 1; \
            } \
        }; \
        META_SCRIPTABLE_LUA_REGISTERER::RegisterFunction( binding, RESULT_METHOD_4_##_Method_##_Name_::Call, #_Name_ );
#endif