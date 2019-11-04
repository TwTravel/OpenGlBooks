#include "DatabaseManager.h"
#include "DatabaseConnection.h"
#include "DatabaseQueryResult.h"
#include "..\Meta\MetaType.h"
#include "..\Meta\MetaObject.h"
#include "..\Serialization\Serialization.h"
#include "..\Utility\Utility.h"
#include "..\Container\Array.h"

DatabaseManager::DatabaseManager()
{
    connection = 0;    
}

DatabaseManager::~DatabaseManager()
{
}

bool DatabaseManager::CreateTable( const MetaType & meta_type )
{
    unsigned int 
        attribute_index;
    std::string 
        sql_create_statement;

    if ( DoesTableExist( meta_type ) )
        return true;

    if ( meta_type.HasParentClass() )
    {
        const MetaType 
            & parent_class_meta_type = META_TYPE_GetByName( meta_type.GetParentClassName() );

        CreateTable( parent_class_meta_type );
    }

    sql_create_statement = "CREATE TABLE \"";
    sql_create_statement += meta_type.GetClassName();
    sql_create_statement += "\" ( \"_Identifier\" serial PRIMARY KEY ";

    for ( attribute_index = 0 ; attribute_index < meta_type.GetAttributeCount() ; ++attribute_index )
    {
        const MetaAttribute 
            & attribute = meta_type.GetAttributeAtIndex( attribute_index );
        const MetaType 
            & attribute_meta_type = attribute.GetMetaType();

        CreateTable( attribute_meta_type );

        sql_create_statement += ",\"";
        sql_create_statement += attribute.GetName();
        sql_create_statement += "\"";
        sql_create_statement += " \"";
        sql_create_statement += GetDatabaseTypeName( attribute_meta_type );
        sql_create_statement += "\"";
        
        if ( attribute.IsArray() )
            sql_create_statement += "[]";
    }

    if ( meta_type.HasParentClass() && meta_type.GetParentClassName() != "MetaObject" )
    {
        sql_create_statement += ", \"_Parent\" \"";
        sql_create_statement += meta_type.GetParentClassName();
        sql_create_statement += "\"";
    }

    sql_create_statement += ");";

    printf( "Creating table\n" );
    printf( sql_create_statement.data() );
    printf( "\n" );

    if ( !connection->ExecuteUpdate( sql_create_statement ) )
    {
        printf("Error creating table\n");
        return false;
    }

    return false;
}

bool DatabaseManager::DoesTableExist( const MetaType & meta_type )
{
    if ( IsNativeDatabaseType( meta_type ) )
        return true;

    return connection->DoesTableExist( meta_type.GetClassName() );
}


bool DatabaseManager::InsertObject(
    void * object,
    const MetaType & meta_type
    )
{
    // make sure we dont insert an object which was already inserted before
    if ( HasObjectInCache( object ) )
    {
        printf( "Object already exists\n" );

        return false;
    }

    connection->BeginTransaction();

    // 2 phase process, generate the primary keys for the objects first and insert "empty" objects
    InsertObjectAndAttributesPrimaryKeys( object, meta_type );

    // Next update those empty objects and fill in the correct member data
    UpdateObject( object, meta_type );

    return ( connection->EndTransaction() );
}

bool DatabaseManager::UpdateObject( 
    void * object, 
    const MetaType & meta_type 
    )
{
    if ( !HasObjectInCache( object ) )
    {
        printf( "Object does not exists\n" );

        return false;
    }

    connection->BeginTransaction();

    UpdateObjectAndAttributes( object, meta_type );

    return ( connection->EndTransaction() );
}

void DatabaseManager::GetObjects(
    Array<MetaObject *> & result_array,
    const MetaType & meta_type,
    const std::string & predicate
    )
{
    std::string
        select_statement;
    DatabaseQueryResult
        query_result;
    int 
        field_index,
        row_index;

    select_statement = "SELECT \"_Identifier\" FROM \"";
    select_statement += meta_type.GetClassName();

    if ( predicate.size() )
    {
        select_statement += "\" WHERE ";
        select_statement += predicate;
    }
    else
    {
        select_statement += "\"";
    }

    if ( !connection->ExecuteQuery( query_result, select_statement ) )
        return;

    field_index = query_result.GetFieldIndex( "_Identifier" );
    result_array.SetItemCount( query_result.GetRowCount() );

    for ( row_index = 0 ; row_index < query_result.GetRowCount() ; ++row_index )
    {
        unsigned int
            primary_key_value;

        primary_key_value = GetIntFromString(query_result.GetFieldValue( field_index, row_index ));
        
        if ( HasObjectInCache( primary_key_value, meta_type ) )
        {
            result_array[ row_index ] = (MetaObject *)GetObjectFromCache( primary_key_value, meta_type );
        }
        else
        {
            MetaObject
                * new_object;

            new_object = (MetaObject *)meta_type.CreateInstance();
            
            AddObjectToCache( new_object, primary_key_value, meta_type );
            result_array[ row_index ] = new_object;
            FillObjectFromDatabase( new_object, meta_type, primary_key_value );
        }
    }
}

