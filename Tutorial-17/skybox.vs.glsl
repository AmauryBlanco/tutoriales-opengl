#version 330 core

layout (location = 0) in vec4 position;
uniform mat4 MVP, MV;
out vec3 uv;

void main()
{
	gl_Position = MVP * position;
	uv = position.xyz;
}