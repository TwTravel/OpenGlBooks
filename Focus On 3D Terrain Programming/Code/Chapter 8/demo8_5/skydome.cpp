//==============================================================
//==============================================================
//= skydome.cpp ================================================
//= Original coders: Trent Polack (trent@voxelsoft.com)		   =
//==============================================================
//= Routines for a skydome (half-sphere) system				   =
//==============================================================
//==============================================================


//--------------------------------------------------------------
//--------------------------------------------------------------
//- HEADERS AND LIBRARIES --------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
#include <math.h>

#include "../Base Code/math_ops.h"
#include "../Base Code/gl_app.h"
#include "../Base Code/image.h"

#include "skydome.h"


//--------------------------------------------------------------
//--------------------------------------------------------------
//- DEFINITIONS ------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------

//--------------------------------------------------------------
// Name:		 CSKYDOME::Init - public
// Description:	 Create the skydome
// Arguments:	 -fTheta/fPhi: "latitude" and "longitude" of the dome
//				 -fRadius: radius of the dome in world space
// Return Value: None
//--------------------------------------------------------------
void CSKYDOME::Init( float fTheta, float fPhi, float fRadius )
{
	CVECTOR vecUV;
	const float fDTOR= PI/180.0f;
	int iTheta, iPhi, i, n= 0;

	//calculate the number of vertices for the skydome
	m_iNumVertices= ( int )( ( 360/fTheta )*( 90/fPhi )*4 );

	//allocate memory for the vertex and texture coordinate buffers
	m_fVertices  = new float [m_iNumVertices*3];
	m_fTexCoords= new float [m_iNumVertices*2];

	//initialize the values for the buffers
	memset( m_fVertices, 0, sizeof( float )*m_iNumVertices*3 );
	memset( m_fTexCoords, 0, sizeof( float )*m_iNumVertices*2 );

	//procedurally generate the dome
	for( iPhi=0; iPhi<=( 90-fPhi ); iPhi+=( int )fPhi )
	{
		for( iTheta=0; iTheta<=( 360-fTheta ); iTheta+=( int )fTheta )
		{
			//compute the vertex at phi, theta
			m_fVertices[( n*3 )+0]= fRadius*sinf( iPhi*fDTOR )*cosf( fDTOR*iTheta );
			m_fVertices[( n*3 )+1]= fRadius*sinf( iPhi*fDTOR )*sinf( fDTOR*iTheta );
			m_fVertices[( n*3 )+2]= fRadius*cosf( iPhi*fDTOR );

			//create a vector from the origin to this vertex and normalize it
			vecUV.Set( m_fVertices[( n*3 )+0], m_fVertices[( n*3 )+1], m_fVertices[( n*3 )+2] );
			vecUV.Normalize( );

			//compute the texture coordinates for the current vertex
			m_fTexCoords[( n*2 )+0]= ( float )( atan2( vecUV[0], vecUV[2] )/( PI*2 ) )+0.5f;
			m_fTexCoords[( n*2 )+1]= ( float )( asinf( vecUV[1] )/PI )+0.5f;		
			n++;

			//compute the vertex at phi+phi, theta
			m_fVertices[( n*3 )+0]= fRadius*sinf( ( iPhi+fPhi )*fDTOR )*cosf( iTheta*fDTOR );
			m_fVertices[( n*3 )+1]= fRadius*sinf( ( iPhi+fPhi )*fDTOR )*sinf( iTheta*fDTOR );
			m_fVertices[( n*3 )+2]= fRadius*cosf( ( iPhi+fPhi )*fDTOR );
			
			//create a vector from the origin to this vertex and normalize it
			vecUV.Set( m_fVertices[( n*3 )+0], m_fVertices[( n*3 )+1], m_fVertices[( n*3 )+2] );
			vecUV.Normalize( );

			//compute the texture coordinates for the current vertex
			m_fTexCoords[( n*2 )+0]= ( float )( atan2( vecUV[0], vecUV[2] )/( PI*2 ) )+0.5f;
			m_fTexCoords[( n*2 )+1]= ( float )( asinf( vecUV[1] )/PI )+0.5f;		
			n++;

			//compute the vertex at phi, theta+theta
			m_fVertices[( n*3 )+0]= fRadius*sinf( fDTOR*iPhi )*cosf( fDTOR*( iTheta+fTheta ) );
			m_fVertices[( n*3 )+1]= fRadius*sinf( fDTOR*iPhi )*sinf( fDTOR*( iTheta+fTheta ) );
			m_fVertices[( n*3 )+2]= fRadius*cosf( fDTOR*iPhi );
			
			//create a vector from the origin to this vertex and normalize it
			vecUV.Set( m_fVertices[( n*3 )+0], m_fVertices[( n*3 )+1], m_fVertices[( n*3 )+2] );
			vecUV.Normalize( );

			//compute the texture coordinates for the current vertex
			m_fTexCoords[( n*2 )+0]= ( float )( atan2( vecUV[0], vecUV[2] )/( PI*2 ) )+0.5f;
			m_fTexCoords[( n*2 )+1]= ( float )( asinf( vecUV[1] )/PI )+0.5f;		
			n++;

			if( iPhi>-90 && iPhi<90 )
			{
				//compute the vertex at phi+phi, theta+theta
				m_fVertices[( n*3 )+0]= fRadius*sinf( ( iPhi+fPhi )*fDTOR )*cosf( fDTOR*( iTheta+fTheta ) );
				m_fVertices[( n*3 )+1]= fRadius*sinf( ( iPhi+fPhi )*fDTOR )*sinf( fDTOR*( iTheta+fTheta ) );
				m_fVertices[( n*3 )+2]= fRadius*cosf( ( iPhi+fPhi )*fDTOR );
				
				//create a vector from the origin to this vertex and normalize it
				vecUV.Set( m_fVertices[( n*3 )+0], m_fVertices[( n*3 )+1], m_fVertices[( n*3 )+2] );
				vecUV.Normalize( );

				//compute the texture coordinates for the current vertex
				m_fTexCoords[( n*2 )+0]= ( float )( atan2( vecUV[0], vecUV[2] )/( PI*2 ) )+0.5f;
				m_fTexCoords[( n*2 )+1]= ( float )( asinf( vecUV[1] )/PI )+0.5f;		
				n++;
			}
		}
	}

	//fix the texture-seam problem
	for( i=0; i<m_iNumVertices-3; i++ )
	{
		int i0= ( i*2 );
		int i1= ( ( i+1 )*2 );
		int i2= ( ( i+2 )*2 );

		if( ( m_fTexCoords[i0+0]-m_fTexCoords[i1+0] )>0.9f )
			m_fTexCoords[i1+0]+= 1.0f;

		if( ( m_fTexCoords[i1+0]-m_fTexCoords[i0+0] )>0.9f )
			m_fTexCoords[i0+0]+= 1.0f;

		if( ( m_fTexCoords[i0+0]-m_fTexCoords[i2+0] )>0.9f )
			m_fTexCoords[i2+0]+= 1.0f;

		if( ( m_fTexCoords[i2+0]-m_fTexCoords[i0+0] )>0.9f )
			m_fTexCoords[i0+0]+= 1.0f;

		if( ( m_fTexCoords[i1+0]-m_fTexCoords[i2+0] )>0.9f )
			m_fTexCoords[i2+0]+= 1.0f;

		if( ( m_fTexCoords[i2+0]-m_fTexCoords[i1+0] )>0.9f )
			m_fTexCoords[i1+0]+= 1.0f;

		if( ( m_fTexCoords[i0+1]-m_fTexCoords[i1+1] )>0.8f )
			m_fTexCoords[i1+1]+= 1.0f;

		if( ( m_fTexCoords[i1+1]-m_fTexCoords[i0+1] )>0.8f )
			m_fTexCoords[i0+1]+= 1.0f;

		if( ( m_fTexCoords[i0+1]-m_fTexCoords[i2+1] )>0.8f )
			m_fTexCoords[i2+1]+= 1.0f;

		if( ( m_fTexCoords[i2+1]-m_fTexCoords[i0+1] )>0.8f )
			m_fTexCoords[i0+1]+= 1.0f;

		if( ( m_fTexCoords[i1+1]-m_fTexCoords[i2+1] )>0.8f )
			m_fTexCoords[i2+1]+= 1.0f;

		if( ( m_fTexCoords[i2+1]-m_fTexCoords[i1+1] )>0.8f )
			m_fTexCoords[i1+1]+= 1.0f;
	}
}