void DatabaseManager::FillObjectFromDatabase( 
    void * new_object, 
    const MetaType & meta_type,
    const unsigned int primary_key_value
    )
{
    std::string
        select_statement;
    DatabaseQueryResult
        query_result;
    std::vector<std::string>
        attribute_name_table;
    unsigned int
        attribute_index;

    GenerateAttributeNameTable( attribute_name_table, meta_type, true );
    select_statement = "SELECT ";

    for ( attribute_index = 0 ; attribute_index < attribute_name_table.size(); ++attribute_index )
    {
        std::string
            & attribute_name = attribute_name_table[ attribute_index ];

        select_statement += "(";
        select_statement += attribute_name;
        select_statement += ")";

        if ( attribute_index < attribute_name_table.size() - 1 )
            select_statement += ",";
    }


    select_statement += " FROM \"";
    select_statement += meta_type.GetClassName();
    select_statement += "\" WHERE \"_Identifier\" = ";
    AppendIntToString( select_statement, primary_key_value );

    if ( !connection->ExecuteQuery( query_result, select_statement ) )
        return;

    FillObjectAttributes( new_object, meta_type, query_result );
}

void DatabaseManager::FillObjectAttributes( 
    void * new_object, 
    const MetaType & meta_type, 
    DatabaseQueryResult & query_result,
    unsigned int start_index
    )
{
    unsigned int
        attribute_index;

    // We have to pay attention here that we iterate over the query rows in the same order the attributes are stored in the object
    // and in the same order the select query returned them

    for( attribute_index = 0 ; attribute_index < meta_type.GetAttributeCount() ; ++attribute_index )
    {
        const MetaAttribute
            & meta_attribute = meta_type.GetAttributeAtIndex( attribute_index );
        const std::string
            & attribute_name = meta_attribute.GetName();
        void 
            * attribute_address = meta_attribute.GetAddress( new_object );
        const MetaType
            & attribute_meta_type = meta_attribute.GetMetaType();
        const std::string
            & field_value = query_result.GetFieldValue( attribute_index + start_index, 0 );

        if ( meta_attribute.IsArray() )
        {
            std::string 
                array_item_count_string;
            unsigned int
                item_count,
                item_index,
                field_value_index;
            ArrayBase
                & array_object = *(ArrayBase *)attribute_address;
            void
                * item_data;

            field_value_index = 1;
            array_item_count_string = "";
            
            while ( isdigit( field_value[ field_value_index ] ))
            {
                array_item_count_string += field_value[ field_value_index ];
                ++field_value_index;
            }

            item_count = GetIntFromString( array_item_count_string );

            array_object.SetItemCount( item_count );
            ++field_value_index;  // Skip the ','

            for ( item_index = 0 ; item_index < item_count ; ++item_index )
            {
                const MetaType
                    & item_meta_type = array_object.GetItemStaticMetaType();
                std::string
                    array_element_value;

                array_element_value = field_value[ field_value_index ];
                ++field_value_index;
                
                // If it is a complex attribute, fill the element value until we encounter the closing parenthesis

                if ( field_value[ field_value_index - 1 ] == '(' )
                {
                    do 
                    {
                        array_element_value += field_value[ field_value_index ];
                        ++field_value_index;
                    } while( field_value[ field_value_index - 1 ] != ')' );

                    ++field_value_index;
                }
                else
                {
                    while ( ( field_value[ field_value_index ] != ',' ) && ( field_value[ field_value_index ] != '}') )
                    {
                        array_element_value += field_value[ field_value_index ];
                        ++field_value_index;
                    }
                    ++field_value_index;
                }

                if ( array_object.ItemIsPointer() )
                {
                    item_data = (char *)array_object.GetData() + sizeof( int * ) * item_index;
                }
                else
                {
                    item_data = (char *)array_object.GetData() + array_object.GetItemStaticMetaType().GetByteCount() * item_index;
                }

                ReadObject( item_data, item_meta_type, array_element_value, array_object.ItemIsPointer() );
            }
        }
        else 
        {
            ReadObject( attribute_address, attribute_meta_type, field_value, meta_attribute.IsPointer() );
        }
    }

    if ( meta_type.HasParentClass() )
    {
        FillObjectAttributes( new_object, META_TYPE_GetByName( meta_type.GetParentClassName() ), query_result, meta_type.GetAttributeCount() );
    }
}


