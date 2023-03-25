#version 460 core

precision highp float;

out vec4 FragColor;

in vec2 TexCoords;

struct DirectionalLight
{
    bool isPresent;

    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    sampler2DArray mapShadow;
};

struct PointLight
{
    vec3 position;
    float constant;

    vec3 ambient;
    float linear;
    vec3 diffuse;
    float quadratic;

    vec3 specular;
};

uniform sampler2D gNormal; // 2 active texture
uniform sampler2D gPosition; // 1 active texture
uniform sampler2D gAlbedoSpec; // 3 active texture
uniform sampler2D gMetallic; // 4 active texture
uniform sampler2D gRoughness; // 5 active texture


// TODO: bind pointligts using SSBO or UBO
// SSBO lets us to use up to 16RB of space, while SSBO can be as large as 128MB. For point ligts purpose, while 1 structure takes 4 * 15 = 60 bytes of space, a size of 16RB can let us bind up to 273 point lights per time, SSBO - over 2 million
const int MaxPointLightAmount = 16;

uniform int pointLightsCount = 0;
uniform PointLight pointLights[MaxPointLightAmount];

uniform DirectionalLight dLight;

uniform int drawMode;
uniform int cascadeCount;

uniform float farPlane;
uniform float cascadePlaneDistances[16];

uniform mat4 view;
uniform vec3 ProjPos;

layout (std140, binding = 0) uniform LightSpaceMatrices
{
    mat4 lightSpaceMatrices[16];
};

float CalculateDirecionalShadowFactor(vec3 lightDir, vec3 normal, vec3 viewDir, vec3 fragPos);

vec3 CalculateDirectionalDiffuseLighting(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 CalculateDirectionalAmbientLighting(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 CalculateDirectionalSpecularLighting(DirectionalLight light, vec3 normal, vec3 viewDir);

vec3 CalculatePointDiffuseLighting(PointLight light, vec3 normal, vec3 viewDir, vec3 fragPos);
vec3 CalculatePointAmbientLighting(PointLight light, vec3 normal, vec3 viewDir, vec3 fragPos);
vec3 CalculatePointSpecularLighting(PointLight light, vec3 normal, vec3 viewDir, vec3 fragPos);

float rand(vec2 co)
{
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

void main()
{
    FragColor = vec4(texture(dLight.mapShadow, vec3(TexCoords, 0)).r, 1.0 - texture(dLight.mapShadow, vec3(TexCoords, 0)).r, 0.0, 1.0);
//    return;

    // retrieve data from gbuffer
    vec3 fragPosition = texture(gPosition, TexCoords).rgb;
    vec3 normalVector = texture(gNormal, TexCoords).rgb;
    vec3 diffuseColor = texture(gAlbedoSpec, TexCoords).rgb;
    vec3 viewDirection = normalize((ProjPos - fragPosition));

    float specularFactor = texture(gAlbedoSpec, TexCoords).a;
    float metallicFactor = texture(gMetallic, TexCoords).r;
    float roughnessFactor = texture(gRoughness, TexCoords).r;

    if(specularFactor == -1.0)
    {
        FragColor = vec4(diffuseColor, 1.0);
        return;
    }

    float shadowFactor = 0.0f;
    vec3 diffuseLighting  = vec3(0.0, 0.0, 0.0);
    vec3 ambientLighting  = vec3(0.0, 0.0, 0.0);
    vec3 specularLighting = vec3(0.0, 0.0, 0.0);

    if (dLight.isPresent)
    {
        shadowFactor = CalculateDirecionalShadowFactor(normalize(-dLight.direction), normalVector, viewDirection, fragPosition);
        ambientLighting  += CalculateDirectionalAmbientLighting(dLight, normalVector, viewDirection);
        diffuseLighting  += CalculateDirectionalDiffuseLighting(dLight, normalVector, viewDirection)  * clamp(1.0 - shadowFactor, 0.0, 1.0);
        specularLighting += CalculateDirectionalSpecularLighting(dLight, normalVector, viewDirection) * clamp(1.0 - shadowFactor, 0.0, 1.0);
    }

    for(int i = 0; i < (pointLightsCount > MaxPointLightAmount ? MaxPointLightAmount : pointLightsCount); i++)
    {
        diffuseLighting  += CalculatePointDiffuseLighting(pointLights[i], normalVector, viewDirection , fragPosition);
        ambientLighting  += CalculatePointAmbientLighting(pointLights[i], normalVector, viewDirection , fragPosition);
        specularLighting += CalculatePointSpecularLighting(pointLights[i], normalVector, viewDirection, fragPosition);
    }

    vec3 finalColor = (diffuseLighting + ambientLighting + specularLighting * specularFactor) * diffuseColor;
    if (drawMode == 1)
    {
        FragColor = vec4(finalColor, 1.0);
    }
    else if (drawMode == 2)
    {
        FragColor = vec4(normalVector, 1.0);
    }
    else if (drawMode == 3)
    {
        FragColor = vec4(diffuseColor, 1.0);
    }
    else if (drawMode == 3)
    {
        FragColor = vec4(viewDirection, 1.0);
    }
    else if (drawMode == 5)
    {
        FragColor = vec4(0.0 + shadowFactor, 1.0 - shadowFactor, 0.1, 1.0);
    }
}

vec3 CalculateDirectionalDiffuseLighting(DirectionalLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    return light.diffuse * diff;
}

vec3 CalculateDirectionalAmbientLighting(DirectionalLight light, vec3 normal, vec3 viewDir)
{
    return light.ambient;
}

vec3 CalculateDirectionalSpecularLighting(DirectionalLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);

    // specular shading
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(halfwayDir, normal), 0.0), 32.0);

    return light.specular * spec;
}

