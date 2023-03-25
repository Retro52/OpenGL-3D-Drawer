#version 330 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;
layout (location = 3) out float gMetallic;
layout (location = 4) out float gRoughness;


// We might need more of this stuff here
in mat3 TBN;
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

struct Material
{
    bool shouldBeLit;
    bool hasNormalTexture;
    bool hasDiffuseTexture;
    bool hasSpecularTexture;
    bool hasRoughnessTexture;
    bool hasMetallicTexture;

    int tilingFactor;

    float specular;
    float metallic;
    float roughness;

    sampler2D mapNormal_1;
    sampler2D mapDiffuse_1;
    sampler2D mapSpecular_1;
    sampler2D mapMetallic_1;
    sampler2D mapRoughness_1;

    vec4 colorDiffuse;
};

uniform Material material;

void main()
{
    vec2 texCoords = TexCoords * material.tilingFactor;
    vec4 diffuseColor = material.hasDiffuseTexture ? texture(material.mapDiffuse_1, texCoords) : material.colorDiffuse;

    // support of masked textures
    if(diffuseColor.a < 1.0)
    {
        discard;
    }

    // store the fragment position vector in the first gbuffer texture, and diffuse color in the second one
    gPosition = FragPos;
    gAlbedoSpec.rgb = diffuseColor.rgb;

    // also store the per-fragment normals into the gbuffer
    gNormal = material.hasNormalTexture ? normalize((texture( material.mapNormal_1, texCoords).rgb * 2.0 - 1.0) * TBN) : normalize(Normal);

    if (!material.shouldBeLit)
    {
        gAlbedoSpec.a = -1.0;
    }
    else
    {
        //specular color is stored into alpha chanel
        gAlbedoSpec.a = material.hasSpecularTexture ? texture(material.mapSpecular_1, texCoords).r : material.specular;
    }

    gMetallic = material.hasMetallicTexture ? texture(material.mapMetallic_1, texCoords).r : material.metallic;
    gRoughness = material.hasRoughnessTexture ? texture(material.mapRoughness_1, texCoords).r : material.roughness;
}