void DatabaseManager::InsertObjectAndAttributesPrimaryKeys( 
    void * attribute_address, 
    const MetaType & meta_type
    )
{
    unsigned int 
        primary_key;
    std::string 
        sql_string;

    if ( HasObjectInCache( attribute_address ) )
        return;

    // Dont bother inserting objects which dont require their own table
    if ( IsNativeDatabaseType( meta_type ) )
        return;

    // Iterate over all the attributes recursively to generate their primary keys if needed
    InsertAttributesPrimaryKeys( attribute_address, meta_type );

    if ( meta_type.GetAttributeCount() == 0 )
        return;


    // If we get at this point, we added all the complex subattributes of this attribute,
    // so we can add the attribute itself to the database
    
    if ( !DoesTableExist( meta_type ) )
    {
        // Create the table if the table doesnt exist for this type
        CreateTable( meta_type );
    }
    
    primary_key = GetNewPrimaryKeyValue( meta_type );

    sql_string = "INSERT INTO \"";
    sql_string += meta_type.GetClassName();
    sql_string += "\" (\"_Identifier\") VALUES (";
    AppendIntToString( sql_string, primary_key );
    sql_string += ");";

    connection->ExecuteUpdate( sql_string );
    AddObjectToCache( attribute_address, primary_key, meta_type );
}

void DatabaseManager::InsertAttributesPrimaryKeys( 
    void * attribute_address, 
    const MetaType & meta_type
    )
{
    unsigned int 
        attribute_index;
    std::string 
        sql_string;

    if ( HasObjectInCache( attribute_address ) )
        return;

    if ( IsNativeDatabaseType( meta_type ) )
        return;

    if ( meta_type.HasParentClass() )
    {
        const MetaType 
            & parent_class_meta_type = META_TYPE_GetByName( meta_type.GetParentClassName() );

        // Iterate over the parent attributes first
        InsertAttributesPrimaryKeys( attribute_address, parent_class_meta_type );
    }

    if ( meta_type.GetAttributeCount() == 0 )
        return;

    for ( attribute_index = 0 ; attribute_index < meta_type.GetAttributeCount() ; ++attribute_index )
    {
        const MetaAttribute 
            & attribute = meta_type.GetAttributeAtIndex( attribute_index );

        // For each attribute, choose what to do depending on its type
        if ( attribute.IsPointer() )
        {
            MetaObject
                * pointer_value;

            if ( !meta_type.HasParentClass( "MetaObject" ) )
            {
                printf("Pointers can only be serialized for objects inheriting from MetaObject\n");
                assert( false );
            }

            // Get the object pointed to and insert it in the database
            pointer_value = (MetaObject *)*((void **)attribute.GetAddress( attribute_address ));

            if ( pointer_value )
            {
                InsertObjectAndAttributesPrimaryKeys( pointer_value, pointer_value->GetMetaType() );
            }
        }
        else if ( attribute.IsArray() )
        {
            ArrayBase
                * array_pointer;
            void
                * data;
            const MetaType
                * array_item_meta_type;
            unsigned int
                item_index,
                item_count;

            array_pointer = ((ArrayBase *)attribute.GetAddress( attribute_address ));
            data = array_pointer->GetData();
            array_item_meta_type = &array_pointer->GetItemStaticMetaType();
            item_count = array_pointer->GetItemCount();

            // We dont iterate over every item if it is an int or a text, no insertion needed

            if ( !IsNativeDatabaseType( *array_item_meta_type ) )
            {
                for ( item_index = 0 ; item_index < item_count ; ++item_index )
                {
                    void
                        * item_address;

                    // If our array contains pointers, dereference them
                    if ( array_pointer->ItemIsPointer() )
                    {
                        MetaObject
                            * item_object;

                        if ( !array_item_meta_type->HasParentClass( "MetaObject" ) )
                        {
                            printf("Pointers can only be serialized for objects inheriting from MetaObject\n");
                            assert( false );
                        }

                        item_address = ( char *)data + item_index * sizeof( int *);

                        item_object = *(MetaObject **)item_address;

                        if ( item_object )
                        {
                            InsertObjectAndAttributesPrimaryKeys( item_object, item_object->GetMetaType() );
                        }
                    }
                    else
                    {
                        // Just process the stored object

                        item_address = ( char *)data + item_index * array_item_meta_type->GetByteCount();
                        InsertObjectAndAttributesPrimaryKeys( item_address, *array_item_meta_type );
                    }
                }
            }

        }
        else
        {
            // C++ object, just insert it
            InsertObjectAndAttributesPrimaryKeys( attribute.GetAddress( attribute_address ), attribute.GetMetaType() );
        }

    }
}

