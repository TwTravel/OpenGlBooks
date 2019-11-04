#ifndef __UTILITY__
#define __UTILITY__

inline void AppendIntToString( 
    std::string & destination_string, 
    const int value 
    )
{
    char 
        buffer[ 256 ];

    sprintf( buffer, "%d", value );

    destination_string += buffer;
}

inline int GetIntFromString(
    const std::string & source_string 
    )
{
    int 
        value(0);
    char 
        *ep = 0;

    value = std::strtol(source_string.c_str(), &ep, 10);

    return value;
}
#endif