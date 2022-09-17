#version 460 core

precision highp float;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out mat3 TBN;
out mat4 viewOut;
out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;
out vec4 FragPosLight;

uniform mat4 view;
uniform mat4 model;
uniform mat4 projection;
uniform mat4 lightProjection;

void main()
{
	FragPos      = vec3(model * vec4(aPos, 1.0));
	TexCoords    = aTexCoords;
	Normal       = mat3(transpose(inverse(model))) * aNormal;

	mat3 normalMatrix = transpose(inverse(mat3(model)));
	vec3 T = normalize(normalMatrix * aTangent);
	vec3 N = normalize(normalMatrix * aNormal);
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);
	TBN = transpose(mat3(T, B, N));

	viewOut = view;

	FragPosLight = lightProjection * vec4(FragPos, 1.0f);
	gl_Position  = projection * view * model * vec4(aPos, 1.0);
}