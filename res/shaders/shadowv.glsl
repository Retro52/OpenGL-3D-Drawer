#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;

uniform vec3 lightDir;


void main()
{

    const float offset = 0.00067f;

    vec3 lighrDirection = normalize(-lightDir);
    float cosAngle = clamp(1.0f - dot(lighrDirection, aNormal), 0.0, 1.0);

    vec3 scaledNormalOffset = aNormal * (offset * cosAngle);
    gl_Position = model * vec4(aPos + scaledNormalOffset, 1.0);
}