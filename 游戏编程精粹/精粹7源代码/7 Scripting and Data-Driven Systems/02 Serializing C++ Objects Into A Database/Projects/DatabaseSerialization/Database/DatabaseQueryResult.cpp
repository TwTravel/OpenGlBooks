#include "DatabaseQueryResult.h"

DatabaseQueryResult::DatabaseQueryResult()
{
    rowCount = 0;
}

DatabaseQueryResult::~DatabaseQueryResult()
{
}

void DatabaseQueryResult::SetRowCount( const int row_count )
{
    unsigned int
        value_index;

    rowCount = row_count;

    for( value_index = 0 ; value_index < fieldValueTable.size() ; ++value_index )
    {
        fieldValueTable[ value_index ].resize( row_count );
    }

}

int DatabaseQueryResult::GetRowCount() const
{
    return rowCount;
}

void DatabaseQueryResult::SetFieldCount( const int field_count )
{
    fieldNameTable.resize( field_count );
    fieldValueTable.resize( field_count );
    SetRowCount( rowCount );
}

int DatabaseQueryResult::GetFieldCount() const
{
    return fieldValueTable.size();
}

void DatabaseQueryResult::SetFieldNameAtIndex( const std::string & field_name, const int field_index )
{
    fieldNameTable[ field_index ] = field_name;
}

const std::string & DatabaseQueryResult::GetFieldNameAtIndex( const int field_index ) const
{
    return fieldNameTable[ field_index ];
}

int DatabaseQueryResult::GetFieldIndex( const std::string & field_name ) const
{
    unsigned int
        field_index;

    for( field_index = 0 ; field_index < fieldNameTable.size() ; ++field_index )
    {
        if ( fieldNameTable[ field_index ] == field_name )
        {
            return field_index;
        }
    }

    return -1;
}

int DatabaseQueryResult::GetFieldIndex( const std::string & field_name, const int start_search_index ) const
{
    unsigned int
        field_index;

    for( field_index = start_search_index; field_index < fieldNameTable.size(); ++field_index )
    {
        if ( fieldNameTable[ field_index ] == field_name )
        {
            return field_index;
        }
    }

    return -1;
}

void DatabaseQueryResult::SetFieldValue( const std::string & value, const int field_index, const int row_index )
{
    fieldValueTable[ field_index ][ row_index ] = value;
}

const std::string & DatabaseQueryResult::GetFieldValue( const int field_index, const int row_index ) const
{
    return fieldValueTable[ field_index ][ row_index ];
}
