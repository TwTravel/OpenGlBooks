
varying vec3 R;
uniform samplerCube texMap;

void main()
{
     vec4 texColor = textureCube(texMap, R);

    gl_FragColor = texColor;
}
