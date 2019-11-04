#ifndef __ARRAY__
#define __ARRAY__

#include "..\Meta\MetaType.h"

// Baseclass of all arrays used
class ArrayBase
{
public:
    ArrayBase()
    {
        META_TYPE_Begin( ArrayBase )
            META_TYPE_AddAttribute( ItemCount )
        META_TYPE_End( ArrayBase )
    };

    virtual ~ArrayBase()
    {
    }

    inline int GetItemCount()
    {
        return ItemCount;
    }

    virtual const MetaType & GetItemStaticMetaType()
    {
        assert( false );
        
        return *(MetaType *)0;
    }

    virtual bool ItemIsPointer()
    {
        assert ( false );
        return false;
    }

    virtual void * GetData()
    {
        assert( false );
        return 0;
    }

    virtual void SetItemCount( const unsigned int item_count )
    {
        assert( false );
    }

protected:

    int
        ItemCount;

    META_TYPE_Declare( ArrayBase );
};

template < typename _ITEM_ >
    class Array : public ArrayBase
{
public:

    Array()
    {
        META_TYPE_Begin( Array<_ITEM_> )
        META_TYPE_End( Array<_ITEM_> )

        Table = 0;
        SetItemCount( 0 );
    };

    virtual ~Array()
    {
        delete [] Table;
    }

    _ITEM_ & operator [] ( const unsigned int item_index )
    {
        assert( item_index < (unsigned int)ItemCount );
        assert( item_index >= 0 );

        return Table[ item_index ];
    }

    virtual void SetItemCount( const unsigned int item_count )
    {
        delete [] Table;
        Table = new _ITEM_[ item_count ];
        ItemCount = item_count;
    }

    virtual const MetaType & GetItemStaticMetaType()
    {
        return META_TYPE_GetStaticMetaType( Table[0] );
    }

    virtual void * GetData()
    {
        return Table;
    }

    virtual bool ItemIsPointer()
    {
        return META_TYPE_IsPointer( *Table );
    }


private:

    _ITEM_
        * Table;

    META_TYPE_Declare( Array<_ITEM_> );
};

#endif