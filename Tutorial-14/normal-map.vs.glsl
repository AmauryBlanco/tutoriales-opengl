#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texture;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

uniform mat4 mvp_matrix;
uniform mat4 mv_matrix;
uniform mat3 n_matrix;

out vec3 V1;
out vec2 UV;
out vec3 T, B, N;

void main(void)
{
	vec4 posW  = mv_matrix * vec4(position, 1.0);

	T = n_matrix * tangent;
	B = n_matrix * bitangent;	
	N = n_matrix * normal;

	UV = texture;
	V1 = -posW.xyz;

	gl_Position = mvp_matrix * vec4(position, 1.0);
}