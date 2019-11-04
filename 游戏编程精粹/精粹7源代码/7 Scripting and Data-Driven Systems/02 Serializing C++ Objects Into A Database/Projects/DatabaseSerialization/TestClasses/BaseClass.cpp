#include "BaseClass.h"

BaseClass::BaseClass()
{
    META_TYPE_Begin( BaseClass )
        META_TYPE_SetParent( MetaObject )
        META_TYPE_AddAttribute( aText )
        META_TYPE_AddAttribute( anInt )
    META_TYPE_End( BaseClass )
}

BaseClass::~BaseClass()
{

}