void DatabaseManager::UpdateAttributes( 
    void * object, 
    const MetaType & meta_type 
    )
{
    unsigned int 
        attribute_index;
    std::string
        sql_string;
    std::vector<std::string>
        attribute_name_table,
        attribute_value_table;

    if ( IsNativeDatabaseType( meta_type ) )
        return;

    if ( meta_type.HasParentClass() )
    {
        const MetaType 
            & parent_class_meta_type = META_TYPE_GetByName( meta_type.GetParentClassName() );

        UpdateAttributes( object, parent_class_meta_type );
    }

    for ( attribute_index = 0 ; attribute_index < meta_type.GetAttributeCount() ; ++attribute_index )
    {
        const MetaAttribute 
            & attribute = meta_type.GetAttributeAtIndex( attribute_index );

        if ( attribute.IsPointer() )
        {
            MetaObject
                * pointer_value;

            if ( !meta_type.HasParentClass( "MetaObject" ) )
            {
                printf("Pointers can only be serialized for objects inheriting from MetaObject\n");
                assert( false );
            }

            pointer_value = (MetaObject *)*((void **)attribute.GetAddress( object ) );

            if ( pointer_value )
            {
                UpdateObjectAndAttributes( pointer_value , pointer_value->GetMetaType() );
            }
        }
        else if ( attribute.IsArray() )
        {
            ArrayBase
                * array_pointer;
            void
                * data;
            const MetaType
                * array_item_meta_type;
            unsigned int
                item_index,
                item_count;

            array_pointer = ((ArrayBase *)attribute.GetAddress( object ));
            data = array_pointer->GetData();
            array_item_meta_type = &array_pointer->GetItemStaticMetaType();
            item_count = array_pointer->GetItemCount();

            // We dont iterate over every item if it is an int or a text

            if ( !IsNativeDatabaseType( *array_item_meta_type ) )
            {
                for ( item_index = 0 ; item_index < item_count ; ++item_index )
                {
                    void
                        * item_address;

                    // If our array contains pointers, dereference them
                    if ( array_pointer->ItemIsPointer() )
                    {
                        MetaObject
                            * item_object;

                        if ( !array_item_meta_type->HasParentClass( "MetaObject" ) )
                        {
                            printf("Pointers can only be serialized for objects inheriting from MetaObject\n");
                            assert( false );
                        }

                        item_address = ( char *)data + item_index * sizeof( int *);

                        item_object = *(MetaObject **)item_address;

                        if ( item_object )
                        {
                            UpdateObjectAndAttributes( item_object, attribute.GetMetaType() );
                        }
                    }
                    else
                    {
                        // Just process the stored object

                        item_address = ( char *)data + item_index * array_item_meta_type->GetByteCount();
                        UpdateObjectAndAttributes( item_address, attribute.GetMetaType() );
                    }
                }
            }

        }
        else
        {
            UpdateObjectAndAttributes( attribute.GetAddress( object ), attribute.GetMetaType() );
        }
    }
}

