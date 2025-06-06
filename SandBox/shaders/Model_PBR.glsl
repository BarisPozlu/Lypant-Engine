#ifdef VERTEX_SHADER

layout (location = 0) in vec4 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec3 a_Tangent;
layout (location = 3) in vec2 a_TexCoord;

out vec3 v_WorldPosition;
out mat3 v_TBNMatrix;
out vec2 v_TexCoord;

out vec3 v_Normal;

out vec4 v_SpotLightSpacePositions[8];

layout (std140, binding = 4) uniform Camera
{
	mat4 u_VP;
	mat4 u_ViewMatrix;
	vec3 u_ViewPosition;
};

uniform mat4 u_ModelMatrix;
uniform mat3 u_NormalMatrix;

uniform bool u_UseNormalMap;

uniform mat4 u_SpotLightSpaceMatrices[8];

void main()
{
	v_WorldPosition = vec3(u_ModelMatrix * a_Position);

	vec3 normal = normalize(u_NormalMatrix * a_Normal);

	if (u_UseNormalMap)
	{
		vec3 tangent = normalize(u_NormalMatrix * a_Tangent);
		tangent = normalize(tangent - dot(tangent, normal) * normal);

		vec3 bitangent = cross(normal, tangent);

		v_TBNMatrix = mat3(tangent, bitangent, normal);
	}
	
	else
	{
		v_Normal = normal;
	}

	for (int i = 0; i < 8; i++)
	{
		v_SpotLightSpacePositions[i] = u_SpotLightSpaceMatrices[i] * vec4(v_WorldPosition, 1.0);
	}

	v_TexCoord = a_TexCoord;
	gl_Position = u_VP * u_ModelMatrix * a_Position;
}

#endif

#ifdef FRAGMENT_SHADER

layout (location = 0) out vec4 o_Color;

in vec3 v_WorldPosition;
in mat3 v_TBNMatrix;
in vec2 v_TexCoord;
in vec3 v_Normal;

in vec4 v_SpotLightSpacePositions[8];

layout (std140, binding = 4) uniform Camera
{
	mat4 u_VP;
	mat4 u_ViewMatrix;
	vec3 u_ViewPosition;
};

struct PointLight
{
	vec3 Color;
	vec3 Position;
	float Linear;
	float Quadratic;
	int ShadowMapIndex;
	bool CastShadows;
};

layout (std140, binding = 0) uniform PointLights
{
	PointLight u_PointLights[30];
};

struct SpotLight
{
	vec3 Color;
	vec3 Position;
	vec3 Direction;
	float OuterCutOff;
	float InnerCutOff;
	int ShadowMapIndex;
	bool CastShadows;
};

layout (std140, binding = 1) uniform SpotLights
{
	SpotLight u_SpotLights[30];
};

struct DirectionalLight
{
	vec3 Color;
	vec3 Direction;
	int ShadowMapIndex;
	bool CastShadows;
};

layout (std140, binding = 2) uniform DirectionalLights
{
	DirectionalLight u_DirectionalLights[30];
};

layout (std140, binding = 3) uniform NumberOfLights
{
	int u_NumberOfPointLights;
	int u_NumberOfSpotLights;
	int u_NumberOfDirectionalLights;
};

uniform bool u_UseCombinedORM;

uniform sampler2D u_AlbedoMap;
uniform sampler2D u_ORMMap;
uniform sampler2D u_AmbientOcclusionMap;
uniform sampler2D u_RoughnessMap;
uniform sampler2D u_MetallicMap;
uniform sampler2D u_NormalMap;

uniform bool u_UseNormalMap;
uniform vec3 u_Albedo;
uniform float u_Roughness;
uniform float u_Metallic;

uniform samplerCube u_DiffuseIrradianceMap;
uniform samplerCube u_PreFilteredMap;
uniform sampler2D u_BRDFIntegrationMap;

