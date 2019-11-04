#ifndef __TEST_CLASS_2__
#define __TEST_CLASS_2__

#include "..\Meta\MetaType.h"
#include "SubClass.h"
#include "BaseClass.h"
#include "..\Container\Array.h"
class BaseClass;

class TestClass2 : public MetaObject
{
public:

    TestClass2();

    virtual ~TestClass2();

    Array<BaseClass> baseClassTable;
    Array<BaseClass *> PolymorphicBaseClassTable;

    META_TYPE_Declare( TestClass2 );
};


#endif //__TEST_CLASS__