#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

uniform mat4 MVP, MV;
uniform mat3 N;
uniform sampler2D heightMap;

float scale_size = 100.0;
float scale_height = 10.8;

out vec3 POS, NORMAL;
out vec2 UV;

out Vertex
{
	vec3 normal;
} vertex;

void main()
{
	vec2 pos = position.xz * scale_size;
	float height = position.y * scale_height;

	vec4 vertex_position = vec4(pos.x, height, pos.y, 1.0);

	gl_Position = MVP * vertex_position;

	POS = (MV * vertex_position).xyz;
	NORMAL = N * normal;
	UV = position.xz * 0.5 + 0.5;
}