void DatabaseManager::UpdateObjectAndAttributes( 
    void * object, 
    const MetaType & meta_type 
    )
{
    unsigned int 
        field_index;
    std::string
        primary_key_text,
        sql_string;
    std::vector<std::string>
        attribute_name_table,
        attribute_value_table;

    if ( IsNativeDatabaseType( meta_type ) )
        return;

    if ( meta_type.HasParentClass() )
    {
        const MetaType & parent_class_meta_type = META_TYPE_GetByName( meta_type.GetParentClassName() );

        UpdateAttributes( object, parent_class_meta_type );
    }

    UpdateAttributes( object, meta_type );

    sql_string = "UPDATE \"";
    sql_string += meta_type.GetClassName();
    sql_string += "\" SET ";
    GenerateAttributeNameTable( attribute_name_table, meta_type, false );
    GenerateAttributeValueTable( object, attribute_value_table, meta_type );
    
    for( field_index = 0 ; field_index < attribute_name_table.size() ; ++field_index )
    {
        std::string
            field_name,
            escaped_field_value,
            field_value;

        field_name = attribute_name_table[ field_index ];
        field_value = attribute_value_table[ field_index ];
        sql_string += field_name;
        sql_string += " = ";

        if ( IsEmptyValue ( field_value ) )
        {
            sql_string += "NULL";
        }
        else
        {
            if ( IsArrayValue ( field_value ) )
            {
                sql_string += "\'";
                sql_string += field_value;
                sql_string += "\'";
            }
            else
            {
                sql_string += field_value;
            }
        }

        if ( field_index < attribute_value_table.size() - 1 )
        {
            sql_string += ", ";
        }
    }

    primary_key_text = " WHERE \"_Identifier\" = ";
    AppendIntToString( primary_key_text, GetObjectPrimaryKeyValue( object ) );
    sql_string += primary_key_text;
    
    connection->ExecuteUpdate( sql_string );
}

const std::string DatabaseManager::GetDatabaseTypeName( const MetaType & meta_type ) const
{
    static std::string
        int_type( "int4" ),
        string_type ( "varchar" );

    if ( meta_type.GetClassName() == "int" )
    {
        return int_type;
    }

    return string_type;
}

bool DatabaseManager::HasObjectInCache( void * object ) const
{
    return ( objectCache.find( object ) != objectCache.end() );
}

bool DatabaseManager::HasObjectInCache( 
    const unsigned int primary_key_value,
    const MetaType & meta_type
    ) const
{
    std::map< void *, int >::const_iterator
        begin;

    begin = objectCache.begin();

    while( begin != objectCache.end() )
    {
        if ( (*begin).second == primary_key_value )
        {
            void
                * item = (*begin).first;

            if ( objectMetaTypeTableCache.find( item )->second == &meta_type )
                return true;
        }

        ++begin;
    }

    return false;
}

void * DatabaseManager::GetObjectFromCache( 
    const unsigned int primary_key_value,
    const MetaType & meta_type
    ) const
{
    std::map< void *, int >::const_iterator
        begin;

    begin = objectCache.begin();

    while( begin != objectCache.end() )
    {
        if ( (*begin).second == primary_key_value )
        {
            void 
                * item = (*begin).first;

            if ( objectMetaTypeTableCache.find( item )->second == &meta_type )
                return (*begin).first;
        }

        ++begin;
    }

    return 0;
}

void DatabaseManager::AddObjectToCache( 
    void * object, 
    const unsigned int primary_key_value,
    const MetaType & meta_type
    )
{
    assert (!HasObjectInCache( object ) );

    objectCache[ object ] = primary_key_value;
    objectMetaTypeTableCache[ object ] = &meta_type;
}

unsigned int DatabaseManager::GetObjectPrimaryKeyValue(
    void * object
    ) const
{
    std::map< void *, int >::const_iterator
        position;

    position = objectCache.find( object );

    return (*position).second;
}

unsigned int DatabaseManager::GetNewPrimaryKeyValue( const MetaType & meta_type )
{
    return connection->GetNewPrimaryKeyValue( meta_type.GetClassName(), "_Identifier" );
}

