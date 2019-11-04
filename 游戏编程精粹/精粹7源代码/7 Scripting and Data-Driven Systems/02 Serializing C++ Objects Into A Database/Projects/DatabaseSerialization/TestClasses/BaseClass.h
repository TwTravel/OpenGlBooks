#ifndef __BASE_CLASS__
#define __BASE_CLASS__

#include "..\Meta\MetaType.h"
#include "..\Meta\MetaObject.h"

class BaseClass : public MetaObject
{
public:

    BaseClass();

    virtual ~BaseClass();

    std::string aText;
    int anInt;

    META_TYPE_Declare( BaseClass );
};


#endif //__BASE_CLASS__