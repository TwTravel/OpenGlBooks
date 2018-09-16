/*
 * Copyright 2004, the OpenGLUT contributors.
 *
 * This program exists *solely* in order to compare the old GLUT
 * built-in fonts to the same fonts as GLUT defined them.  Every
 * glyph has a bounding-box drawn around it.  The bounding box includes
 * some "padding" space that is conceptually part of the glyph both
 * for horizontal spacing and for inter-line spacing.
 */
#include <stdio.h>
#include <stdlib.h>

#if defined( USE_GLUT )
#include <GL/glut.h>
#elif defined( USE_FREEGLUT )
#include <GL/freeglut.h>
#else
#include <GL/openglut.h>
#endif

#define NUMBEROF(a) ((sizeof(a))/sizeof(a[0]))

struct font_map
{
    void *id;
    const char *const name;
    int height;
    int descent;
};

struct font_map font_map[ ]=
{
    {GLUT_BITMAP_8_BY_13, "8_BY_13", 14, 2},
    {GLUT_BITMAP_9_BY_15, "9_BY_15", 16, 3},
    {GLUT_BITMAP_TIMES_ROMAN_10, "TIMES_ROMAN_10", 14, 2},
    {GLUT_BITMAP_TIMES_ROMAN_24, "TIMES_ROMAN_24", 29, 5},
    {GLUT_BITMAP_HELVETICA_10, "HELVETICA_10", 14, 2},
    {GLUT_BITMAP_HELVETICA_12, "HELVETICA_12", 16, 3},
    {GLUT_BITMAP_HELVETICA_18, "HELVETICA_18", 23, 4},
};

static int font_num;             /* 0 == 8_BY_13, 1 == 9_BY_15, ... */
static int window;
static int menu;
static int max_glyph_width = 8;  /* Default font is 8_BY_13 */


#if !defined( OPENGLUT ) && !defined( FREEGLUT )
static int glutBitmapHeight( const void *const fid )
{
    int ret = 0;
    int i;
    for(
        i = 0;
        ( i < NUMBEROF( font_map ) ) && ( fid != font_map[ i ].id );
        ++i
    )
        ;
    if( fid == font_map[ i ].id )
        ret = font_map[ i ].height;

    return ret;
}
#endif

/*
 * Not really needed because in the one spot we need this info,
 * we also know the font index in the {font_map} table.
 */
/*
static int glutBitmapDescent( const void *const fid )
{
    int ret = 0;
    int i;
    for(
        i = 0;
        ( i < NUMBEROF( font_map ) ) && ( fid != font_map[ i ].id );
        ++i
    )
        ;
    if( fid == font_map[ i ].id )
        ret = font_map[ i ].descent;

    return ret;
}
*/


void cb_display( void )
{
    int i;
    void *fid = font_map[ font_num ].id;
    int d = font_map[ font_num ].descent;

    glClearColor( 0, 0, 0, 0 );
    glClear( GL_COLOR_BUFFER_BIT );
    for( i = 0; i < 16; ++i )
    {
        int j;
        double x;

        glEnable( GL_BLEND );
        glBlendFunc(
            GL_SRC_ALPHA,
            GL_ONE_MINUS_SRC_ALPHA
        );
        glBegin( GL_LINES );
        glColor4d( .5, 0, 0, .5 );
        for( j = 0, x = 0; j < 16; ++j )
        {
            x += glutBitmapWidth( fid, ( i * 16 ) + j );
            glVertex2d( x, glutBitmapHeight( fid ) * ( i + 1 ) );
            glVertex2d( x, glutBitmapHeight( fid ) * i );
            x += max_glyph_width - glutBitmapWidth( fid, ( i * 16 ) + j);
            glVertex2d( x, glutBitmapHeight( fid ) * ( i + 1 ) );
            glVertex2d( x, glutBitmapHeight( fid ) * i );
        }
        glVertex2d( 0, glutBitmapHeight( fid ) * i );
        glVertex2d( x, glutBitmapHeight( fid ) * i );
        glVertex2d( 0, glutBitmapHeight( fid ) * ( i + 1 ) );
        glVertex2d( x, glutBitmapHeight( fid ) * ( i + 1 ) );
        glVertex2d( 0, glutBitmapHeight( fid ) * i - d );
        glVertex2d( x, glutBitmapHeight( fid ) * i - d );
        glEnd( );
        glDisable( GL_BLEND );
        glColor3d( 1.0, 1.0, 1.0 );
        for( j = 0; j < 16; ++j )
        {
            glRasterPos2i(
                j * max_glyph_width, glutBitmapHeight( fid ) * ( i + 1 ) - d
            );
            glutBitmapCharacter( fid, ( i*16 ) + j );
        }
    }
    glutSwapBuffers( );
    glutSetWindowTitle( font_map[ font_num ].name );
}

void cb_reshape( int w, int h )
{
    glViewport( 0, 0, w, h );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity( );
    glOrtho( 0, w, h, 0, -1, 1 );
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity( );
    glTranslated( 0.375, 0.375, 0 );
    glutPostRedisplay( );
}

void cb_keyboard( unsigned char key, int x, int y )
{
    if( 0x1b == key )
        exit( 0 );
}

void cb_menu( int value )
{
    int need_w;
    int need_h;
    int win_w;
    int win_h;
    void *fid;
    int menu_win = glutGetWindow( );
    int i;

    font_num = value;
    fid = font_map[ font_num ].id;
    glutSetWindow( window );

    win_w = glutGet( GLUT_WINDOW_WIDTH );
    win_h = glutGet( GLUT_WINDOW_HEIGHT );
    
    /*need_w = 16 * glutBitmapWidth( fid, 'M' ); */
    max_glyph_width = 0;
    for( i = 0; i < 256; ++i )
        if( max_glyph_width < glutBitmapWidth( fid, i ) )
            max_glyph_width = glutBitmapWidth( fid, i );
    need_w = 16 * max_glyph_width;
    need_h = 16 * glutBitmapHeight( fid );
    if( ( need_w > win_w ) ||
        ( need_h > win_h ) )
    {
        if( need_w > win_w )
            win_w = need_w;
        if( need_h > win_h )
            win_h = need_h;
        glutReshapeWindow( win_w + 1, win_h + 1 );
    }
    glutPostRedisplay( );
    glutSetWindow( menu_win );
}

int main (int argc, char **argv)
{
    unsigned int i;

    glutInitWindowSize( 130, 200 );
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE );
    window = glutCreateWindow( "OpenGLUT Font Demo" );
    glutReshapeFunc( cb_reshape );
    glutDisplayFunc( cb_display );
    glutKeyboardFunc( cb_keyboard );

    menu = glutCreateMenu( cb_menu );
    for( i = 0; i < NUMBEROF( font_map ); ++i )
        glutAddMenuEntry( font_map[ i ].name, i );
    glutAttachMenu( 2 );

    glutMainLoop( );
    return 0;
}