void DatabaseManager::GenerateAttributeNameTable( 
    std::vector<std::string> & attribute_name_table, 
    const MetaType & meta_type, 
    const bool add_parent_parenthesis,
    const unsigned int parent_level
    ) const
{
    std::string
        parent_attribute_table,
        attribute_table;
    unsigned int
        attribute_index;

    // here there is a lot of fun going on with parenthesis and quotes
    // in order to make postgresql correctly understand the table names and field names with correct case
    for( attribute_index = 0 ; attribute_index < meta_type.GetAttributeCount() ; ++attribute_index )
    {
        std::string
            attribute,
            new_attribute;
        unsigned int
            recursion_index;

        attribute = "\"";
        attribute += meta_type.GetAttributeAtIndex( attribute_index ).GetName();
        attribute += "\"";

        for( recursion_index = 0 ;  recursion_index < parent_level ; ++recursion_index )
        {
            if ( recursion_index == parent_level - 1 )
            {
                if ( add_parent_parenthesis )
                {
                    new_attribute = "(\"";
                }
                else
                {
                    new_attribute = "\"";
                }
                
                new_attribute += "_Parent";

                if ( add_parent_parenthesis )
                {
                    new_attribute += "\").";
                }
                else
                {
                    new_attribute += "\".";
                }
                
                new_attribute += attribute;
                
            }
            else
            {
                new_attribute = "(\"";
                new_attribute += "_Parent";
                new_attribute += "\".";
                new_attribute += attribute;
            }
            
            attribute = new_attribute;
        }

        attribute_name_table.push_back( attribute );
    }

    if ( meta_type.HasParentClass() )
    {
        GenerateAttributeNameTable( attribute_name_table, META_TYPE_GetByName( meta_type.GetParentClassName() ), add_parent_parenthesis, parent_level + 1 );
    }
}

void DatabaseManager::GenerateAttributeValueTable( 
    void * object,
    std::vector<std::string> & attribute_value_table, 
    const MetaType & meta_type
    ) const
{
    unsigned int
        attribute_index;

    for( attribute_index = 0 ; attribute_index < meta_type.GetAttributeCount() ; ++attribute_index )
    {
        std::string
            attribute_value;
        const MetaAttribute
            & attribute = meta_type.GetAttributeAtIndex( attribute_index );

        GenerateAttributeValue( attribute.GetAddress( object ), attribute, attribute_value );
        attribute_value_table.push_back( attribute_value );
    }

    if ( meta_type.HasParentClass() )
    {
        GenerateAttributeValueTable( object, attribute_value_table, META_TYPE_GetByName( meta_type.GetParentClassName() ) );
    }
}

void DatabaseManager::GenerateAttributeValue( 
    void * object, 
    const MetaAttribute & meta_attribute, 
    std::string & attribute_value
    ) const
{
    const MetaType 
        & meta_type = meta_attribute.GetMetaType();

    if ( meta_attribute.IsPointer() )
    {
        MetaObject
            * pointer_value;
        const MetaType
            * pointer_value_meta_type;

        if ( !meta_type.HasParentClass( "MetaObject" ) )
        {
            printf("Pointers can only be serialized for objects inheriting from MetaObject\n");
            assert( false );
        }

        pointer_value = (MetaObject *)*((void **)object);

        if ( pointer_value )
        {
            // Construct the ( identifier, classname ) textstring
            pointer_value_meta_type = &pointer_value->GetMetaType();
            attribute_value = "'(";
            AppendIntToString( attribute_value, GetObjectPrimaryKeyValue( pointer_value ) );
            attribute_value += ",";
            attribute_value += pointer_value_meta_type->GetClassName();
            attribute_value += ")'";
        }
        else
        {
            attribute_value = "(0,0)";
        }
    }
    else if ( meta_attribute.IsArray() )
    {
        ArrayBase
            * array_pointer;
        void
            * data;
        const MetaType
            * array_item_meta_type;
        unsigned int
            item_index,
            item_count;

        array_pointer = (ArrayBase *)object;
        data = array_pointer->GetData();
        array_item_meta_type = &array_pointer->GetItemStaticMetaType();
        item_count = array_pointer->GetItemCount();

        if ( item_count == 0 )
        {
            attribute_value = "{0}";
        }

        // Construct an array text { ..,..,...}
        attribute_value = "{";

        AppendIntToString( attribute_value, item_count );
        attribute_value+=",";

        if ( IsNativeDatabaseType( *array_item_meta_type ) )
        {

            for ( item_index = 0 ; item_index < item_count ; ++item_index )
            {
                void
                    * item_address;
                std::string
                    item_string;

                item_address = ( char *)data + item_index * array_item_meta_type->GetByteCount();
                WriteObjectToText( item_address, *array_item_meta_type, item_string );
                attribute_value += item_string;
                
                if ( item_index < item_count - 1 )
                    attribute_value += ",";
            }

        }
        else
        {
            for ( item_index = 0 ; item_index < item_count ; ++item_index )
            {
                void
                    * item_address;

                // If our array contains pointers, dereference them
                if ( array_pointer->ItemIsPointer() )
                {
                    MetaObject
                        * item_object;

                    if ( !array_item_meta_type->HasParentClass( "MetaObject" ) )
                    {
                        printf("Pointers can only be serialized for objects inheriting from MetaObject\n");
                        assert( false );
                    }

                    item_address = ( char *)data + item_index * sizeof( int *);

                    item_object = *(MetaObject **)item_address;

                    if ( item_object )
                    {
                        attribute_value += "(";
                        AppendIntToString( attribute_value, GetObjectPrimaryKeyValue( item_object ) );
                        attribute_value += ",";
                        attribute_value += item_object->GetMetaType().GetClassName();
                        attribute_value += ")";
                    }
                    else
                    {
                        attribute_value += "(0,0)";
                    }
                }
                else
                {
                    // Just process the stored object

                    item_address = ( char *)data + item_index * array_item_meta_type->GetByteCount();
                    
                    attribute_value += "(";
                    AppendIntToString( attribute_value, GetObjectPrimaryKeyValue( item_address ) );
                    attribute_value += ",";
                    attribute_value += meta_type.GetClassName();
                    attribute_value += ")";
                }

                if ( item_index < item_count - 1 )
                    attribute_value += ",";
            }
        }

        attribute_value += "}";
    }
    else if ( IsNativeDatabaseType( meta_type ) )
    {
        WriteObjectToText( object, meta_type, attribute_value );
    }
    else
    {
        attribute_value = "'(";
        AppendIntToString( attribute_value, GetObjectPrimaryKeyValue( object ) );
        attribute_value += ",";
        attribute_value += meta_type.GetClassName();
        attribute_value += ")'";
    }
}