uniform sampler2DArray u_DirectionalLightShadowMaps;
uniform float u_CascadePlaneDistances[5];
uniform mat4 u_DirectionalLightSpaceMatrices[5];

uniform sampler2DArray u_SpotLightShadowMaps;

vec3 CalculatePointLight(int i, vec3 normal, vec3 viewDirection, vec3 albedo, float roughness, float metallic, vec3 F0);
vec3 CalculateSpotLight(int i, vec3 normal, vec3 viewDirection, vec3 albedo, float roughness, float metallic, vec3 F0);
vec3 CalculateDirectionalLight(int i, vec3 normal, vec3 viewDirection, vec3 albedo, float roughness, float metallic, vec3 F0);

float CalculateDirectionalLightShadowFactor(vec3 normal, vec3 lightDirection, int shadowMapIndex);
float CalculateSpotLightShadowFactor(vec3 normal, vec3 lightDirection, int shadowMapIndex);

float DistributionGGX(vec3 normal, vec3 halfwayDirection, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 normal, vec3 viewDirection, vec3 lightDirection, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);

const float PI = 3.14159265359;

void main()
{
	vec3 normal;

	if (u_UseNormalMap)
	{
		normal = texture(u_NormalMap, v_TexCoord).rgb * 2.0 - 1.0;
		normal = normalize(v_TBNMatrix * normal);
	}

	else
	{
		normal = normalize(v_Normal);
	}

	vec3 viewDirection = normalize(u_ViewPosition - v_WorldPosition);

	vec3 albedo = texture(u_AlbedoMap, v_TexCoord).rgb * u_Albedo;

	float ao;
	float roughness;
	float metallic;

	if (u_UseCombinedORM)
	{
		vec3 orm = texture(u_ORMMap, v_TexCoord).rgb;
		ao = orm.r;
		roughness = orm.g * u_Roughness;
		metallic = orm.b * u_Metallic;
	}

	else
	{
		ao = texture(u_AmbientOcclusionMap, v_TexCoord).r;
		roughness = texture(u_RoughnessMap, v_TexCoord).r * u_Roughness;
		metallic = texture(u_MetallicMap, v_TexCoord).r * u_Metallic;		
	}

	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);

	vec3 Lo = vec3(0);
	int i;

	for (i = 0; i < u_NumberOfPointLights; i++)
	{
		Lo += CalculatePointLight(i, normal, viewDirection, albedo, roughness, metallic, F0);
	}

	for (i = 0; i < u_NumberOfSpotLights; i++)
	{
		Lo += CalculateSpotLight(i, normal, viewDirection, albedo, roughness, metallic, F0);
	}

	for (i = 0; i < u_NumberOfDirectionalLights; i++)
	{
		Lo += CalculateDirectionalLight(i, normal, viewDirection, albedo, roughness, metallic, F0);
	}

	vec3 Ks = fresnelSchlickRoughness(max(dot(normal, viewDirection), 0.0), F0, roughness);
	vec3 Kd = 1.0 - Ks;
	Kd *= 1.0 - metallic;

	vec3 irradiance = texture(u_DiffuseIrradianceMap, normal).rgb;
	vec3 diffuse = albedo * Kd * irradiance;

	const float maxMipmapLevel = 4.0;
	vec3 reflectionVector = reflect(-viewDirection, normal);
	vec3 preFilteredSample = textureLod(u_PreFilteredMap, reflectionVector, roughness * maxMipmapLevel).rgb;

	vec2 BRDFSample = texture(u_BRDFIntegrationMap, vec2(max(dot(normal, viewDirection), 0.0), roughness)).rg;

	vec3 specular = preFilteredSample * (BRDFSample.x * Ks + BRDFSample.y);

	//vec3 ambient = (diffuse + specular) * ao;
	vec3 ambient = (diffuse + specular);

	o_Color = vec4(Lo + ambient, 1.0);
}

