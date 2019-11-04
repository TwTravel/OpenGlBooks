//==============================================================
//==============================================================
//= log.cpp ====================================================
//= Original coders: Evan Pipho (terminate@gdnmail.net) and    =
//=					 Trent Polack (trent@voxelsoft.com)		   =
//==============================================================
//= The routines for the run-time log (HTML output) component  =
//==============================================================
//==============================================================


//------------------------------------------------------------------//
//------------------------------------------------------------------//
//- HEADERS AND LIBRARIES ------------------------------------------//
//------------------------------------------------------------------//
//------------------------------------------------------------------//
#include <windows.h>
#include <time.h>

#include "log.h"


//--------------------------------------------------------------
//--------------------------------------------------------------
//- GLOBALS ----------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
CLOG g_log;


//--------------------------------------------------------------
//--------------------------------------------------------------
//- DEFINITIONS ------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------

//--------------------------------------------------------------
// Name:			CLOG::Init - public
// Description:		Initiate the log file
// Arguments:		-szFilename: name of the log HTML file
// Return Value:	A boolean value: -true: successful initiation
//									 -false: unsuccessful initiation
//--------------------------------------------------------------
bool CLOG::Init( char* szFilename )
{
	FILE*  pFile;
	time_t ttime;
	tm*    pCurTime;
	char   cLogStart[128];

	//Store filename & type for later use
	m_szFilename= szFilename;

	//Get the current time
	time( &ttime );
	pCurTime= localtime(&ttime);
	strftime( cLogStart, 128, "Log Started on %m/%d/%Y at %H:%M:%S", pCurTime );

	//open the log file
	pFile= fopen( szFilename, "wt" );

	//something *really* went wrong
	if( !pFile )
		return false;

	//setup the HTML file	
	fprintf( pFile, "<HTML>\n<TITLE>%s</TITLE>\n"
					"<BODY BGCOLOR= 000000>\n"
					"<FONT COLOR= FFFFFF>%s</FONT><BR><BR>\n"
					"</BODY>\n</HTML>", cLogStart, cLogStart );

	//enable the log
	m_bEnabled= true;

	//close the log file
	fclose( pFile );
	return true;
}

//--------------------------------------------------------------
// Name:			CLOG::Write - public
// Description:		Write a colored string to the log file
// Arguments:		-color: color from the ELOG_COLORS enumeration
//					-szString: the C-style string to output to the log
// Return Value:	None
//--------------------------------------------------------------
void CLOG::Write( ELOG_COLORS color, char* szString, ... )
{
	FILE* pFile;
	va_list va;
	time_t ttime;
	tm* pCurTime;

	char szParsedString[1024];
	char szLogTime[32];

	if( !m_bEnabled )
		return;

	//get time
	time( &ttime );
	pCurTime= localtime( &ttime );
	strftime( szLogTime, 32, "<PRE>%H:%M:%S    ", pCurTime );

	//parse the given string
	va_start( va, szString );
		vsprintf( szParsedString, szString, va );
	va_end( va );

	//open the log file
	pFile= fopen( m_szFilename, "r+" );
	if( !pFile )
		return;

	//write text to the log file
	fseek( pFile, -16, SEEK_END );
	fprintf( pFile, "<FONT COLOR= \"%s\">%s %s</PRE></FONT>\n"
					"</BODY>\n</HTML>", GetColor( color ), szLogTime, szParsedString );
	fclose( pFile );
}

//--------------------------------------------------------------
// Name:			CLOG::GetColor - private
// Description:		Convert a constant to an HTML color string
// Arguments:		-color: color from the ELOG_COLORS enumeration
// Return Value:	A char string: HTML color value that corresponds
//					to the given constant
//--------------------------------------------------------------
char* CLOG::GetColor( ELOG_COLORS color )
{
	switch( color )
	{
		case LOG_SUCCESS:
			return "#00FF00";
			break;

		case LOG_FAILURE:
			return "#FF0000";
			break;

		case LOG_PLAINTEXT:
			return "#FFFFFF";
			break;

		case LOG_RENDERER:
		return "#4995B1";
			break;
	}

	return NULL;
}