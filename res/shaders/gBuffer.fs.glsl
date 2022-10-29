#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

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
    bool hasTranslucencyTexture;

    int tilingFactor;

    float opacity;
    float specular;

    sampler2D mapNormal_1;
    sampler2D mapDiffuse_1;
    sampler2D mapSpecular_1;
    sampler2D mapRoughness_1;
    sampler2D mapTranslucency_1;

    vec3 colorDiffuse;
};

uniform Material material;

void main()
{
    vec2 texCoords = TexCoords * material.tilingFactor;

    // store the fragment position vector in the first gbuffer texture
    gPosition = FragPos;

    // also store the per-fragment normals into the gbuffer
    if(material.hasNormalTexture)
    {
        gNormal = normalize((texture( material.mapNormal_1, texCoords).rgb * 2.0 - 1.0) * TBN);
    }
    else
    {
        gNormal = normalize(Normal);
    }

    // store specular intensity in gAlbedoSpec's alpha component
    if(material.hasSpecularTexture)
    {
        gAlbedoSpec.a = texture(material.mapSpecular_1, texCoords).r;
    }
    else
    {
        gAlbedoSpec.a = material.specular;
    }

    // and the diffuse per-fragment color
    if(material.hasDiffuseTexture)
    {
        gAlbedoSpec.rgb = texture(material.mapDiffuse_1, texCoords).rgb;
    }
    else
    {
        gAlbedoSpec.rgb = material.colorDiffuse;
    }
}