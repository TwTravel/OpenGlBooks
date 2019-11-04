#ifndef __META_SCRIPTABLE_BINDING_DATA__
    #define __META_SCRIPTABLE_BINDING_DATA__

    // -- LOCAL

    // .. REFERENCES
    
    #include <vector>
    #include <map>
    #include "primitive_static_text.h"
    #include <lua.h>

    // -- GLOBAL

    // .. TYPES

    class META_SCRIPTABLE_BINDING_DATA
    {
        // -- PUBLIC

    public :

        // .. TYPE

        typedef int ( *BINDING_FUNCTION )( lua_State * );

        // .. CONSTRUCTORS

        META_SCRIPTABLE_BINDING_DATA(
            void
            );

        // ~~

        ~META_SCRIPTABLE_BINDING_DATA(
            void
            );

        // .. OPERATIONS

        int GetClassIndex(
            const char * class_name
            )
        {
            int
                class_index;
            std::map<PRIMITIVE_STATIC_TEXT, int>::iterator
                iterator;

            iterator = ClassIndexTable.find( class_name );

            if ( iterator == ClassIndexTable.end() )
            {
                class_index = int( ClassIndexTable.size() );
                ClassIndexTable[ class_name ] = class_index;

                MethodTable.resize( class_index + 1 );
                ParentIndexTable.resize( class_index + 1 );
				DeleteFunctionTable.resize( class_index + 1 );

                return class_index;
            }

            return iterator->second;
        }

        // ~~

        bool FindClassIndex(
            int & class_index,
            const char * class_name
            )
        {
            std::map<PRIMITIVE_STATIC_TEXT, int>::iterator
                iterator;

            iterator = ClassIndexTable.find( class_name );

            if( iterator != ClassIndexTable.end() )
            {
                class_index = iterator->second;
                return true;
            }

            return false;
        }

        // ~~

        void AddMethodToClass(
            const int class_index,
            const char * function_name,
            const BINDING_FUNCTION function
            )
        {
            if ( !MethodTable[ class_index ] )
            {
                MethodTable[ class_index ] = new std::map<PRIMITIVE_STATIC_TEXT, BINDING_FUNCTION>;
            }

            (*MethodTable[ class_index ] ) [ function_name ] = function;
        }

        // ~~


		void SetDeleteFunction(
			const int class_index,
			const BINDING_FUNCTION function
			)
		{
			DeleteFunctionTable[ class_index ] = function;
		}

		// ~~

        void SetParentClass(
            const int class_index,
            const int parent_class_index
            )
        {
            ParentIndexTable[ class_index ] = parent_class_index;
        }

        // ~~

        void SetNoParentClass(
            const int class_index
            )
        {
            ParentIndexTable[ class_index ] = -1;
        }

        // ~~

        BINDING_FUNCTION GetFunction(
            int class_index,
            const char* method
            )
        {
            int
                current_class_index;
            std::map< PRIMITIVE_STATIC_TEXT, BINDING_FUNCTION>::iterator
                iterator;

            current_class_index = class_index;

            while( current_class_index != -1 )
            {
                if( MethodTable[ current_class_index ] 
                    && ( ( iterator = MethodTable[ current_class_index ]->find( method ) ) != MethodTable[ current_class_index ]->end() )
                    )
                {
                    return iterator->second;
                }

                current_class_index = ParentIndexTable[ current_class_index ];
            }

            return NULL;
        }

		// ~~

		BINDING_FUNCTION GetDeleteFunction(
			int class_index
			)
		{
			return DeleteFunctionTable[ class_index ];
		}

		// ~~

		bool CheckType(
			int class_index,
			int base_class_index
			);

        // -- PRIVATE

    private :

        // .. CONSTRUCTORS

        META_SCRIPTABLE_BINDING_DATA(
            const META_SCRIPTABLE_BINDING_DATA & other
            );

        // .. OPERATORS

        META_SCRIPTABLE_BINDING_DATA & operator=(
            const META_SCRIPTABLE_BINDING_DATA & other
            );

        // .. ATTRIBUTES

        std::map<PRIMITIVE_STATIC_TEXT, int>
            ClassIndexTable;
        std::vector<std::map< PRIMITIVE_STATIC_TEXT, BINDING_FUNCTION>* >
            MethodTable;
		std::vector<BINDING_FUNCTION>
			DeleteFunctionTable;
        std::vector<int>
            ParentIndexTable;
    };
#endif
