#version 460 core

out vec4 FragColor;

in mat3 TBN;
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;
in vec4 FragPosLight;

struct DirLight
{
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
	sampler2D texture_normal1;
	sampler2D texture_height1;
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	sampler2DArray texture_shadow;

	vec4 colorDiffuse;
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



vec3 CalcLight(DirLight dirLight, PointLight pointLights[16], vec3 normal, vec3 fragPos, vec3 viewDir);

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 FragPos);

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
	vec3 result;
	vec3 norm;
	vec3 viewDir;
	float gamma = 1.0f;
	/* Unlit */
	if (drawMode == 2 || drawMode == 4)
	{
		result = texture(material.texture_diffuse1, TexCoords).rgb;
	}
	else if(drawMode == 7)
	{
		result = texture(material.texture_specular1, TexCoords).rgb;
	}
	else if (drawMode == 8)
	{
		result = texture(material.texture_normal1, TexCoords).rgb;
	}
	else
	{
		norm = normalize(texture( material.texture_normal1, TexCoords ).rgb * 2.0 - 1.0);
		viewDir = normalize(TBN * ProjPos - TBN * FragPos);
		result = CalcLight(dirLight, pointLights, norm, FragPos, viewDir);
	}
	if (drawMode == 9)
	{
		gamma = 2.2f;
	}
	FragColor = vec4(result, 1.0f);
	FragColor.rgb = pow(FragColor.rgb, vec3(1.0/gamma));
}

vec3 CalcLight(DirLight dirLight, PointLight pointLights[16], vec3 norm, vec3 FragPos, vec3 viewDir)
{
	vec3 result;
	if (drawMode != 6)
	{
		// calculating directional light impact
		result += CalcDirLight(dirLight, norm, viewDir, FragPos);
	}

	if (drawMode != 5)
	{
		// calculating point lights impact
		for(int i = 0; i < pointLightsCount; i++)
		{
			result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
		}
	}
	// returning result
	return result;
}

float ShadowCalculation(vec3 lightDir, vec3 nnormal)
{
	float shadow = 0.0;
	float ambientShadow = 0.7;
	if (dot(Normal, lightDir) <= 0)
	{
		shadow = 9 * ambientShadow * (1 + dot(Normal, lightDir));
		// Get average shadow
		shadow /= 9.0f;

	}
	else
	{
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

		vec4 fragPosLightSpace = lightSpaceMatrices[layer] * vec4(FragPos, 1.0);
		// perform perspective divide
		vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
		// transform to [0,1] range
		projCoords = projCoords * 0.5 + 0.5;

		// get depth of current fragment from light's perspective
		float currentDepth = projCoords.z;

		// keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
		if (currentDepth > 1.0)
		{
			return 0.0;
		}
		// calculate bias (based on depth map resolution and slope)
		vec3 normal = normalize(nnormal);
		float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.0005);
		const float biasModifier = 0.3f;
		if (layer == cascadeCount)
		{
			bias *= 10 / (farPlane * biasModifier);
		}
		else
		{
			bias *= 1 / (cascadePlaneDistances[layer] * biasModifier);
		}

		// PCF
		vec2 texelSize = 1.0 / vec2(textureSize(material.texture_shadow, 0));
		for(int x = -1; x <= 1; ++x)
		{
			for(int y = -1; y <= 1; ++y)
			{
				float pcfDepth = texture(material.texture_shadow, vec3(projCoords.xy + vec2(x, y) * texelSize, layer)).r;
				shadow += (currentDepth - bias) > pcfDepth ? ambientShadow : 0.0;
			}
		}
		shadow /= 9.0;
	}

	return shadow;
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 FragPos)
{
	vec3 lightDir = normalize(-light.direction);

	float shadow = ShadowCalculation(lightDir, normal);

	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);

	// specular shading
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

	// combine results
	vec3 ambient  = light.ambient;
	vec3 diffuse  = light.diffuse * diff;
	vec3 specular = light.specular * spec;

	if (drawMode == 3)
	{
		ambient = ambient * vec3(1.0f, 1.0f, 1.0f);
		diffuse = diffuse * vec3(1.0f, 1.0f, 1.0f);

		return (ambient * (1.0f - shadow) + diffuse * (1.0f - shadow));
	}
	else
	{
		specular = specular * vec3(texture(material.texture_specular1, TexCoords));
		return ((diffuse * (1.0f - shadow) + ambient * (1.0f - shadow)) * vec3(texture(material.texture_diffuse1, TexCoords)) + specular * (1.0f - shadow));
	}
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir;
	lightDir = normalize(light.position - FragPos);

	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);

	// specular shading
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);

	// attenuation
	float distance    = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	// combine results
	vec3 ambient  = light.ambient;
	vec3 diffuse  = light.diffuse * diff;
	vec3 specular = light.specular * spec;

	if (drawMode == 3)
	{
		ambient = ambient * vec3(1.0f, 1.0f, 1.0f);
		diffuse = diffuse * vec3(1.0f, 1.0f, 1.0f);
		ambient  *= attenuation;
		diffuse  *= attenuation;
		return (ambient + diffuse);
	}
	else
	{
		ambient = ambient * vec3(texture(material.texture_diffuse1, TexCoords));
		diffuse = diffuse * vec3(texture(material.texture_diffuse1, TexCoords));
		specular = specular * vec3(texture(material.texture_specular1, TexCoords));
	}

	ambient  *= attenuation;
	diffuse  *= attenuation;
	specular *= attenuation;
	return (ambient + diffuse + specular);
}