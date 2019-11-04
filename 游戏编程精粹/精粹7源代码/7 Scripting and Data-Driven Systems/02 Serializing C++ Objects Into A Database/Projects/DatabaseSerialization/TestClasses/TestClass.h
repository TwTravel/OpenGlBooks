#ifndef __TEST_CLASS__
#define __TEST_CLASS__

#include "..\Meta\MetaType.h"
#include "SubClass.h"
#include "..\Container\Array.h"
class BaseClass;

class TestClass : public MetaObject
{
public:

    TestClass();

    virtual ~TestClass();

    BaseClass baseClassInstance;
    BaseClass * baseClassPointer;
    Array<int> intTable;

    META_TYPE_Declare( TestClass );
};


#endif //__TEST_CLASS__