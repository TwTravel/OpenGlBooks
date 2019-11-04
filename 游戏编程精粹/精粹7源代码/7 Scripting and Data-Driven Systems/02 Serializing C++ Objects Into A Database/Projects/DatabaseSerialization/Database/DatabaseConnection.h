#ifndef __DATABASE_CONNECTION__
#define __DATABASE_CONNECTION__

#include <string>
#include ".\Postgresql82\include\libpq-fe.h"
class DatabaseQueryResult;

class DatabaseConnection
{
public:

    DatabaseConnection();
    virtual ~DatabaseConnection();

    bool OpenConnection(
        const std::string & host_address,
        const std::string & database_name,
        const std::string & username,
        const std::string & password
        );

    bool CloseConnection();

    bool CheckConnection();

    bool DoesTableExist( const std::string & table_name );

    bool ExecuteQuery( DatabaseQueryResult & query_result, const std::string & query_text );
    bool ExecuteUpdate( const std::string & query_text );

    unsigned int GetNewPrimaryKeyValue( const std::string & table_name, const std::string & primary_key_name );

    bool BeginTransaction();
    bool EndTransaction();

private:

    PGconn * postgresConnection;
	unsigned int transactionCounter;
};


#endif