vec3 CalculatePointLight(int i, vec3 normal, vec3 viewDirection, vec3 albedo, float roughness, float metallic, vec3 F0)
{
	// calculate radiance
	vec3 lightDirection = normalize(u_PointLights[i].Position - v_WorldPosition);
	vec3 halfwayDirection = normalize(lightDirection + viewDirection);

	float pointLightDistance = distance(v_WorldPosition, u_PointLights[i].Position);
	float attenuation = 1.0 / (u_PointLights[i].Linear * pointLightDistance + u_PointLights[i].Quadratic * pointLightDistance * pointLightDistance);

	vec3 radiance = u_PointLights[i].Color * attenuation;
	
	// calculate cook-torrance BRDF
	float D = DistributionGGX(normal, halfwayDirection, roughness);
	vec3 F = fresnelSchlick(max(dot(halfwayDirection, viewDirection), 0.0), F0);
	float G = GeometrySmith(normal, viewDirection, lightDirection, roughness);

	vec3 Kd = vec3(1.0) - F;
	Kd *= 1.0 - metallic;

	vec3 numerator = D * F * G;
	float denominator = 4.0 * max(dot(normal, viewDirection), 0.0) * max(dot(normal, lightDirection), 0.0) + 0.0001;

	vec3 specular = numerator / denominator;

	return (Kd * albedo / PI + specular) * radiance * max(dot(normal, lightDirection), 0.0);
}

vec3 CalculateSpotLight(int i, vec3 normal, vec3 viewDirection, vec3 albedo, float roughness, float metallic, vec3 F0)
{
	// calculate radiance
	vec3 lightDirection = normalize(u_SpotLights[i].Position - v_WorldPosition);
	vec3 halfwayDirection = normalize(lightDirection + viewDirection);

	float spotLightDistance = distance(v_WorldPosition, u_SpotLights[i].Position);
	float attenuation = 1.0 / (spotLightDistance * spotLightDistance);

	float cosOfAngle = dot(lightDirection, -u_SpotLights[i].Direction);
	float intensity = (cosOfAngle - u_SpotLights[i].OuterCutOff) / (u_SpotLights[i].InnerCutOff - u_SpotLights[i].OuterCutOff);
	intensity = clamp(intensity, 0.0, 1.0);

	vec3 radiance = u_SpotLights[i].Color * attenuation * intensity;

	// calculate cook-torrance BRDF
	float D = DistributionGGX(normal, halfwayDirection, roughness);
	vec3 F = fresnelSchlick(max(dot(halfwayDirection, viewDirection), 0.0), F0);
	float G = GeometrySmith(normal, viewDirection, lightDirection, roughness);

	vec3 Kd = vec3(1.0) - F;
	Kd *= 1.0 - metallic; 

	vec3 numerator = D * F * G;
	float denominator = 4.0 * max(dot(normal, viewDirection), 0.0) * max(dot(normal, lightDirection), 0.0) + 0.0001;

	vec3 specular = numerator / denominator;

	return (Kd * albedo / PI + specular) * radiance * max(dot(normal, lightDirection), 0.0) * (1 - CalculateSpotLightShadowFactor(normal, lightDirection, u_SpotLights[i].ShadowMapIndex));
}

vec3 CalculateDirectionalLight(int i, vec3 normal, vec3 viewDirection, vec3 albedo, float roughness, float metallic, vec3 F0)
{
	// calculate radiance
	vec3 halfwayDirection = normalize(-u_DirectionalLights[i].Direction + viewDirection);
	vec3 radiance = u_DirectionalLights[i].Color;

	// calculate cook-torrance BRDF
	float D = DistributionGGX(normal, halfwayDirection, roughness);
	vec3 F = fresnelSchlick(max(dot(halfwayDirection, viewDirection), 0.0), F0);
	float G = GeometrySmith(normal, viewDirection, -u_DirectionalLights[i].Direction, roughness);

	vec3 Kd = vec3(1.0) - F;
	Kd *= 1.0 - metallic; 

	vec3 numerator = D * F * G;
	float denominator = 4.0 * max(dot(normal, viewDirection), 0.0) * max(dot(normal, -u_DirectionalLights[i].Direction), 0.0) + 0.0001;

	vec3 specular = numerator / denominator;

	return (Kd * albedo / PI + specular) * radiance * max(dot(normal, -u_DirectionalLights[i].Direction), 0.0) * (1 - CalculateDirectionalLightShadowFactor(normal, -u_DirectionalLights[i].Direction, u_DirectionalLights[i].ShadowMapIndex));
}

