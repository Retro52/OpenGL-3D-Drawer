#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 FragPos;
out vec3 TexCoords;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoords = aPos;
    vec4 pos = projection * view * vec4(aPos, 0.0);
    gl_Position = pos;
}