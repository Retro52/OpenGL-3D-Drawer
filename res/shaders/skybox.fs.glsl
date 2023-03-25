#version 330 core

layout (location = 2) out vec4 gAlbedoSpec;

in vec3 FragPos;
in vec3 TexCoords;

uniform samplerCube skybox;

void main()
{
    // TODO: add an ability to replace skybox ith the color
    gAlbedoSpec = vec4(texture(skybox, TexCoords).rgb, -1.0);
}