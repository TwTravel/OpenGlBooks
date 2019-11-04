/*
 *  DataportManager.h
 */

#ifndef DATAPORTMANAGER_H
#define DATAPORTMANAGER_H

#include<string>
#include<list>

class DataportManager {
public:
	
	enum ReturnValue {
		kSuccess = 0,
		kErrorNotFound,
		kErrorNonZeroRefCount,
		kErrorWrongType,
		kErrorDuplicate
	};
	
	//--------------------------------------------------------------------------
	// GetInstance
	//--------------------------------------------------------------------------
	//
	// Parameters:	None.
	//
	// Returns:		Static pointer to instance of DataportManager.
	//
	// Description:	Returns singleton instance of DataportManager.
	//
	//--------------------------------------------------------------------------
	
	static DataportManager*	GetInstance( void );
	
	//--------------------------------------------------------------------------
	// Destroy
	//--------------------------------------------------------------------------
	//
	// Parameters:	None.
	//
	// Returns:		None.
	//
	// Description:	Destroys the singleton instance of DataportManager. Only to 
	//				be done at program termination.
	//
	//--------------------------------------------------------------------------
	
	void	Destroy( void );
	
	//--------------------------------------------------------------------------
	// Add
	//--------------------------------------------------------------------------
	//
	// Parameters:	std::string name - name of the dataport
	//				void* ptr - address of the dataport
	//				unsigned int classID - type of the dataports data
	//
	// Returns:		kSuccess - dataport added successfully
	//				kErrorDuplicate - dataport already exists in system
	//
	// Description:	Adds a dataport to the system
	//
	//--------------------------------------------------------------------------
	
	ReturnValue	Add( std::string name, void* ptr, unsigned int classID );

	//--------------------------------------------------------------------------
	// Remove
	//--------------------------------------------------------------------------
	//
	// Parameters:	void* ptr - address of dataport
	//
	// Returns:		kSuccess - dataport removed OK
	//				kErrorNotFound - requested dataport not in system
	//
	// Description:	Removes a dataport from the system
	//
	//--------------------------------------------------------------------------
	
	ReturnValue	Remove( void* ptr );

	//--------------------------------------------------------------------------
	// Poll
	//--------------------------------------------------------------------------
	//
	// Parameters:	void* ptr - address of dataport
	//
	// Returns:		kSuccess - dataport is present in system
	//				kErrorNotFound - dataport not present in system
	//
	// Description:	Removes a dataport from the system
	//
	//--------------------------------------------------------------------------
	
	ReturnValue	Poll( void* ptr );

	//--------------------------------------------------------------------------
	// Attach
	//--------------------------------------------------------------------------
	//
	// Parameters:	std::string name - name of Dataport to attach to
	//				unsigned int classID - ID of class of Dataport to attach to
	//
	// Returns:		zero - invalid request - could not find requested Dataport
	//				non-zero - pointer to Dataport
	//
	// Description:	Returns a pointer to a requested Dataport
	//
	//--------------------------------------------------------------------------
	
	void*		Attach( std::string name, unsigned int classID );

	//--------------------------------------------------------------------------
	// Detach
	//--------------------------------------------------------------------------
	//
	// Parameters:	void* ptr - pointer to Dataport to detach from
	//
	// Returns:		kSuccess - 
	//
	// Description:	Returns a pointer to a requested Dataport
	//
	//--------------------------------------------------------------------------
	
	ReturnValue	Detach( void* ptr );
	
protected:
		
	DataportManager();
	~DataportManager();
		
	static DataportManager* s_pInstance;
	
	struct DataportEntry {
		std::string		name;
		unsigned int	classID;
		void*			ptr;
	};
	
	std::list<DataportEntry> m_entries;
};

#endif // DATAPORTMANAGER_H
