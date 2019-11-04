#include "DatabaseConnection.h"
#include "DatabaseQueryResult.h"
#include <assert.h>

DatabaseConnection::DatabaseConnection()
{
    postgresConnection = NULL;
	transactionCounter = 0;
}

DatabaseConnection::~DatabaseConnection()
{
}

bool DatabaseConnection::OpenConnection(
    const std::string & host_address,
    const std::string & database_name,
    const std::string & username,
    const std::string & password
    )
{
    std::string
        connection_string;

    assert( postgresConnection == NULL );
    connection_string += "hostaddr = ";
    connection_string += host_address;
    connection_string += " dbname = ";
    connection_string += database_name;
    connection_string += " user = ";
    connection_string += username;
    connection_string += " password = ";
    connection_string += password;

    postgresConnection = PQconnectdb( connection_string.data() );

    if ( PQstatus( postgresConnection ) == CONNECTION_OK )
        return true;

    return false;
}

bool DatabaseConnection::CloseConnection()
{
    PQfinish( postgresConnection );
    postgresConnection = 0;

    return true;
}

bool DatabaseConnection::CheckConnection()
{
    if ( PQstatus( postgresConnection )  != CONNECTION_OK )
    {
        return false;
    }

    return true;
}

bool DatabaseConnection::DoesTableExist( const std::string & table_name )
{
    std::string
        table_list_query;
    DatabaseQueryResult
        query_result;

    table_list_query = "SELECT tablename FROM pg_tables WHERE schemaname = \'public\' AND tablename = \'";
    table_list_query += table_name;
    table_list_query += "\';";
    ExecuteQuery( query_result, table_list_query );

    return ( query_result.GetRowCount() != 0 );
}

bool DatabaseConnection::ExecuteQuery( 
    DatabaseQueryResult & query_result, 
    const std::string & query_text )
{
    PGresult
        * pg_query_result;
    int
        field_count,
        row_count,
        row_index,
        field_index;

    if ( !CheckConnection() )
    {
        return false;
    }

    pg_query_result = PQexec( postgresConnection, query_text.data() );

    if ( PQresultStatus( pg_query_result ) != PGRES_TUPLES_OK )
    {
        printf("database error: %s\n",PQerrorMessage( postgresConnection ) );
        printf( query_text.data() );
        printf( "\n" );

        PQclear( pg_query_result );

        return false;
    }

    field_count = PQnfields( pg_query_result );
    row_count = PQntuples( pg_query_result );

    query_result.SetFieldCount( field_count );
    query_result.SetRowCount( row_count );

    for( field_index = 0 ; field_index < field_count ; ++field_index )
    {
        std::string
            row_value,
            field_name;

        field_name = PQfname( pg_query_result, field_index );
        query_result.SetFieldNameAtIndex( field_name, field_index );

        for( row_index = 0 ; row_index < row_count ; ++row_index )
        {
            row_value = PQgetvalue( pg_query_result, row_index, field_index );
            query_result.SetFieldValue( row_value, field_index, row_index );
        }
    }

    PQclear( pg_query_result );

    return true;
}

bool DatabaseConnection::ExecuteUpdate( const std::string & query_text )
{
    PGresult
        * pg_query_result;

    if ( !CheckConnection() )
    {
        return false;
    }

    pg_query_result = PQexec( postgresConnection, query_text.data() );

    if ( ( PQresultStatus( pg_query_result ) != PGRES_TUPLES_OK ) && ( PQresultStatus( pg_query_result ) != PGRES_COMMAND_OK ) )
    {
        printf("database error: %s\n",PQerrorMessage(postgresConnection));
        printf( query_text.data() );
        printf( "\n" );

        PQclear( pg_query_result );

        return false;
    }

    PQclear( pg_query_result );

    return true;
}

unsigned int DatabaseConnection::GetNewPrimaryKeyValue( 
    const std::string & table_name, 
    const std::string & primary_key_name 
    )
{
    DatabaseQueryResult
        query_result;
    std::string
        sequence_name,
        get_sequence_name_text,
        get_value_text;
    unsigned int
        return_value;
    char 
        * end;

    get_sequence_name_text = "SELECT pg_get_serial_sequence(\'\"";
    get_sequence_name_text += table_name;
    get_sequence_name_text += "\"\',\'";
    get_sequence_name_text += primary_key_name;
    get_sequence_name_text += "\');";

    if( ! ExecuteQuery(
        query_result,
        get_sequence_name_text
        ) )
    {
        assert( false );
    }

    sequence_name = query_result.GetFieldValue( 0, 0 );
    get_value_text = "SELECT nextval(\'";
    get_value_text += sequence_name;
    get_value_text += "\');";

    if( ! ExecuteQuery(
        query_result,
        get_value_text
        ) )
    {
        assert( false );
    }

    return_value = std::strtol(query_result.GetFieldValue( 0, 0 ).data(),&end,10);

    assert( return_value );

    return return_value;
}

bool DatabaseConnection::BeginTransaction()
{
    if ( transactionCounter == 0 )
	{
		ExecuteUpdate( "BEGIN" );
	}

	++transactionCounter;
	return true;
}

bool DatabaseConnection::EndTransaction()
{
    --transactionCounter;
	if ( transactionCounter < 0 )
		return false;

	if ( transactionCounter == 0 )
	{
		ExecuteUpdate( "COMMIT" );
	}

	return true;
}
