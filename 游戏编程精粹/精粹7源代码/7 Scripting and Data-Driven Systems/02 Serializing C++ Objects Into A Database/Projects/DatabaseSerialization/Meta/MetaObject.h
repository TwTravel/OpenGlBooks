#ifndef __META_OBJECT__
#define __META_OBJECT__

#include "MetaType.h"

class MetaObject
{
public:

    MetaObject();

    virtual ~MetaObject();

    META_TYPE_Declare( MetaObject );
};

#endif // __META_OBJECT__
