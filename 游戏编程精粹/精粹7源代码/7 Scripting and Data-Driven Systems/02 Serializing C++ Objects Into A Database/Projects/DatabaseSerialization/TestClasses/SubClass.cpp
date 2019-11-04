#include "SubClass.h"

SubClass::SubClass()
{
    META_TYPE_Begin( SubClass )
        META_TYPE_SetParent( BaseClass )
        META_TYPE_AddAttribute( anotherInt )
    META_TYPE_End( SubClass )
}

SubClass::~SubClass()
{

}

