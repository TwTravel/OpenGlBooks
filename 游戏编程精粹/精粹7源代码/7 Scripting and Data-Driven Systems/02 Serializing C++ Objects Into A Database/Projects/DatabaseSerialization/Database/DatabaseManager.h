#ifndef __DATABASE_MANAGER__
#define __DATABASE_MANAGER__

#include <string>
#include <vector>
#include <map>
#include "..\Container\Array.h"
class DatabaseConnection;
class MetaType;
class MetaObject;
class MetaAttribute;
class DatabaseQueryResult;

class DatabaseManager
{
public:

    DatabaseManager();

    virtual ~DatabaseManager();

    void SetConnection( DatabaseConnection * database_connection )
    {
        connection = database_connection;
    }

    bool CreateTable( const MetaType & meta_type );

    bool DoesTableExist( const MetaType & meta_type );

    bool InsertObject( 
        void * object, 
        const MetaType & meta_type 
        );

    bool UpdateObject( 
        void * object, 
        const MetaType & meta_type 
        );

    void GetObjects(
        Array<MetaObject *> & result_array,
        const MetaType & meta_type,
        const std::string & predicate
        );

private:

    const std::string GetDatabaseTypeName( const MetaType & meta_type ) const;

    bool HasObjectInCache( void * object ) const;

    bool HasObjectInCache( 
        const unsigned int primary_key_value,
        const MetaType & meta_type
        ) const;

    void * GetObjectFromCache( 
        const unsigned int primary_key_value,
        const MetaType & meta_type
        ) const;

    void AddObjectToCache( 
        void * object, 
        const unsigned int primary_key_value,
        const MetaType & meta_type
        );

    unsigned int GetObjectPrimaryKeyValue(
        void * object
        ) const;

    void FillObjectFromDatabase( 
        void * new_object, 
        const MetaType & meta_type,
        const unsigned int primary_key_value
        );


    void InsertObjectAndAttributesPrimaryKeys( 
        void * attribute_address, 
        const MetaType & meta_type
        );

    void InsertAttributesPrimaryKeys( 
        void * attribute_address, 
        const MetaType & meta_type
        );

    void UpdateObjectAndAttributes( 
        void * object, 
        const MetaType & meta_type 
        );

    void UpdateAttributes( 
        void * object, 
        const MetaType & meta_type 
        );

    void FillObjectAttributes( 
        void * new_object, 
        const MetaType & meta_type, 
        DatabaseQueryResult & query_result,
        unsigned int start_index = 0
        );

    unsigned int GetNewPrimaryKeyValue( const MetaType & meta_type );
    
    bool IsNativeDatabaseType( const MetaType & meta_type ) const;

    void GenerateAttributeNameTable( 
        std::vector<std::string> & attribute_name_table, 
        const MetaType & meta_type,
        const bool add_parent_parenthesis,
        const unsigned int parent_level = 0 
        ) const;

    void GenerateAttributeValueTable( 
        void * object,
        std::vector<std::string> & attribute_value_table, 
        const MetaType & meta_type
        ) const;

    void GenerateAttributeValue( 
        void * object, 
        const MetaAttribute & meta_attribute, 
        std::string & attribute_value
        ) const;

    bool IsEmptyValue( const std::string & value ) const;

    bool IsArrayValue( const std::string & value ) const;

    void ReadObject( 
        void * object, 
        const MetaType & meta_type, 
        const std::string & value, 
        const bool it_is_pointer 
        );

    DatabaseConnection * connection;
    std::map< void *, int > objectCache;
    std::map< void *, const MetaType * > objectMetaTypeTableCache;

};


#endif //__DATABASE_MANAGER__