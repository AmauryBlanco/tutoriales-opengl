#version 330 core

out vec4 color;

uniform sampler2DShadow shadowMap;

uniform vec3 light_position;
uniform vec3 diffuse_color;

in vec3 NORMAL;
in vec3 POSITION;
in vec4 SHADOW;

void main() {
	vec3 L = normalize(light_position - POSITION);

	float diffuse = max(0, dot(NORMAL, L));
	
	if(SHADOW.w > 1) {
		 float shadow = textureProj(shadowMap, SHADOW);
		 diffuse = mix(diffuse, diffuse * shadow, 0.5);
	} 
	
	color = diffuse * vec4(diffuse_color, 1);
}