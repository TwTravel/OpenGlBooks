varying vec3 R;
attribute vec4 vPosition;
attribute vec4 Normal;

uniform mat4 ModelView;
uniform mat4 Projection;


void main()
{
    gl_Position = Projection*ModelView*vPosition;

    vec4 eyePos  = ModelView*vPosition;
    
    vec4 NN = ModelView*Normal;
    //vec4 NN = Normal;
    vec3 N =NN.xyz;
    R = reflect(eyePos.xyz, N);


}
