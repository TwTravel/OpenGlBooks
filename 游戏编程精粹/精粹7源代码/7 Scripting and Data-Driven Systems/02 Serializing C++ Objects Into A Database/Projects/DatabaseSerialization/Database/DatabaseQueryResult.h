#ifndef __DATABASE_QUERY_RESULT__
#define __DATABASE_QUERY_RESULT__

#include <string>
#include <vector>

class DatabaseQueryResult
{
public:

    DatabaseQueryResult();

    virtual ~DatabaseQueryResult();

    void SetRowCount( const int row_count );

    int GetRowCount() const;

    void SetFieldCount( const int field_count );

    int GetFieldCount() const;

    void SetFieldNameAtIndex( const std::string & field_name, const int field_index );

    const std::string & GetFieldNameAtIndex( const int field_index ) const;

    int GetFieldIndex( const std::string & field_name ) const;

    int GetFieldIndex( const std::string & field_name, const int start_search_index ) const;

    void SetFieldValue( const std::string & value, const int field_index, const int row_index );

    const std::string & GetFieldValue( const int field_index, const int row_index ) const;

    bool HasFieldValue( const int field_index, const int row_index ) const
    {
        return ( GetFieldValue( field_index, row_index ).size() > 0 );
    }

    void Log() const;

    void SetEmpty()
    {
        fieldNameTable.empty();
        fieldValueTable.empty();
        rowCount = 0;
    }

private:
    typedef std::vector<std::string> string_vector;

    string_vector
        fieldNameTable;
    std::vector<string_vector>
        fieldValueTable;
    int            
        rowCount;
};

#endif //__DATABASE_QUERY_RESULT__