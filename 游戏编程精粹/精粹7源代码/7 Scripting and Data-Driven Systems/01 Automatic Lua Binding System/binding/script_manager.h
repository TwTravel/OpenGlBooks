#ifndef __SCRIPT_MANAGER__
    #define __SCRIPT_MANAGER__

    // -- LOCAL

    // .. REFERENCES

    #include "script.h"
    #include "counted_object.h"
    #include "counted_ref_to_.h"


    #include <lua.h>

    // -- GLOBAL

    // .. TYPES

    class SCRIPT_MANAGER :
        public COUNTED_OBJECT
    {
        // -- PUBLIC

    public :

        friend SCRIPT;

        // .. META

        // .. CONSTRUCTORS

        SCRIPT_MANAGER(
            void
            );

        // ~~

        SCRIPT_MANAGER(
            const SCRIPT_MANAGER & other
            );

        // ~~

        virtual ~SCRIPT_MANAGER(
            void
            );

        // .. OPERATORS

        SCRIPT_MANAGER & operator=(
            const SCRIPT_MANAGER & other
            );

        // .. ACCESSORS

        const META_SCRIPTABLE_BINDING_DATA & GetBindingData(
            void
            ) const
        {
            return BindingData;
        }

        // .. OPERATIONS

        void ExecuteScriptFile(
            const char * script_file_path
            );

        // ~~

        COUNTED_REF_TO_<SCRIPT> CreateScript(
            void
            );

        // -- PRIVATE

    private :

        // .. ATTRIBUTES

        META_SCRIPTABLE_BINDING_DATA
            BindingData;
    };
#endif
