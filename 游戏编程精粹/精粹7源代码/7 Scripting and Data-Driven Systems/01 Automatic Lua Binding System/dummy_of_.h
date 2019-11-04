#ifndef __DUMMY_OF___
    #define __DUMMY_OF___

    // -- LOCAL

    // .. REFERENCES

    #include "meta_scriptable.h"

    // -- GLOBAL

    // .. TYPES

    template < typename _ITEM_>
    class DUMMY_OF_
    {
        // -- PUBLIC

    public :

        // .. META

        META_SCRIPTABLE_DefineTemplateClass( DUMMY_OF_, _ITEM_ )

        // .. CONSTRUCTORS

        DUMMY_OF_(
            void
            );

        // ~~


		DUMMY_OF_(
			const DUMMY_OF_ & other
			)
		{
		}

		// ~~

        DUMMY_OF_(
            _ITEM_ & item
            )
        {
            printf( "pointer to argument of dummy object: %p\n", & item );
        }

        // ~~

        ~DUMMY_OF_(
            void
            );

        // -- PRIVATE

    private :

        // .. OPERATORS

        DUMMY_OF_ & operator=(
            const DUMMY_OF_ & other
            );
    };
#endif
