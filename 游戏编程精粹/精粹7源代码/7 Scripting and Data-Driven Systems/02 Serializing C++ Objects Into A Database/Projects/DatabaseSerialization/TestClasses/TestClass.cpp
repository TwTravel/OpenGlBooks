#include "TestClass.h"

TestClass::TestClass()
{
    META_TYPE_Begin( TestClass )
        META_TYPE_SetParent( MetaObject )
        META_TYPE_AddAttribute( baseClassInstance )
        META_TYPE_AddPointerAttribute( baseClassPointer )
        META_TYPE_AddArrayAttribute( intTable )
    META_TYPE_End( TestClass )
}

TestClass::~TestClass()
{

}

