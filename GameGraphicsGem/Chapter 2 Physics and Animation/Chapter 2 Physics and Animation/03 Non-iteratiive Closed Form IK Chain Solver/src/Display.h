#pragma once

#include <GLVector.h>
#include "IKChain.h"

using mathglpp::GLVector3f;

extern void DisplayHelp();			

void DrawChain( );
void DrawBone( int index, bool picking = false );
void DrawCross( GLVector3f position, float size );

void DrawLine( GLVector3f startPosition,  GLVector3f endPosition );
void DrawDashedLine( GLVector3f startPosition,  GLVector3f endPosition );
void DrawDashedArc( GLVector3f position, GLVector3f axis, GLVector3f baseVector, float startAngle , float endAngle );
