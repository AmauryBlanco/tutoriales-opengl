#version 330 core

out vec4 color;

in vec3 V1;
in vec2 UV;
in vec3 T, B, N;

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
	sampler2D normal;
	float shininess;
	float shininess_strength;
};

uniform Light light;
uniform Material material;

uniform bool texture_off = false;
uniform bool normal_off = false;

void main()
{
	vec3 normalT = texture(material.normal, UV).rgb;
		 normalT = normalT * 2.0 - vec3(1.0);

	mat3 TBN = mat3(normalize(T), normalize(B), normalize(N));

	vec3 NT = normal_off ? N : TBN * normalT;
		 NT = normalize(NT);

	vec3 L = normalize(light.direction);
	vec3 V = normalize(V1);
	vec3 H = normalize(L + V);	

	float diff = max(dot(NT, L), 0);
	float spec = pow(max(dot(NT, H), 0), material.shininess) * material.shininess_strength;

	vec3 texture_ambient  = texture_off ? vec3(0.10) : texture(material.ambient , UV).rgb;
	vec3 texture_diffuse  = texture_off ? vec3(0.64) : texture(material.diffuse , UV).rgb;
	vec3 texture_specular = texture_off ? vec3(0.50) : texture(material.specular, UV).rgb;

	vec3 ambient  = texture_ambient  * light.ambient;
	vec3 diffuse  = texture_diffuse  * light.diffuse  * diff;
	vec3 specular = texture_specular * light.specular * spec;

	color = vec4(ambient + diffuse + specular, 1.0);
} 