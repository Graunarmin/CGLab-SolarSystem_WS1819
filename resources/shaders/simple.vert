#version 150
#extension GL_ARB_explicit_attrib_location : require
//gets called for every vertex and computes where this vertex is supposed to be
// vertex attributes of VAO
layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Normal;

//Matrix Uniforms as specified with glUniformMatrix4fv
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 NormalMatrix;
uniform vec3 PlanetColor;

//those get passed to fragment shader
out vec3 pass_Normal; 
out vec3 planet_color;
out vec3 light_pos;
out vec3 fragment_pos;
out vec3 camera_pos;

void main(void)
{
	gl_Position = (ProjectionMatrix  * ViewMatrix * ModelMatrix) * vec4(in_Position, 1.0);
	pass_Normal = (NormalMatrix * vec4(in_Normal, 0.0)).xyz;
	fragment_pos = (ModelMatrix * vec4(in_Position, 1.0)).xyz;
	//invert viewMatrix, otherwise the planets look like bumble-bees in cel shading
	camera_pos = (inverse(ViewMatrix) * vec4(fragment_pos,1.0)).xyz; 
	planet_color = PlanetColor;
}
