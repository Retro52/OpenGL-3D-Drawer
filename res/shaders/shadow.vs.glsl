#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 model;
uniform vec3 lightDir;


void main()
{
    gl_Position = model * vec4(aPos - 0.005 * aNormal, 1.0);
}