#ifndef __PRIMITIVE_STATIC_TEXT__
    #define __PRIMITIVE_STATIC_TEXT__

    // -- LOCAL

    // .. REFERENCES

    #include <string.h>

    // -- GLOBAL

    // .. TYPES

    class PRIMITIVE_STATIC_TEXT
    {
        // -- PUBLIC

    public :

        // .. CONSTRUCTORS

        PRIMITIVE_STATIC_TEXT(
            void
            ) :
            CharacterArray( "" ),
            CharacterCount( 0 )
        {
        }

        // ~~

        PRIMITIVE_STATIC_TEXT(
            const PRIMITIVE_STATIC_TEXT & other
            ) :
            CharacterArray( "" ),
            CharacterCount( 0 )
        {
            SetArray( other.CharacterArray, other.CharacterCount );
        }

        // ~~

        PRIMITIVE_STATIC_TEXT(
            const char * character_array
            ) :
            CharacterArray( character_array ),
            CharacterCount( strlen( character_array ) )
        {
        }

        // ~~

        ~PRIMITIVE_STATIC_TEXT(
            void
            )
        {
        }

        // .. OPERATORS

        PRIMITIVE_STATIC_TEXT & operator=(
            const PRIMITIVE_STATIC_TEXT & other
            )
        {
            SetArray( other.CharacterArray, other.CharacterCount );

            return *this;
        }

        // ~~

        bool operator==(
            const PRIMITIVE_STATIC_TEXT & other
            ) const
        {
            return !strcmp( CharacterArray, other.CharacterArray );
        }

        // ~~

        bool operator<(
            const PRIMITIVE_STATIC_TEXT & other
            ) const
        {
            return strcmp( CharacterArray, other.CharacterArray ) < 0;
        }

        // ~~

        bool operator==(
            const char * character_array
            ) const
        {
            return !strcmp( CharacterArray, character_array );
        }

        // ~~

        bool operator!=(
            const char * character_array
            ) const
        {
            return !!strcmp( CharacterArray, character_array );
        }

        // ~~

        bool operator<(
            const char * character_array
            ) const
        {
            return strcmp( CharacterArray, character_array ) < 0;
        }

        // ~~

        bool operator>(
            const char * character_array
            ) const
        {
            return strcmp( CharacterArray, character_array ) > 0;
        }

        // ~~

        bool operator<=(
            const char * character_array
            ) const
        {
            return strcmp( CharacterArray, character_array ) <= 0;
        }

        // ~~

        bool operator>=(
            const char * character_array
            ) const
        {
            return strcmp( CharacterArray, character_array ) >= 0;
        }

        // .. INQUIRIES

        bool IsEmpty(
            void
            ) const
        {
            return CharacterCount == 0;
        }

        // ~~

        const char * GetArray(
            void
            ) const
        {
            return CharacterArray;
        }

        // .. OPERATIONS

        void SetEmpty(
            void
            );

        // ~~

        void SetArray(
            const char * character_array,
            const size_t character_count
            );

        // -- PRIVATE

    private :

        // .. ATTRIBUTES

        const char
            * CharacterArray;
        size_t
            CharacterCount;
    };

#endif
