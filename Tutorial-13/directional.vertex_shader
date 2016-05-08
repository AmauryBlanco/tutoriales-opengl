#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texture;
layout (location = 2) in vec3 normal;

uniform mat4 mvp_matrix;
uniform mat4 mv_matrix;
uniform mat3 n_matrix;

out vec3 N1;
out vec3 V1;
out vec2 UV;

void main(void)
{
	N1 = n_matrix * normal;
	V1 = -(mv_matrix * vec4(position, 1)).xyz;
	UV = texture;

	gl_Position = mvp_matrix * vec4(position, 1.0);
}