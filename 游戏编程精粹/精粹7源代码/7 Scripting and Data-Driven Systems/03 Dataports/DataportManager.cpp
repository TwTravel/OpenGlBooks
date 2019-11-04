/*
 *  DataportManager.cpp
 */

#include "DataportManager.h"

DataportManager* DataportManager::s_pInstance;

//------------------------------------------------------------------------------

DataportManager::DataportManager()
{
	m_entries.clear();
}

//------------------------------------------------------------------------------

DataportManager::~DataportManager()
{
	
}

//------------------------------------------------------------------------------

DataportManager* DataportManager::GetInstance( void )
{
	if( !s_pInstance )
	{
		s_pInstance = new DataportManager;
	}
	return s_pInstance;
}

//------------------------------------------------------------------------------
// Destroy
//------------------------------------------------------------------------------
//
// Parameters:	None.
//
// Returns:		None.
//
// Description:	Destroys the singleton instance of DataportManager. Only to be
//				done at program termination.
//
//------------------------------------------------------------------------------

void DataportManager::Destroy( void )
{
	if( s_pInstance )
	{
		delete s_pInstance;
		s_pInstance = 0;
	}
}

//------------------------------------------------------------------------------

DataportManager::ReturnValue
DataportManager::Add( std::string name, void* ptr, unsigned int classID )
{
	DataportEntry entry;
	
	entry.ptr = ptr;
	entry.name = name;
	entry.classID = classID;
	
	// check for collision with existing emtries

	if( Poll( ptr ) == kSuccess )
		return kErrorDuplicate;
	
	m_entries.push_back( entry );
	
	return kSuccess;
}

//------------------------------------------------------------------------------

DataportManager::ReturnValue
DataportManager::Remove( void* ptr )
{
	for( std::list<DataportEntry>::iterator iter = m_entries.begin() ; iter != m_entries.end() ; ++iter )
	{
		if( iter->ptr == ptr )
		{
			m_entries.erase( iter );
			return kSuccess;
		}
	}
	
	return kErrorNotFound;
}

//------------------------------------------------------------------------------

DataportManager::ReturnValue
DataportManager::Poll( void* ptr )
{
	for( std::list<DataportEntry>::iterator iter = m_entries.begin() ; iter != m_entries.end() ; ++iter )
	{
		if( iter->ptr == ptr )
		{
			return kSuccess;
		}
	}
	return kErrorNotFound;
}

//------------------------------------------------------------------------------

void*
DataportManager::Attach( std::string name, unsigned int classID )
{
	for( std::list<DataportEntry>::iterator iter = m_entries.begin() ; iter != m_entries.end() ; ++iter )
	{
		if( ( iter->classID == classID ) &&
			( iter->name == name ) )
		{
			return iter->ptr;
		}
	}
	return 0;
}

//------------------------------------------------------------------------------

DataportManager::ReturnValue
DataportManager::Detach( void* ptr )
{	
	return kSuccess;
}

//------------------------------------------------------------------------------