bool DatabaseManager::IsNativeDatabaseType( const MetaType & meta_type ) const
{
    return ( META_TYPE_IsScalar( meta_type ) || ( &META_TYPE_GetByName( "std::string" ) == &meta_type ) );
}

// ~~

bool DatabaseManager::IsEmptyValue( const std::string & value ) const
{
    unsigned int
        start_index;

    start_index = 0;

    if ( value.size() == 0 )
    {
        return true;
    }

    while ( value[ start_index ] == ' ' )
    {
        ++start_index;
    }

    if ( value[ start_index ] != '{' )
    {
        return false;
    }

    ++start_index;

    while ( value[ start_index ] != '}' )
    {
        if ( value[ start_index ] != ' ' )
        {
            return false;
        }

        ++start_index;
    }

    return true;
}

// ~~

bool DatabaseManager::IsArrayValue( const std::string & value ) const
{
    unsigned int
        start_index;

    start_index = 0;

    if ( value.size() == 0 )
    {
        return false;
    }

    while ( value[ start_index ] == ' ' )
    {
        ++start_index;
    }

    if ( value[ start_index ] == '{' )
    {
        return true;
    }

    return false;
}

void DatabaseManager::ReadObject( 
    void * object, 
    const MetaType & meta_type, 
    const std::string & value, 
    const bool it_is_pointer 
    )
{
    if ( IsNativeDatabaseType( meta_type ) )
    {
        ReadObjectFromText( object, meta_type, value );
    }
    else
    {
        // Split up in primary key and classname
        std::string
            primary_key_text,
            classname_text;
        unsigned int
            primary_key_value,
            index;
        const MetaType
            * meta_type;

        index = 1;
        while ( value[ index ] != ',' )
        {
            primary_key_text += value[ index ];
            ++index;
        }
        ++index;
        while ( value[ index ] != ')' )
        {
            classname_text += value[ index ];
            ++index;
        }

        primary_key_value = GetIntFromString( primary_key_text );
        meta_type = &META_TYPE_GetByName( classname_text );

        if ( it_is_pointer )
        {
            // See if we have the object in the cache
            if ( HasObjectInCache( primary_key_value, *meta_type ) )
            {
                *( void **)object = GetObjectFromCache( primary_key_value, *meta_type );
            }
            else
            {
                // Create the object
                MetaObject
                    * new_object;

                new_object = ( MetaObject *)meta_type->CreateInstance();
                *( void **)object = (void *)new_object;
                AddObjectToCache( new_object, primary_key_value, *meta_type );
                FillObjectFromDatabase( new_object, *meta_type, primary_key_value );
            }
        }
        else
        {
            AddObjectToCache( object, primary_key_value, *meta_type );
            FillObjectFromDatabase( object, *meta_type, primary_key_value );
        }
    }
    
}
