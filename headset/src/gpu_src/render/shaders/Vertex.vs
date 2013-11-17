
/// @brief MVP passed from app
uniform mat4 MVP;
// first attribute the vertex values from our VAO
attribute vec3 inVert;
// second attribute the UV values from our VAO
attribute vec2 inUV;
// we use this to pass the UV values to the frag shader
varying vec2 vertUV;

void main(void)
{
// pre-calculate for speed we will use this a lot

// calculate the vertex position
gl_Position = MVP*vec4(inVert, 1.0);
// pass the UV values to the frag shader
vertUV=inUV;
}
