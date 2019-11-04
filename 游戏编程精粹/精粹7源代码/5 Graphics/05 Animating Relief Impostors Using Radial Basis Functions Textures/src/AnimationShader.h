/*
    Imagedit - Demo for Animating Relief Impostors using Radial Basis Functions Textures
    Copyright (C) 2007 Vitor Fernando Pamplona (vitor@vitorpamplona.com)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ANIMATIONSHADER_H_
#define ANIMATIONSHADER_H_

class AnimationShader {
	
protected: 	
	CGprofile cg_vs_profile;
	CGprofile cg_ps_profile;

	CGprogram cg_ps_program;
	CGprogram cg_vs_program;
		
	CGparameter modelViewProj_parameter;
	CGparameter modelView_parameter;
	CGparameter modelViewIT_parameter;
	
	CGparameter modelViewPS_parameter;	
	CGparameter rbfTexture_parameter;
	CGparameter texture_parameter;
	CGparameter normal_parameter;
	CGparameter shininess_parameter;
	CGparameter keyTime_parameter;
	CGparameter points_parameter;
	CGparameter worldLightPosition_parameter;	
	CGparameter suavization_parameter;
	
	char * vsFilename; 
	char * psFilename;
	
	virtual void initParameters() {
		modelViewProj_parameter = cgGetNamedParameter(cg_vs_program, "modelViewProj");
		modelView_parameter = cgGetNamedParameter(cg_vs_program, "modelView");
		modelViewIT_parameter = cgGetNamedParameter(cg_vs_program, "modelViewIT");
		
		modelViewPS_parameter		= cgGetNamedParameter(cg_ps_program, "modelViewPS"); 	
		rbfTexture_parameter		= cgGetNamedParameter(cg_ps_program, "rbfTexture");
		texture_parameter 			= cgGetNamedParameter(cg_ps_program, "texture");
		normal_parameter 			= cgGetNamedParameter(cg_ps_program, "normal");
		keyTime_parameter 			= cgGetNamedParameter(cg_ps_program, "keyTime");
		points_parameter 			= cgGetNamedParameter(cg_ps_program, "points");
		shininess_parameter 		= cgGetNamedParameter(cg_ps_program, "shininess"); 
		worldLightPosition_parameter= cgGetNamedParameter(cg_ps_program, "lightInViewSpace");
		suavization_parameter 		= cgGetNamedParameter(cg_ps_program, "suavization");
	}	
	
	virtual void enableParameters() {
		cgGLEnableTextureParameter(texture_parameter);
		cgGLEnableTextureParameter(normal_parameter);
		cgGLEnableTextureParameter(rbfTexture_parameter);
		
		glEnable( GL_TEXTURE_RECTANGLE_ARB );
	}	
	
	virtual void disableParameters() {
		glDisable( GL_TEXTURE_RECTANGLE_ARB );
		cgGLDisableTextureParameter(texture_parameter);
		cgGLDisableTextureParameter(rbfTexture_parameter);
		cgGLDisableTextureParameter(normal_parameter);
	}	
public:
	CGcontext cg_context;
	char * name;

	AnimationShader(char * _name,  char * _vsFilename, char * _psFilename) {
		vsFilename = _vsFilename; 
		psFilename = _psFilename;
		name = _name;
	}
	virtual ~AnimationShader() {
	}
	
	virtual void initShader() {
		cg_context = cgCreateContext();

		cg_vs_profile = cgGLGetLatestProfile(CG_GL_VERTEX);
		cgGLSetOptimalOptions(cg_vs_profile);
	
		cg_ps_profile = cgGLGetLatestProfile(CG_GL_FRAGMENT);
		cgGLSetOptimalOptions(cg_ps_profile);

		//cg_vs_profile = CG_PROFILE_VP40;
		//cg_ps_profile = CG_PROFILE_FP40;
		
		// RELIEF OR NORMAL MAP
		cg_vs_program = cgCreateProgramFromFile(cg_context, CG_SOURCE, vsFilename, cg_vs_profile, "main", 0);
		cg_ps_program = cgCreateProgramFromFile(cg_context, CG_SOURCE, psFilename, cg_ps_profile, "main", 0);
		
		initParameters();
		
		cgGLLoadProgram(cg_vs_program);
		cgGLLoadProgram(cg_ps_program);
	}
	
	virtual void setParameters(float animationTime, float numberOfPoints, float suavization, float shininess,
							   float * light, bool hasRBF, GLuint RBFTextureName, GLuint  colorTextureName, GLuint normalTextureName) {
		enableParameters();							   	
							   	
		cgGLSetParameter1f(keyTime_parameter, animationTime);
		cgGLSetParameter1f(points_parameter, numberOfPoints);
		cgGLSetParameter1f(suavization_parameter, suavization);
		cgGLSetParameter1f(shininess_parameter, shininess);

		cgGLSetParameter4fv(worldLightPosition_parameter, light);

		cgGLSetStateMatrixParameter(modelViewProj_parameter, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY);
		cgGLSetStateMatrixParameter(modelView_parameter,     CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
		cgGLSetStateMatrixParameter(modelViewIT_parameter,   CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_INVERSE_TRANSPOSE);
		cgGLSetStateMatrixParameter(modelViewPS_parameter,   CG_GL_MODELVIEW_MATRIX, CG_GL_MATRIX_IDENTITY);
		
		if (hasRBF) {
			cgGLSetTextureParameter(rbfTexture_parameter,  RBFTextureName);
		}
		
		cgGLSetTextureParameter(texture_parameter,  colorTextureName);
		cgGLSetTextureParameter(normal_parameter,  normalTextureName);
	}
	
	virtual void enableShader() {
		cgGLEnableProfile(cg_vs_profile);
		cgGLEnableProfile(cg_ps_profile);

		cgGLBindProgram(cg_vs_program);
		cgGLBindProgram(cg_ps_program);
	}
	
	virtual void disableShader() {
		disableParameters();

		cgGLDisableProfile(cg_vs_profile);
		cgGLDisableProfile(cg_ps_profile);
	}	
	
};

#endif /*SHADER_H_*/
