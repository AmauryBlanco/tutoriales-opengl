#version 330 core

out vec4 color;

uniform sampler2D depthMap;

in vec2 UV;

void main(void)
{
	float depth = texture(depthMap, UV).r;
	color = vec4(vec3(depth), 1.0);
}
