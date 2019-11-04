#include "MetaType.h"

IntMetaType::IntMetaType()
{
    SetInitialized( true );
    SetClassName( "int" );
}

IntMetaType::~IntMetaType()
{
}


StringMetaType::StringMetaType()
{
    SetInitialized( true );
    SetClassName( "std::string" );
}

StringMetaType::~StringMetaType()
{
}



IntMetaType IntMetaType::metaType;
StringMetaType StringMetaType::metaType;
std::vector<MetaType *> * MetaType::metaTypeTable = 0;