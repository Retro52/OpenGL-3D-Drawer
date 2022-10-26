#version 460 core

precision highp float;

out vec4 FragColor;

in mat3 TBN;
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;
in vec4 FragPosLight;

struct DirLight
{
	bool isPresent;

	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct PointLight
{
	vec3 position;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct Material
{
	bool shouldBeLit;
	bool hasNormalTexture;
	bool hasDiffuseTexture;
	bool hasTranslucencyTexture;

	int tilingFactor;

	float opacity;

	sampler2D mapNormal_1;
	sampler2D mapDiffuse_1;
	sampler2D mapSpecular_1;
	sampler2D mapRoughness_1;
	sampler2D mapTranslucency_1;

	sampler2DArray mapShadow;

	vec3 colorDiffuse;
};

uniform int drawMode;
uniform int cascadeCount;
uniform int pointLightsCount;

uniform float farPlane;
uniform float cascadePlaneDistances[16];

uniform vec3 ProjPos;

uniform mat4 view;

uniform Material material;
uniform DirLight dirLight;
uniform PointLight pointLights[16];

layout (std140, binding = 0) uniform LightSpaceMatrices
{
	mat4 lightSpaceMatrices[16];
};

float CalculateDirecionalShadowFactor(vec3 lightDir, vec3 normal, vec3 viewDir);

vec3 CalculateDirectionalDiffuseLighting(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalculateDirectionalAmbientLighting(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalculateDirectionalSpecularLighting(DirLight light, vec3 normal, vec3 viewDir);

vec3 CalculatePointDiffuseLighting(PointLight light, vec3 normal, vec3 viewDir);
vec3 CalculatePointAmbientLighting(PointLight light, vec3 normal, vec3 viewDir);
vec3 CalculatePointSpecularLighting(PointLight light, vec3 normal, vec3 viewDir);

float rand(vec2 co)
{
	return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

/*
 * drawModes
 * 0 - RESERVED
 * 1 - Default lighting
 * 2 - Lighting only
 * 3 - Directional lights only
 * 4 - Point lights only
 * 5 - Diffuse map/color view
 * 6 - Normal map view
 * 7 - Specular map view
 * 8 - Shadow map debug view
 * 9 - Roughness map view
*/
void main()
{
	vec3 finalColor;
	vec3 normalVector;
	vec3 viewDirection = normalize((ProjPos - FragPos));
	vec2 textureCoordinates = TexCoords * material.tilingFactor;

	if(drawMode == 2)
	{
		finalColor = vec3(1.0);
	}
	else if(material.hasDiffuseTexture)
	{
		finalColor = vec3(texture(material.mapDiffuse_1, textureCoordinates));
	}
	else
	{
		finalColor = material.colorDiffuse.rgb;
	}

	if(drawMode == 5)
	{
		FragColor = vec4(finalColor, material.opacity);
		return;
	}
	if(drawMode == 6)
	{
		FragColor = vec4(texture(material.mapNormal_1, textureCoordinates).rgb, 1);
		return;
	}
	if(drawMode == 7)
	{
		FragColor = vec4(texture(material.mapSpecular_1, textureCoordinates).rgb, 1);
		return;
	}

	if(material.hasNormalTexture)
	{
		normalVector = normalize((texture( material.mapNormal_1, textureCoordinates).rgb * 2.0 - 1.0) * TBN);
	}
	else
	{
		normalVector = Normal;
	}
	if(drawMode == 8)
	{
		if (!dirLight.isPresent)
		{
			FragColor = vec4(0, 0, 1, 1);
			return;
		}
		float shadowFactor = CalculateDirecionalShadowFactor(normalize(-dirLight.direction), normalVector, viewDirection);
		float r = mix(0, 1, shadowFactor);
		float g = mix(1, 0, shadowFactor);
		FragColor = vec4(r, g, 0, 1);
		return;
	}

	if(drawMode == 9)
	{
		FragColor = vec4(texture(material.mapRoughness_1, textureCoordinates).rgb, 1);
		return;
	}
	if(!material.shouldBeLit)
	{
		FragColor = vec4(finalColor, material.opacity);
		return;
	}

	float shadowFactor;
	vec3 diffuseLighting;
	vec3 ambientLighting;
	vec3 specularLighting;

	if(drawMode != 4 && dirLight.isPresent)
	{
		shadowFactor = CalculateDirecionalShadowFactor(normalize(-dirLight.direction), normalVector, viewDirection);
		diffuseLighting  += CalculateDirectionalDiffuseLighting(dirLight, normalVector, viewDirection) * (1.0 - shadowFactor);
		specularLighting += CalculateDirectionalSpecularLighting(dirLight, normalVector, viewDirection) * (1.0 - shadowFactor);
		ambientLighting  += CalculateDirectionalAmbientLighting(dirLight, normalVector, viewDirection);
	}
	if(drawMode != 3)
	{
		for(int i = 0; i < pointLightsCount; i++)
		{
			diffuseLighting  += CalculatePointDiffuseLighting(pointLights[i], normalVector, viewDirection);
			ambientLighting  += CalculatePointAmbientLighting(pointLights[i], normalVector, viewDirection);
			specularLighting += CalculatePointSpecularLighting(pointLights[i], normalVector, viewDirection);
		}
	}

	finalColor = (diffuseLighting + ambientLighting) * finalColor + specularLighting * texture(material.mapSpecular_1, textureCoordinates).rgb;
	FragColor = vec4(finalColor, material.opacity);
}


vec3 CalculateDirectionalDiffuseLighting(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);

	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	return light.diffuse * diff + light.ambient;
}

vec3 CalculateDirectionalAmbientLighting(DirLight light, vec3 normal, vec3 viewDir)
{
	return light.ambient;
}

vec3 CalculateDirectionalSpecularLighting(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);

	// specular shading
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
	return light.specular * spec;
}

vec3 CalculatePointDiffuseLighting(PointLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - FragPos);

	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);

	// attenuation
	float distance    = length(light.position - FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	return light.diffuse * diff * attenuation;
}

vec3 CalculatePointAmbientLighting(PointLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - FragPos);

	// attenuation
	float distance    = length(light.position - FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	return light.ambient * attenuation;
}

vec3 CalculatePointSpecularLighting(PointLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - FragPos);
	// specular shading
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

	float distance    = length(light.position - FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	return light.specular * spec * attenuation;
}

float CalculateDirecionalShadowFactor(vec3 lightDir, vec3 normal, vec3 viewDir)
{
	float shadow = 0.0f;

	// select cascade layer
	vec4 fragPosViewSpace = view * vec4(FragPos, 1.0);
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
		layer = cascadeCount;
	}

	vec4 fragPosLightSpace = lightSpaceMatrices[layer]  * vec4(FragPos, 1.0);
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
	vec2 texelSize = 1.0f / vec2(textureSize(material.mapShadow, 0));

	const int sampleRadius = 3;
	const float sampleRadiusCount = pow(sampleRadius * 2 + 1, 2);

	for(int x = -sampleRadius; x <= sampleRadius; ++x)
	{
		for(int y = -sampleRadius; y <= sampleRadius; ++y)
		{
            float randomFactor = clamp(mix(0.0f, 1.0f, rand((projCoords.xy + vec2(x, y)) * depthValue * FragPos.xy)), 0.0f, 1.0f);
            float pcfDepth = texture(material.mapShadow, vec3(projCoords.xy + vec2(x + randomFactor, y + randomFactor) * texelSize, layer)).r;
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
