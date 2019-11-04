#ifndef __META_TYPE__
#define __META_TYPE__

#include "MetaAttribute.h"
#include <vector>
#include <assert.h>

/*
    The class containing the metainformation of another class, created using the macros show at the bottom of the file
*/

class MetaType
{
public:

    MetaType()
    {
        if ( metaTypeTable == 0 )
        {
            metaTypeTable = new std::vector<MetaType *>;
        }
        itIsInitialized = false;
        metaTypeTable->push_back( this );
    };

    virtual ~MetaType()
    {

    };

    inline void SetClassName( const char * class_name )
    {
        className = class_name;
    }

    inline const std::string & GetClassName() const
    {
        return className;
    }

    inline const std::string & GetParentClassName() const
    {
        return parentClassName;
    }

    inline bool HasParentClass() const
    {
        return parentClassName.size() != 0;
    }

    inline bool HasParentClass( const std::string & parent_class_name ) const
    {
        if ( parent_class_name == parentClassName )
            return true;

        if ( HasParentClass() )
        {
            return GetMetaTypeByName( parentClassName ).HasParentClass( parent_class_name );
        }
        
        return false;
    }

    inline void SetParentClassName( const char * parent_class_name )
    {
        parentClassName = parent_class_name;
    }

    inline bool IsInitialized() const
    {
        return itIsInitialized;
    }

    inline void SetInitialized( const bool it_is_initialized )
    {
        itIsInitialized = it_is_initialized;
    }

    inline void AddAttribute( const MetaAttribute & attribute )
    {
        attributeTable.push_back( attribute );
    }

    inline unsigned int GetAttributeCount() const
    {
        return (unsigned int)attributeTable.size();
    }

    inline const MetaAttribute & GetAttributeAtIndex( unsigned int index ) const
    {
        return attributeTable[ index ];
    }

    virtual void * CreateInstance() const = 0;

    virtual unsigned int GetByteCount() const = 0;

    virtual bool IsScalar() const
    {
        return false;
    }


    static MetaType & GetMetaTypeByName( const std::string & name )
    {
        unsigned int type_index;

        for ( type_index = 0 ; type_index < metaTypeTable->size() ; ++type_index )
        {
            if ( (*metaTypeTable)[ type_index ]->GetClassName() == name )
                return *(*metaTypeTable)[ type_index ];
        }

        assert( false );

        return *(MetaType *)0;
    }
private:

    std::string className;
    std::string parentClassName;
    std::vector<MetaAttribute> attributeTable;
    bool itIsInitialized;

    static std::vector<MetaType *> * metaTypeTable;
};

template <typename _TYPE_> class MetaTypeTemplate : public MetaType
{
    public:

        virtual void * CreateInstance() const
        {
            return new _TYPE_;
        }

        virtual unsigned int GetByteCount() const
        {
            return sizeof (_TYPE_);
        }
};

/*
    For some primitive types we create specific subclasses with specific behaviors
    This could be done for all scalar/primitive types in a more general way by using macros
*/

class IntMetaType : public MetaTypeTemplate<int>
{
public:
    
    IntMetaType();
    
    virtual ~IntMetaType();

    virtual bool IsScalar() const
    {
        return true;
    }

    static MetaType & GetStaticMetaType() { return metaType; };

    static IntMetaType metaType;
};

// ~~

class StringMetaType : public MetaTypeTemplate<std::string>
{
public:

    StringMetaType();

    virtual ~StringMetaType();

    static MetaType & GetStaticMetaType() { return metaType; };

    static StringMetaType metaType;
};


/*
    Metatype functions
*/

template < typename _TYPE_ > const MetaType & META_TYPE_GetStaticMetaType( _TYPE_ & object )
{
    return _TYPE_::GetStaticMetaType();
}

template < typename _TYPE_ > const MetaType & META_TYPE_GetStaticMetaType( _TYPE_ * object )
{
    return _TYPE_::GetStaticMetaType();
}

template <> inline const MetaType & META_TYPE_GetStaticMetaType( int & object )
{
    return IntMetaType::GetStaticMetaType();
}

template <> inline const MetaType & META_TYPE_GetStaticMetaType( std::string & object )
{
    return StringMetaType::GetStaticMetaType();
}

template <> inline const MetaType & META_TYPE_GetStaticMetaType( int * object )
{
    return IntMetaType::GetStaticMetaType();
}

template <> inline const MetaType & META_TYPE_GetStaticMetaType( std::string * object )
{
    return StringMetaType::GetStaticMetaType();
}

inline const MetaType & META_TYPE_GetByName( const std::string & name )
{
    return MetaType::GetMetaTypeByName( name );
}

inline bool META_TYPE_IsScalar( const MetaType & meta_type )
{
    return &meta_type == &IntMetaType::GetStaticMetaType();
}

/*
    To be used when declaring classes
*/

#define META_TYPE_Declare( _TYPE_ ) \
    public: \
        virtual MetaType & GetMetaType() const \
        { \
            return _TYPE_::GetStaticMetaType() ; \
        } \
        \
        static MetaType & GetStaticMetaType() \
        {  \
            static MetaTypeTemplate<_TYPE_> metaType; \
            return metaType;  \
        };

#define META_TYPE_Begin( _TYPE_ ) \
    if ( !GetMetaType().IsInitialized() ) \
    { \
        GetMetaType().SetClassName( #_TYPE_ );


#define META_TYPE_SetParent( _PARENT_TYPE_NAME_ ) \
    { \
        GetMetaType().SetParentClassName( #_PARENT_TYPE_NAME_ ); \
    }
#define META_TYPE_AddAttribute( _NAME_ ) \
    { \
        MetaAttribute attribute( #_NAME_, META_TYPE_GetStaticMetaType( _NAME_ ),  (int)(((__int64)&_NAME_) - ((__int64)this)) ); \
        GetMetaType().AddAttribute( attribute ); \
    }

#define META_TYPE_AddArrayAttribute( _NAME_ ) \
    { \
        MetaAttribute attribute( #_NAME_, _NAME_.GetItemStaticMetaType(),  (int)(((__int64)&_NAME_) - ((__int64)this)) ); \
        attribute.SetIsArray( true ); \
        GetMetaType().AddAttribute( attribute ); \
    }

#define META_TYPE_AddPointerAttribute( _NAME_ ) \
    { \
        MetaAttribute attribute( #_NAME_, META_TYPE_GetStaticMetaType(_NAME_),  (int)(((__int64)&_NAME_) - ((__int64)this)) ); \
        attribute.SetIsPointer( true ); \
        GetMetaType().AddAttribute( attribute ); \
    }
#define META_TYPE_End( _TYPE_ )  \
        GetMetaType().SetInitialized( true ); \
    }

template <typename _TYPE_> inline bool META_TYPE_IsPointer( _TYPE_ * type )
{
    return true;
}

template <typename _TYPE_> inline bool META_TYPE_IsPointer( _TYPE_ & type )
{
    return false;
}

#endif //__META_TYPE__