vec3 CalculatePointDiffuseLighting(PointLight light, vec3 normal, vec3 viewDir, vec3 fragPos)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    return light.diffuse * diff * attenuation;
}

vec3 CalculatePointAmbientLighting(PointLight light, vec3 normal, vec3 viewDir, vec3 fragPos)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    return light.ambient * attenuation;
}

vec3 CalculatePointSpecularLighting(PointLight light, vec3 normal, vec3 viewDir, vec3 fragPos)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // specular shading
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    return light.specular * spec * attenuation;
}

float CalculateDirecionalShadowFactor(vec3 lightDir, vec3 normal, vec3 viewDir, vec3 fragPos)
{
    float shadow = 0.0f;

    // select cascade layer
    vec4 fragPosViewSpace = view * vec4(fragPos, 1.0);
    float depthValue = abs(fragPosViewSpace.z);

    int layer = -1;
    for (int i = 0; i < cascadeCount; ++i)
    {
        if (depthValue < cascadePlaneDistances[i])
        {
            layer = i;
            break;
        }
    }
    if (layer == -1)
    {
        layer = cascadeCount - 1;
    }

    vec4 fragPosLightSpace = lightSpaceMatrices[layer]  * vec4(fragPos, 1.0);
    vec3 projCoords        = fragPosLightSpace.xyz / fragPosLightSpace.w;

    // transform to [0,1] range
    projCoords = projCoords * 0.5f + 0.5f;

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    if (currentDepth > 1.0)
    {
        return 0.0;
    }

    // PCF
    vec2 texelSize = 1.0f / vec2(textureSize(dLight.mapShadow, 0));

    const int sampleRadius = 3;
    const float sampleRadiusCount = pow(sampleRadius * 2 + 1, 2);

    for(int x = -sampleRadius; x <= sampleRadius; ++x)
    {
        for(int y = -sampleRadius; y <= sampleRadius; ++y)
        {
            float randomFactor = clamp(mix(0.0f, 1.0f, rand((projCoords.xy + vec2(x, y))* fragPos.xy)), 0.0f, 1.0f);
            float pcfDepth = texture(dLight.mapShadow, vec3(projCoords.xy + vec2(x + randomFactor, y + randomFactor) * texelSize, layer)).r;
            shadow += currentDepth > pcfDepth ? 1.0f : 0.0f;
        }
    }

    shadow /= sampleRadiusCount;

    const float shadowFadeDistance = 10.0f;
    float delta = cascadePlaneDistances[layer] - depthValue;

    if (layer == cascadeCount - 1 && delta < shadowFadeDistance)
    {
        shadow *= delta / shadowFadeDistance;
    }
    return shadow;
}