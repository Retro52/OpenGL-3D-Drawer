#version 460 core

out vec4 FragColor;

in vec4 FragPosLight;
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;
in mat3 TBN;

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
	sampler2D texture_shadow;
};

uniform vec3 ProjPos;
uniform Material material;
uniform DirLight dirLight;
uniform int NR_POINT_LIGHTS;
uniform PointLight pointLights[16];
uniform int drawMode;

layout (std140, binding = 0) uniform LightSpaceMatrices
{
	mat4 lightSpaceMatrices[16];
};

uniform float cascadePlaneDistances[16];
uniform sampler2DArray shadowMap;
uniform int cascadeCount;   // number of frusta - 1
uniform float farPlane;


vec3 CalcLight(DirLight dirLight, PointLight pointLights[16], vec3 normal, vec3 fragPos, vec3 viewDir);

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 FragPos);

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
	vec3 result;
	vec3 norm;
	vec3 viewDir;

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
	else if (drawMode == 9)
	{
		result = texture(material.texture_shadow, TexCoords).rgb;
	}
	else
	{
		norm = normalize(texture( material.texture_normal1, TexCoords ).rgb * 2.0 - 1.0);
		viewDir = normalize(TBN * ProjPos - TBN * FragPos);
		result = CalcLight(dirLight, pointLights, norm, FragPos, viewDir);
	}
	FragColor = vec4(result, 1.0f);
//	float gamma = 2.2;
//	FragColor.rgb = pow(FragColor.rgb, vec3(1.0/gamma));
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
		for(int i = 0; i < NR_POINT_LIGHTS; i++)
		{
			result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);
		}
	}
	// returning result
	return result;
}

float CalcDirShadows(vec3 lightDir, vec3 normal)
{
	float shadow = 0.0f;
	float ambientShadow = 0.7f;
	vec3 lightCoords = FragPosLight.xyz / FragPosLight.w;

//	if (dot(Normal, lightDir) <= 0)
//	{
//		shadow = 9 * ambientShadow * (1 + dot(Normal, lightDir));
//		// Get average shadow
//		shadow /= 9.0f;
//
//	}
//	else if(lightCoords.z <= 1.0f)
//	{
//		// Get from [-1, 1] range to [0, 1] range just like the shadow map
//		lightCoords = (lightCoords + 1.0f) / 2.0f;
//		float currentDepth = lightCoords.z;
//
//		// Prevents shadow acne
//		float bias = max(0.01 * (1.0 - dot(normal, lightDir)), 0.001);
//
//		// Smoothens out the shadowsWAS
//		int sampleRadius = 1;
//		vec2 pixelSize = 1.0 / vec2(textureSize(material.texture_shadow, 0));
//
//		for(int y = -sampleRadius; y <= sampleRadius; y++)
//		{
//			for(int x = -sampleRadius; x <= sampleRadius; x++)
//			{
//				float pcfDepth = texture(material.texture_shadow, lightCoords.xy + vec2(x, y) * pixelSize).r;
//				shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;
//
//			}
//		}
//
//		// Get average shadow
//		shadow /= 9.0f;
//	}
//	return shadow;
	return 0.0f;
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, vec3 FragPos)
{
	vec3 lightDir = normalize(-light.direction);

	float shadow = CalcDirShadows(lightDir, normal);

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