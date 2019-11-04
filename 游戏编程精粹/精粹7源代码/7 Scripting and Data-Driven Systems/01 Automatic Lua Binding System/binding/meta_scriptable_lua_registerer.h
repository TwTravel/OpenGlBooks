#ifndef __META_SCRIPTABLE_LUA_REGISTERER__
    #define __META_SCRIPTABLE_LUA_REGISTERER__

    // -- LOCAL

    // .. REFERENCES

    class META_SCRIPTABLE_BINDING_DATA;
    class META_SCRIPTABLE_TYPE;
    #include <lua.h>

    // -- GLOBAL

    // .. TYPES

    struct META_SCRIPTABLE_BINDING_HELPER
    {
        void
            * Object;
        int
            ClassIndex;
    };

    class META_SCRIPTABLE_LUA_REGISTERER
    {
        // -- PUBLIC

    public :

        // .. FUNCTIONS

        static void Init(
            lua_State * lua_state
            );

        // ~~

        static void BeginClass(
            META_SCRIPTABLE_BINDING_DATA & binding,
            int ( *destroy_function ) ( lua_State* ),    // :TODO: typedef this
            META_SCRIPTABLE_TYPE & type
            );

        // ~~

        static void EndClass(
            META_SCRIPTABLE_BINDING_DATA & binding,
            const char * class_name,
            const char * base_class_name
            );

        // ~~

        static void RegisterFunction(
            META_SCRIPTABLE_BINDING_DATA & binding,
            int ( *function ) ( lua_State* ),
            const char * function_name
            );

        // ~~

        static int IndexEventHandler(
            lua_State* lua_state
            );

        // ~~

        static int GlobalIndexEventHandler(
            lua_State* lua_state
            );

        // ~~

		static int GarbageCollectionEventHandler(
            lua_State* lua_state
            );

		// ~~

		static void PushMetatable(
			lua_State * lua_state 
			);

        // .. VARIABLES

        static META_SCRIPTABLE_TYPE
            * CurrentClass;

    private:


        META_SCRIPTABLE_LUA_REGISTERER(
            void
            );

        META_SCRIPTABLE_LUA_REGISTERER(
            const META_SCRIPTABLE_LUA_REGISTERER & other
            );

        ~META_SCRIPTABLE_LUA_REGISTERER(
            void
            );

        META_SCRIPTABLE_LUA_REGISTERER & operator=(
            const META_SCRIPTABLE_LUA_REGISTERER & other
            );
    };
#endif