//--------------------------------------------------------------
// Name:		 CSKYDOME::Shutdown - public
// Description:	 Shutdown the skydome system
// Arguments:	 None
// Return Value: None Render the water mesh
//--------------------------------------------------------------
void CSKYDOME::Shutdown( void )
{
	delete[] m_fTexCoords;
	delete[] m_fVertices;
}

//--------------------------------------------------------------
// Name:		 CSKYDOME::Render - public
// Description:	 Render the skydome
// Arguments:	 -fDelta: time passed since the previous frame (for rotation purposes)
//				 -bRotate: rotate the skydome or not to simulate cloud movement
// Return Value: None
//--------------------------------------------------------------
void CSKYDOME::Render( float fDelta, bool bRotate )
{
	static float fRot= 0.0f;

	//bind the dome texture
	glBindTexture( GL_TEXTURE_2D, m_uiTexID );
	glEnable( GL_TEXTURE_2D );

	glPushMatrix( );
		glTranslatef( m_vecCenter[0], m_vecCenter[1], m_vecCenter[2] );

		//rotate the dome to simulate cloud movement
		if( bRotate )
		{
			fRot+= fDelta;
			glRotatef( fRot, 0.0f, 1.0f, 0.0f );
		}
		//orient the dome correctly
		glRotatef( 270, 1.0f, 0.0f, 0.0f );

		glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );

		//enable vertex arrays and send OpenGL the vertex buffer pointer
		glEnableClientState( GL_VERTEX_ARRAY );
		glVertexPointer( 3, GL_FLOAT, 0, m_fVertices );

		//enable normal arrays and send OpenGL the normal buffer pointer
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );	
		glTexCoordPointer( 2, GL_FLOAT, 0, m_fTexCoords );

		//lock the arrays
		glLockArraysEXT( 0, m_iNumVertices );

		//render the skydome
		glDrawArrays( GL_TRIANGLE_STRIP, 0, m_iNumVertices );

		//unlock the arrays
		glUnlockArraysEXT( );

		//disable vertex arrays
		glDisableClientState( GL_VERTEX_ARRAY );
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	glPopMatrix( );
}

