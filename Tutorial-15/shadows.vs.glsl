#version 330 core

layout(location=0) in vec3 position;
layout(location=2) in vec3 normal;

uniform mat4 MVP; //modelview projection matrix
uniform mat4 MV;  //modelview matrix
uniform mat4 M;   //model matrix
uniform mat3 N;   //normal matrix
uniform mat4 S;   //shadow matrix

out vec3 NORMAL;
out vec3 POSITION;
out vec4 SHADOW;

void main()
{
	 POSITION = (MV * vec4(position, 1.0)).xyz;
	 NORMAL   = N * normal;
	 SHADOW   = S * (M * vec4(position, 1.0));
	 gl_Position = MVP * vec4(position, 1.0);
}
