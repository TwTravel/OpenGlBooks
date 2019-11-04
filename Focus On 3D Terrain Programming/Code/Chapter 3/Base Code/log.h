//==============================================================
//==============================================================
//= log.h ======================================================
//= Original coders: Evan Pipho (evan@codershq.com) and		   =
//=					 Trent Polack (trent@voxelsoft.com)		   =
//==============================================================
//= The routines for the run-time log (HTML output) component  =
//==============================================================
//==============================================================
#ifndef __LOG_H__
#define __LOG_H__


//--------------------------------------------------------------
//--------------------------------------------------------------
//- HEADERS AND LIBRARIES --------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
#include <stdio.h>


//--------------------------------------------------------------
//--------------------------------------------------------------
//- DATA STRUCTURES --------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
enum ELOG_COLORS
{
	LOG_SUCCESS= 0,
	LOG_FAILURE,
	LOG_PLAINTEXT,
	LOG_RENDERER
};


//--------------------------------------------------------------
//--------------------------------------------------------------
//- CLASSES ----------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
class CLOG
{
	private:
		char* m_szFilename;		//the log filename (set at initiation)
		bool m_bEnabled;		//is the log currently enabled?


	char* GetColor( ELOG_COLORS color );

	public:
	

	bool Init( char* szFilename );
	void Write( ELOG_COLORS color, char* szString, ... );

	//----------------------------------------------------------
	// Name:			CLOG::Enable - public
	// Description:		Enable the log
	// Arguments:		None
	// Return Value:	None
	//----------------------------------------------------------
	inline void Enable( void )
	{	m_bEnabled= true;	}

	//----------------------------------------------------------
	// Name:			CLOG::Disable - public
	// Description:		Disable the log
	// Arguments:		None
	// Return Value:	None
	//----------------------------------------------------------
	inline void Disable( void )
	{	m_bEnabled= false;	}

	//----------------------------------------------------------
	// Name:			CLOG::IsEnabled - public
	// Description:		Check to see if the log is enabled or not
	// Arguments:		None
	// Return Value:	A boolean variable: -true: log is enabled
	//										-false: log is disabled
	//----------------------------------------------------------
	inline bool IsEnabled( void )
	{	return m_bEnabled;	}

	//--------------------------------------------------------------
	// Name:			CLOG::CLOG - public
	// Description:		Default constructor
	// Arguments:		None
	// Return Value:	None
	//--------------------------------------------------------------
	CLOG::CLOG( void ) : m_bEnabled( false ), m_szFilename( 0 )
	{	}

	//--------------------------------------------------------------
	// Name:			CLOG::CLOG - public
	// Description:		Constructor that initiates the log immediatly
	// Arguments:		-szFilename: filename the log takes
	// Return Value:	None
	//--------------------------------------------------------------
	CLOG( char* szFilename ) : m_bEnabled( false ), m_szFilename( 0 )
	{	Init( szFilename );	}

	//--------------------------------------------------------------
	// Name:			CLOG::~CLOG - public
	// Description:		Default destructor
	// Arguments:		None
	// Return Value:	None
	//--------------------------------------------------------------
	~CLOG( void )
	{	}
};


//--------------------------------------------------------------
//--------------------------------------------------------------
//- GLOBALS ----------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
extern CLOG g_log;


#endif //__LOG_H__