//--------------------------------------------------------------
// Name:		 CSKYDOME::GenCloudTexture - public
// Description:	 Fractally generate a cloud texture
// Arguments:	 -size: size of the texture to generate
//				 -blur: how much to blur the texture
//				 -x, y: where to calculate the value for
//				 -fOctaves: number of noise values to add
//				 -fAmplitude: amount of "height" the values have
//				 -fFrequency: amount of randomness
//				 -fH: change in amplitude per octave
// Return Value: A floating point random value
//--------------------------------------------------------------
void CSKYDOME::GenCloudTexture( int size, float fBlur, float fOctaves, float fAmplitude,
								float fFrequency, float fH )
{
	float* ucpTexData;
	float* fpData;
	float fTemp=0;
	int x, y;

	//allocate the buffer for the fractal generation data
	fpData= new float [SQR( size )];

	//fractally generate the data
	for( y=0; y<size; y++ )
	{
		for( x=0; x<size; x++ )
			fpData[( y*size )+x]= FBM( ( float )x, ( float )y, fOctaves, fAmplitude, fFrequency, fH );
	}

	//normalize the buffer's values to the range of 0-255
	NormalizeFractal( fpData, size );
	for( y=0; y<size-1; y++ )
	{
		for( x=0; x<size-1; x++ )
		{
			//eliminate "extra" Noise
			if( fpData[( y*size )+x]<128 )
				fpData[( y*size )+x]= 0;
		}
	}

	//blur the data
	Blur( fpData, size, fBlur );

	//allocate memory for the texture data
	ucpTexData= new float [SQR( size )*3];

	//generate the cloud texture map
	for( y=0; y<size-1; y++ )
	{
		for( x=0; x<size-1; x++ )
		{
			int index= ( y*size )+x;

			//fill the buffer with values for the current RGB pixel
			ucpTexData[( index*3 )+0]= 0.25f+( fpData[index]/255 );
			ucpTexData[( index*3 )+1]= 0.25f+( fpData[index]/255 );
			ucpTexData[( index*3 )+2]= 1.0f+( fpData[index]/255 );

			//clamp the data to a range of [0, 1]
			CLAMP( ucpTexData[( index*3 )+0], 0.0f, 1.0f );
			CLAMP( ucpTexData[( index*3 )+1], 0.0f, 1.0f );
			CLAMP( ucpTexData[( index*3 )+2], 0.0f, 1.0f );
		}
	}

	//build the texture for use with OpenGL
	glGenTextures( 1, &m_uiTexID );
	glBindTexture( GL_TEXTURE_2D, m_uiTexID );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );						

	//create a mipmapped texture
	gluBuild2DMipmaps( GL_TEXTURE_2D, GL_RGB, size, size, 
					   GL_RGB, GL_FLOAT, ucpTexData );

	//delete our previously allocated buffers
	delete[] ucpTexData;
	delete[] fpData;
}

