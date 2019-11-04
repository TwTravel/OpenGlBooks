// DatabaseSerialization.cpp : Defines the entry point for the console application.
//

#include "TestClasses\TestClass.h"
#include "TestClasses\TestClass2.h"
#include "TestClasses\BaseClass.h"
#include "TestClasses\SubClass.h"
#include "Database\DatabaseConnection.h"
#include "Database\DatabaseManager.h"

#define DatabaseName "postgres"
#define DatabaseUserName "postgres"
#define DatabasePassword "banana"
#define DatabaseAddress "127.0.0.1"

int main(int argc, char* argv[])
{
    DatabaseConnection
        * connection;
    DatabaseManager
        * manager;
    TestClass
        * test_class;
    SubClass
        * sub_class,
        * sub_class_2;
    TestClass2
        * test_class_2;
    Array<MetaObject *>
        result_array,
        result_array_2,
        result_array_3;


    connection = new DatabaseConnection;
    
	if ( !connection->OpenConnection( DatabaseAddress, DatabaseName, DatabaseUserName, DatabasePassword ) )
    {
        printf("Error opening connection to database\n" );
        delete connection;
        return -1;
    }

    manager = new DatabaseManager;
    manager->SetConnection( connection );

    // Initialize the test data

    test_class = new TestClass;
    test_class->baseClassInstance.anInt = 4;
    test_class->baseClassInstance.aText = "Hello world";
    
    sub_class = new SubClass;
    sub_class->anInt = 20;
    sub_class->anotherInt = 6;
    sub_class->aText = "Subclass";
    
    // We put a pointer to a subclass to show the polymorphism
    test_class->baseClassPointer = sub_class;
    
    // And a simple array of ints
    test_class->intTable.SetItemCount( 3 );
    test_class->intTable[ 0 ] = 3;
    test_class->intTable[ 1 ] = 2;
    test_class->intTable[ 2 ] = 1;
    
    
    // Second test case, arrays with objects
    test_class_2 = new TestClass2;
    test_class_2->baseClassTable.SetItemCount( 2 );
    test_class_2->baseClassTable[ 0 ].anInt = 1;
    test_class_2->baseClassTable[ 0 ].aText = "Hello";
    test_class_2->baseClassTable[ 1 ].anInt = 2;
    test_class_2->baseClassTable[ 1 ].aText = "World";

    test_class_2->PolymorphicBaseClassTable.SetItemCount( 2 );
    
    sub_class_2 = new SubClass;
    sub_class_2->anInt = 10;
    sub_class_2->anotherInt = 5;
    sub_class_2->aText = "Subclass2";

    test_class_2->PolymorphicBaseClassTable[ 0 ] = sub_class_2;
    // Lets do something "tricky", make a pointer to an object stored by value
    test_class_2->PolymorphicBaseClassTable[ 1 ] = &test_class_2->baseClassTable[ 0 ];

    // Run the 2 insert operations

    manager->InsertObject( test_class, test_class->GetMetaType() );
    manager->InsertObject( test_class_2, test_class_2->GetMetaType() );

    // Next we take a part of one of those objects and update it
    sub_class_2->aText = "This is the updated text";
    manager->UpdateObject( sub_class_2, sub_class_2->GetMetaType() );

    // We do a cleanup before retrieving data
    delete test_class;
    delete sub_class,
    delete sub_class_2;
    delete test_class_2;

    connection->CloseConnection();

    delete manager;
    delete connection;


    // Now start retrieving data

    connection = new DatabaseConnection;

    if ( !connection->OpenConnection( DatabaseAddress, DatabaseName, DatabaseUserName, DatabasePassword ) )
    {
        printf("Error opening connection to database\n" );
        delete connection;
        return -1;
    }

    manager = new DatabaseManager;
    manager->SetConnection( connection );

    // try querying for an object which doesnt exist
    manager->GetObjects( result_array, SubClass::GetStaticMetaType(), "\"anotherInt\" = 7" );
    assert( result_array.GetItemCount() == 0 );

    // Try getting one which does exist
    manager->GetObjects( result_array, SubClass::GetStaticMetaType(), "\"anotherInt\" = 6" );

    assert( ((SubClass *)result_array[ 0 ])->anotherInt == 6 );
    
    // Remember from above, when inserting
    // sub_class->aText = "Subclass";
    assert( ((SubClass *)result_array[ 0 ])->aText == "Subclass" );

    // Get it a second time and verify we get the same objects
    manager->GetObjects( result_array_2, SubClass::GetStaticMetaType(), "\"anotherInt\" = 6" );
    assert( result_array_2[ 0 ] == result_array[ 0 ] );
    
    // Now retrieve our complex object, without filter
    manager->GetObjects( result_array_3, TestClass2::GetStaticMetaType(), "" );
    
    // See if the data is correct
    TestClass2
        * result_object = ( TestClass2 *)result_array_3[ 0 ];
    
    // We check some values
    assert( result_object->baseClassTable.GetItemCount() == 2 );
    assert( result_object->baseClassTable[ 0 ].anInt == 1 );
    assert( result_object->baseClassTable[ 1 ].aText == "World" );

    assert( result_object->PolymorphicBaseClassTable.GetItemCount() == 2 );
    
    // Did polymorphism work?
    assert( &result_object->PolymorphicBaseClassTable[ 0 ]->GetMetaType() == &SubClass::GetStaticMetaType() );
    // See if he resolved our "pointer-to-member-object" correctly
    assert( result_object->PolymorphicBaseClassTable[ 1 ] == &result_object->baseClassTable[ 0 ] );


	return 0;
}

