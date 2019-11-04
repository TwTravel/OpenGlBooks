#ifndef __META_ATTRIBUTE__
#define __META_ATTRIBUTE__

#include <string>
class MetaType;

class MetaAttribute
{
public:

    MetaAttribute()
    {

    }

    MetaAttribute( 
        const char * attribute_name,
        const MetaType & attribute_type,
        int attribute_offset
        ) : attributeType( &attribute_type ), attributeName( attribute_name ), offset( attribute_offset )
    {
        itIsPointer = false;
        itIsArray = false;
    };

    virtual ~MetaAttribute()
    {

    };

    int GetOffset() const
    {
        return offset;
    }

    MetaAttribute & operator = ( const MetaAttribute & other )
    {
        attributeName = other.attributeName;
        attributeType = other.attributeType;
        offset = other.offset;
        itIsPointer = other.itIsPointer;
        itIsArray = other.itIsArray;

        return *this;
    }

    inline const std::string & GetName() const
    {
        return attributeName;
    }

    inline const MetaType & GetMetaType() const
    {
        return *attributeType;
    }

    inline void SetIsArray( const bool it_is_array )
    {
        itIsArray = it_is_array;
    }

    inline const bool IsArray() const
    {
        return itIsArray;
    }

    inline void SetIsPointer( const bool it_is_pointer )
    {
        itIsPointer = it_is_pointer;
    }

    inline const bool IsPointer() const
    {
        return itIsPointer;
    }

    inline void * GetAddress( void * object_address ) const
    {
        return ((char *)object_address) + offset;
    }

private:

    std::string attributeName;
    const MetaType * attributeType;
    int offset;
    bool itIsPointer;
    bool itIsArray;
};


#endif //__META_ATTRIBUTE__