//--------------------------------------------------------------
// Name:		 CSKYDOME::CosineInterpolation - private
// Description:	 Interpolate the two given values
// Arguments:	 -number1, number2: values to interpolate
//				 -x: interpolation bias
// Return Value: A double interpolated value
//--------------------------------------------------------------
float CSKYDOME::CosineInterpolation( float fNum1, float fNum2, float x )
{
	float fXPI, fTemp, fValue;

	fXPI= x*PI;
	fTemp= ( float )( 1-cos( fXPI ) )*0.5f;
	fValue= fNum1*( 1-fTemp )+( fNum2*fTemp );
	
	return fValue;
}

//--------------------------------------------------------------
// Name:		 CSKYDOME::RangedRandom - private
// Description:	 Get a ranged random value
// Arguments:	 -x, y: value range
// Return Value: A floating point random value
//--------------------------------------------------------------
float CSKYDOME::RangedRandom( int x, int y )
{
	float fValue;
	int n= x+y*57;

	n= ( n<<13 )^n;

	fValue= ( 1-( ( n*( n*n*15731+789221 )+1376312589 ) & 2147483647 )/1073741824.0f );
	return fValue;    
}

//--------------------------------------------------------------
// Name:		 CSKYDOME::RangedSmoothRandom - private
// Description:	 Get a ranged random "smoothed" value
// Arguments:	 -x, y: value range
// Return Value: A floating point random value
//--------------------------------------------------------------
float CSKYDOME::RangedSmoothRandom( int x, int y )
{
	float fCorners, fSides, fValue;
	float fCenter= RangedRandom( x, y )/4; 

	fCorners= ( RangedRandom( x-1, y-1 )+
				RangedRandom( x+1, y-1 )+
				RangedRandom( x-1, y+1 )+
				RangedRandom( x+1, y+1 ) )/16;
	fSides  = ( RangedRandom( x-1, y )+
				RangedRandom( x+1, y )+
				RangedRandom( x, y-1 )+
				RangedRandom( x, y+1 ) )/8;

	fValue= fCorners+fSides+fCenter;

	return fValue;
}

//--------------------------------------------------------------
// Name:		 CSKYDOME::Noise - private
// Description:	 A noise calculation function
// Arguments:	 -x, y: where on the fractal map to calculate noise for
// Return Value: A floating point noise value
//--------------------------------------------------------------
float CSKYDOME::Noise( float x, float y )
{
	int iX= ( int )x;
	int iY= ( int )y;
	float f1, f2, f3, f4, fI1, fI2, fValue;
	float fFracX= x-iX;
	float fFracY= y-iY;

	//generate a random noise value
	f1= RangedSmoothRandom( iX,   iY );
	f2= RangedSmoothRandom( iX+1, iY );
	f3= RangedSmoothRandom( iX,   iY+1 );
	f4= RangedSmoothRandom( iX+1, iY+1 );
	fI1= CosineInterpolation( f1, f2, fFracX );
	fI2= CosineInterpolation( f3, f4, fFracX );
	fValue= CosineInterpolation( fI1, fI2, fFracY );
	return fValue;
}

