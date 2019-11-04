#ifndef __SUB_CLASS__
#define __SUB_CLASS__

#include "..\Meta\MetaType.h"
#include "BaseClass.h"

class SubClass : public BaseClass
{
public:

    SubClass();

    virtual ~SubClass();

    int anotherInt;

    META_TYPE_Declare( SubClass );
};


#endif //__META_TYPE__