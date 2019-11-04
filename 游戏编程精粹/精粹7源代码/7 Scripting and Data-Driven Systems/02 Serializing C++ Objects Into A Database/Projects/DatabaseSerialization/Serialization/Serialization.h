#include "..\Meta\MetaType.h"
#include "..\Utility\Utility.h"

inline void WriteObjectToText( void * object, const MetaType & meta_type, std::string & output_text )
{
    if ( &meta_type == &StringMetaType::metaType )
    {
        std::string
            * string_object = (std::string *)object;

        output_text = "\'";
        output_text += *string_object;
        output_text += "\'";
        
        return;
    }

    if ( &meta_type == &IntMetaType::metaType )
    {
        int
            * int_pointer = (int *)object;

        output_text = "";
        AppendIntToString( output_text, *int_pointer );

        return;
    }

    assert( false );
}

inline void ReadObjectFromText( void * object, const MetaType & meta_type, const std::string & output_text )
{
    if ( &meta_type == &StringMetaType::metaType )
    {
        std::string
            & new_string = *(std::string *)object;
        
        new_string = output_text;

        return;
    }

    if ( &meta_type == &IntMetaType::metaType )
    {
        *((int *)object) = GetIntFromString( output_text );

        return;
    }

    assert( false );
}