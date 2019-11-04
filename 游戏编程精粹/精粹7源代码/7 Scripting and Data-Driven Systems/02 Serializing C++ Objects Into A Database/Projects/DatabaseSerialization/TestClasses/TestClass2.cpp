#include "TestClass2.h"

TestClass2::TestClass2()
{
    META_TYPE_Begin( TestClass2 )
        META_TYPE_SetParent( MetaObject )
        META_TYPE_AddArrayAttribute( baseClassTable )
        META_TYPE_AddArrayAttribute( PolymorphicBaseClassTable )
    META_TYPE_End( TestClass2 )
}

TestClass2::~TestClass2()
{

}

