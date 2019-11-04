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

#ifndef ANIMATIONRELIEFMAPSHADER_H_
#define ANIMATIONRELIEFMAPSHADER_H_

class AnimationReliefMapShader : public AnimationShader {
	
	CGparameter normal_map_x_parameter;
	CGparameter normal_map_y_parameter;
	
public:

	AnimationReliefMapShader(char * _name, char * _vsFilename, char * _psFilename) : AnimationShader(_name, _vsFilename, _psFilename) {
	}
	virtual ~AnimationReliefMapShader() {}
	
	virtual void enableParameters() {
		AnimationShader::enableParameters();
		cgGLEnableTextureParameter(normal_map_x_parameter);
		cgGLEnableTextureParameter(normal_map_y_parameter);
	}
	
	virtual void disableParameters() {
		AnimationShader::disableParameters();
		cgGLDisableTextureParameter(normal_map_x_parameter);
		cgGLDisableTextureParameter(normal_map_y_parameter);
	}
	
	virtual void setParameters(GLuint  layer12TextureName, GLuint layer34TextureName) {
		cgGLSetTextureParameter(normal_map_x_parameter,  layer12TextureName);
		cgGLSetTextureParameter(normal_map_y_parameter,  layer34TextureName);
	}
	
	virtual void initParameters() {
		AnimationShader::initParameters();
		normal_map_x_parameter 		= cgGetNamedParameter(cg_ps_program, "normal_map_x");
		normal_map_y_parameter 		= cgGetNamedParameter(cg_ps_program, "normal_map_y");
	}
	
};

#endif /*ANIMATIONRELIEFMAPSHADER_H_*/