//--------------------------------------------------------------
// Name:			CSKYDOME::FBM - private
// Description:		Returns a fractally generated (fractal brownian motion) value
// Arguments:		-x, y: where to calculate the value for
//					-fOctaves: number of noise values to add
//					-fAmplitude: amount of "height" the values have
//					-fFrequency: amount of randomness
//					-fH: change in amplitude per octave
// Return Value:	A floating point fractally generated value
//--------------------------------------------------------------
float CSKYDOME::FBM( float x, float y, float fOctaves, float fAmplitude, float fFrequency, float fH )
{
	float fValue= 0;

	//generate a fractal value using "fractal brownian motion"
	for( int i=0; i<( fOctaves-1 ); i++ )
	{
		fValue+= ( Noise( x*fFrequency, y*fFrequency )*fAmplitude );
		fAmplitude*= fH;
	}

	return fValue;	
}

//--------------------------------------------------------------
// Name:		 CSKYDOME::NormalizeFractal - private
// Description:	 Scale the values of a buffer to the range of [0, 255]
// Arguments:	 -fpData: the data buffer
//				 -iSize: size of the data buffer
// Return Value: None
//--------------------------------------------------------------
void CSKYDOME::NormalizeFractal( float* fpData, int iSize )
{
	float fMin, fMax;
	float fHeight;
	int i;

	fMin= fpData[0];
	fMax= fpData[0];

	//find the min/max values of the buffer
	for( i=1; i<SQR( iSize ); i++ )
	{
		if( fpData[i]>fMax ) 
			fMax= fpData[i];

		else if( fpData[i]<fMin ) 
			fMin= fpData[i];
	}

	//find the range of the altitude
	if( fMax<=fMin )
		return;

	fHeight= fMax-fMin;

	//scale the values to a range of 0-255 (because I like things that way)
	for( i=0; i<SQR( iSize ); i++ )
		fpData[i]= ( ( fpData[i]-fMin )/fHeight )*255.0f;
}

//--------------------------------------------------------------
// Name:		 CSKYDOME::BlurBand - private
// Description:	 Blur a band of values
// Arguments:	 -fpBand: buffer of values in the band to blue
//				 -iStride: how much to advance each "count"
//				 -iCount: how many values to blur
//				 -fFilter: power of the blur filter
// Return Value: None
//--------------------------------------------------------------
void CSKYDOME::BlurBand( float* fpBand, int iStride, int iCount, float fFilter )
{
	float v= fpBand[0];
	int j  = iStride;
	int i;

	//go through the band and apply the erosion filter
	for( i=0; i<iCount-1; i++ )
	{
		fpBand[j]= fFilter*v + ( 1-fFilter )*fpBand[j];
		
		v = fpBand[j];
		j+= iStride;
	}
}

//--------------------------------------------------------------
// Name:		 CSKYDOME::Blur - private
// Description:	 Blur a buffer of values
// Arguments:	 -fpData: the data buffer to blue
//				 -iSize: size of the data buffer
//				 -fFilter: the power of the blur filter
// Return Value: None
//--------------------------------------------------------------
void CSKYDOME::Blur( float* fpData, int iSize, float fFilter )
{
	int i;

	//erode left to right
	for( i=0; i<iSize; i++ )
		BlurBand( &fpData[iSize*i], 1, iSize, fFilter );
	
	//erode right to left
	for( i=0; i<iSize; i++ )
		BlurBand( &fpData[iSize*i+iSize-1], -1, iSize, fFilter );

	//erode top to bottom
	for( i=0; i<iSize; i++ )
		BlurBand( &fpData[i], iSize, iSize, fFilter);

	//erode from bottom to top
	for( i=0; i<iSize; i++ )
		BlurBand( &fpData[iSize*(iSize-1)+i], -iSize, iSize, fFilter );
}