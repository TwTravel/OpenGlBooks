#ifndef __WORLD__
    #define __WORLD__

    // -- LOCAL

    // .. REFERENCES

    #include "meta_scriptable.h"
    #include "counted_object.h"
    #include "entity.h"
    #include <map>
    #include <string>

    // -- GLOBAL

    // .. TYPES

    class WORLD :
        public COUNTED_OBJECT
    {
        // -- PUBLIC

    public :

        // .. META

        META_SCRIPTABLE_DefineClass( WORLD )

        // .. CONSTRUCTORS

        WORLD(
            void
            );

        // ~~

        virtual ~WORLD(
            void
            );

        ENTITY & GetEntityAtName( 
            const std::string & name 
            )
        {
            return *EntityTable[ name ];
        }

        void AddEntity(
            ENTITY & entity,
            const std::string & name
            )
        {
            EntityTable[ name ] = &entity;
        }


        static WORLD & GetInstance()
        {
            if( Instance == NULL )
            {
                Instance = new WORLD;
                Instance->AddRef();
            }

            return *Instance;
        }

        // -- PRIVATE

    private :

        
        std::map<std::string, ENTITY_REF>
            EntityTable;
        static WORLD
            * Instance;
    };
#endif