float CalculateDirectionalLightShadowFactor(vec3 normal, vec3 lightDirection, int shadowMapIndex)
{
	if (shadowMapIndex == -1)
	{
		return 0.0;
	}

	vec4 worldPosition = vec4(v_WorldPosition, 1.0);
	vec4 viewPosition = u_ViewMatrix * worldPosition;
	float depthValue = abs(viewPosition.z);
	
	int layer = 4;

	for (int i = 0; i < 4; i++)
	{
		if (depthValue < u_CascadePlaneDistances[i])
		{
			layer = i;
			break;
		}
	}

	vec4 lightSpacePosition = u_DirectionalLightSpaceMatrices[layer] * worldPosition;

	float shadowFactor = 0;
	vec3 shadowCoords = lightSpacePosition.xyz / lightSpacePosition.w * 0.5 + 0.5; // xy being texture coords and z being the depth value

	if (shadowCoords.z > 1)
	{
		return 0.0;
	}

	float bias = max(0.05 * (1.0 - dot(normal, lightDirection)), 0.005);
	bias *= 1 / (u_CascadePlaneDistances[layer] * 0.5);

	vec2 texelSize = 1.0 / textureSize(u_DirectionalLightShadowMaps, 0).xy;

	for (int x = -1; x <= 1; x++)
	{
		for (int y = -1; y <= 1; y++)
		{
			float sampledDepth = texture(u_DirectionalLightShadowMaps, vec3(shadowCoords.xy + texelSize * vec2(x, y), layer)).r;
			shadowFactor += shadowCoords.z - bias > sampledDepth ? 1.0 : 0.0;
		}
	}
	
	return shadowFactor / 9;
}

float CalculateSpotLightShadowFactor(vec3 normal, vec3 lightDirection, int shadowMapIndex)
{
	if (shadowMapIndex == -1)
	{
		return 0.0;
	}

	float shadowFactor = 0;
	vec3 shadowCoords = v_SpotLightSpacePositions[shadowMapIndex].xyz / v_SpotLightSpacePositions[shadowMapIndex].w * 0.5 + 0.5; // xy being texture coords and z being the depth value

	if (shadowCoords.z > 1)
	{
		return 0.0;
	}

	float bias = max(0.05 * (1.0 - dot(normal, lightDirection)), 0.005);

	vec2 texelSize = 1.0 / textureSize(u_SpotLightShadowMaps, 0).xy;

	for (int x = -1; x <= 1; x++)
	{
		for (int y = -1; y <= 1; y++)
		{
			float sampledDepth = texture(u_SpotLightShadowMaps, vec3(shadowCoords.xy + texelSize * vec2(x, y), shadowMapIndex)).r;
			shadowFactor += shadowCoords.z - bias > sampledDepth ? 1.0 : 0.0;
		}
	}
	
	return shadowFactor / 9;
}

float DistributionGGX(vec3 normal, vec3 halfwayDirection, float roughness)
{
    float a      = roughness * roughness;
    float a2     = a*a;
    float NdotH  = max(dot(normal, halfwayDirection), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / max(denom, 1e-8);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}

float GeometrySmith(vec3 normal, vec3 viewDirection, vec3 lightDirection, float roughness)
{
    float NdotV = max(dot(normal, viewDirection), 0.0);
    float NdotL = max(dot(normal, lightDirection), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, roughness);
    float ggx1  = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}  

#endif