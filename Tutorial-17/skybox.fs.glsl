#version 330 core

out vec4 color;
uniform samplerCube cubeMap;
in vec3 uv;

void main()
{
	color = texture(cubeMap, uv);
}