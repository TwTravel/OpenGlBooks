
#include <stdio.h>
#include <stdlib.h>
#include <GL/gl.h>

GLubyte*
readImage( const char* filename, GLsizei* width, GLsizei *height )
{
    int       n;
    GLubyte*  pixels;

    FILE* infile = fopen( filename, "rb" );

    if ( !infile ) {
	fprintf( stderr, "Unable to open file '%s'\n", filename );
	return NULL;
    }

    fread( width, sizeof( GLsizei ), 1, infile );
    fread( height, sizeof( GLsizei ), 1, infile );

    n = 3 * (*width) * (*height);

    pixels = (GLubyte *) malloc( n * sizeof( GLubyte ));
    if ( !pixels ) {
	fprintf( stderr, "Unable to malloc() bytes for pixels\n" );
	return NULL;
    }

    fread( pixels, sizeof( GLubyte ), n, infile );
    
    fclose( infile );

    return pixels;
}
