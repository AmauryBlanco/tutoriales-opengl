#version 330 core

out vec4 color;

in vec3 N1;
in vec3 V1;
in vec2 UV;

struct Light {
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct Material {
	sampler2D diffuse;
	sampler2D specular;	
	sampler2D ambient;
	float shininess;
	float shininess_strength;
};

uniform mat4 mv_matrix;

uniform Light light;
uniform Material material;

void main()
{
		vec3 N = normalize(N1);
		vec3 L = normalize(light.direction);
		vec3 V = normalize(V1);
		vec3 H = normalize(L + V);	

		float diff = max(dot(N, L), 0);
		float spec = pow(max(dot(N, H), 0), material.shininess) * material.shininess_strength;

		vec3 ambient  = texture(material.ambient , UV).rgb * light.ambient;
		vec3 diffuse  = texture(material.diffuse , UV).rgb * light.diffuse  * diff;
		vec3 specular = texture(material.specular, UV).rgb * light.specular * spec;

		color = vec4(ambient + diffuse + specular, 1.0);
} 