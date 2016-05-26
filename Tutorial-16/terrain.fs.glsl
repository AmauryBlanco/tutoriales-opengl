#version 330 core

out vec4 color;

in vec3 POS, NORMAL;
in vec2 UV;

uniform vec3 light_position ;
uniform vec3 diffuse_color;
uniform sampler2D terrain;

void main() {
	vec3 N = normalize(NORMAL);
	vec3 L = normalize(light_position - POS);

	float diffuse = max(0, dot(N, L));
	
	//color = diffuse * vec4(diffuse_color, 1.0);
	color = diffuse * texture(terrain, UV);
	//color = vec4(1.0);
}