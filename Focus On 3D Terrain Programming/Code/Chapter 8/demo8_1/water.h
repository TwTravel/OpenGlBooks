//==============================================================
//==============================================================
//= water.h ====================================================
//= Original coders: Trent Polack (trent@voxelsoft.com)		   =
//==============================================================
//= A simple water system
//==============================================================
//==============================================================
#ifndef __WATER_H__
#define __WATER_H__


//--------------------------------------------------------------
//--------------------------------------------------------------
//- CLASS ------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
class CWATER
{
	private:
		CVECTOR m_vecColor;
		float m_fTransparency;

		unsigned int m_texID;

	public:

	void Render( float fWorldSize );

	void LoadTextureMaps( char* szFilename );

	//--------------------------------------------------------------
	// Name:		 CWATER::SetColor - public
	// Description:	 Set the color of the water mesh
	// Arguments:	 -fRed, fGreen, fBlue, fAlpha: RGBA color values
	// Return Value: None
	//--------------------------------------------------------------
	inline void SetColor( float fRed, float fGreen, float fBlue, float fAlpha )
	{
		m_vecColor.Set( fRed, fGreen, fBlue );
		m_fTransparency= fAlpha;
	}

	//--------------------------------------------------------------
	// Name:			CWATER::CWATER - public
	// Description:		Default constructor
	// Arguments:		None
	// Return Value:	None
	//--------------------------------------------------------------
	CWATER( void ) : m_vecColor( 1.0f, 1.0f, 1.0f ), m_fTransparency( 1.0f )
	{	}

	//--------------------------------------------------------------
	// Name:			CWATER::~CWATER - public
	// Description:		Default destructor
	// Arguments:		None
	// Return Value:	None
	//--------------------------------------------------------------
	~CWATER( void )
	{	}
};


#endif	//__WATER_H__