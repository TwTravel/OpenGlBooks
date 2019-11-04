#include "primitive_static_text.h"

// -- LOCAL

// .. REFERENCES

// -- PUBLIC

// .. META

// .. OPERATIONS

void PRIMITIVE_STATIC_TEXT::SetEmpty(
    void
    )
{
    CharacterArray = "";
    CharacterCount = 0;
}

// ~~

void PRIMITIVE_STATIC_TEXT::SetArray(
    const char * character_array,
    const size_t character_count
    )
{
    SetEmpty();

    if ( character_count != 0 )
    {
        CharacterArray = character_array;
        CharacterCount = character_count;
    }
}
