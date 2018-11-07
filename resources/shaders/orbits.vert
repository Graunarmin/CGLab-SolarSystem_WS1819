#version 150
#extension GL_ARB_explicit_attrib_location : require
// glVertexAttribPointer mapped positions to first
layout(location = 0) in vec3 in_Position;

//Matrix Uniforms uploaded with glUniform*
uniform mat4 OrbitMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

void main() {
	// as in simple.vert (ModelMatrix = OrbitMatrix)
	gl_Position = (ProjectionMatrix * ViewMatrix * OrbitMatrix) * vec4(in_Position, 1.0);
}