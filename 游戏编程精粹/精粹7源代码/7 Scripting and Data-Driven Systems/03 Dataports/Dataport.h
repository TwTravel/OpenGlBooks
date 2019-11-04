/*
 *  Dataport.h
 */

#ifndef DATAPORT_H
#define DATAPORT_H

#include <string>
#include "DataportManager.h"

//---

typedef unsigned int tDataportFuncPointer;

template<class T> tDataportFuncPointer GetID()
{
	typedef tDataportFuncPointer (*TempFunc)();
	
	TempFunc FuncPtr = GetID<T>;
	tDataportFuncPointer retID = (tDataportFuncPointer)FuncPtr;	
	return retID;
}

//---

template<class T> class Dataport
{
public:

	DataportManager::ReturnValue Register( std::string name )
	{
		tDataportFuncPointer classID = GetID<T>();		
		DataportManager::ReturnValue returnVal = DataportManager::GetInstance()->Add( name, (void*)this, classID );
		m_refCount = 0;
		return returnVal;
	}
	
	//----------------------------------------------
	
	DataportManager::ReturnValue DeRegister( void )
	{
		if( m_refCount == 0 )
			return DataportManager::GetInstance()->Remove( (void*)this );
		else
			return DataportManager::kErrorNonZeroRefCount;
	}
	
	//----------------------------------------------
	
	Dataport<T> *Attach( std::string name )
	{
		tDataportFuncPointer classID = GetID<T>();
		Dataport<T>* returnVal = (Dataport<T>*)DataportManager::GetInstance()->Attach( name, classID );		

		if(returnVal)
			returnVal->m_refCount++;

		return returnVal;
	}
	
	//----------------------------------------------
	
	DataportManager::ReturnValue Detach(void)
	{
		int returnVal = DataportManager::GetInstance()->Detach( (void*)this );		
		
		if( returnVal == DataportManager::kSuccess )
			this->m_refCount--;
		
		return DataportManager::kErrorNotFound;
	}
	
	//----------------------------------------------
	// debug functions
	
	int Poll(void)
	{
		return DataportManager::GetInstance()->Poll( (void*)this );
	}
	
	unsigned int GetRefCount(void){return m_refCount;}
	
	T	data;
	
protected:
	unsigned int m_refCount;
};


#endif // DATAPORT_H
