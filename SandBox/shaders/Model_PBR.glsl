#version 460

#ifdef VERTEX_SHADER

#extension GL_EXT_buffer_reference : require

layout (location = 0) out vec3 v_WorldPosition;
layout (location = 1) out mat3 v_TBNMatrix;
layout (location = 4) out vec2 v_TexCoord;
layout (location = 5) out vec3 v_Normal;
layout (location = 6) flat out uint v_DrawId;

struct Vertex
{
	vec4 Position;
	vec3 Normal;
	vec3 Tangent;
	vec2 TexCoord;
};

struct MetaData
{
	uint VertexOffset;
	uint IndexOffset;
};

struct MatrixData
{
	mat4 ModelMatrix;
	mat3 NormalMatrix;
};

struct MaterialConstants
{
	vec3 Albedo;
	float Roughness;
	float Metallic;
	bool UseCombinedORM;
	bool UseNormalMap;
};

layout (buffer_reference) readonly buffer VertexBuffer
{
	Vertex vertices[];
};

layout (buffer_reference) readonly buffer IndexBuffer
{
	uint indices[];
};

layout (push_constant) uniform PushConstants
{
	VertexBuffer vertexBuffer;
    IndexBuffer indexBuffer;
};

//out vec4 v_SpotLightSpacePositions[8];

layout (set = 0, binding = 0) readonly buffer Camera
{
	mat4 u_VP;
	mat4 u_ViewMatrix;
	vec3 u_ViewPosition;
};

layout (set = 2, binding = 1) readonly buffer MetaBuffer
{
	MetaData u_MetaData[];
};

layout (set = 2, binding = 2) readonly buffer MatrixBuffer
{
	MatrixData u_MatrixData[];
};

layout (set = 2, binding = 3) readonly buffer MaterialBuffer
{
	MaterialConstants u_MaterialConstants[];
};

//uniform mat4 u_SpotLightSpaceMatrices[8];

const uint IndexSize = 4;
const uint VertexSize = 64;

void main()
{
	uint drawId = gl_InstanceIndex;
	v_DrawId = drawId;

	uint index = indexBuffer.indices[u_MetaData[drawId].IndexOffset / IndexSize + gl_VertexIndex];
    Vertex vertex = vertexBuffer.vertices[u_MetaData[drawId].VertexOffset / VertexSize + index];

	mat4 modelMatrix = u_MatrixData[drawId].ModelMatrix;
	mat3 normalMatrix = u_MatrixData[drawId].NormalMatrix;

	v_WorldPosition = vec3(modelMatrix * vertex.Position);

	vec3 normal = normalize(normalMatrix * vertex.Normal);

	if (u_MaterialConstants[drawId].UseNormalMap)
	{
		vec3 tangent = normalize(normalMatrix * vertex.Tangent);
		tangent = normalize(tangent - dot(tangent, normal) * normal);

		vec3 bitangent = cross(normal, tangent);

		v_TBNMatrix = mat3(tangent, bitangent, normal);
	}
	
	else
	{
		v_Normal = normal;
	}

//	for (int i = 0; i < 8; i++)
//	{
//		v_SpotLightSpacePositions[i] = u_SpotLightSpaceMatrices[i] * vec4(v_WorldPosition, 1.0);
//	}

	v_TexCoord = vertex.TexCoord;
	gl_Position = u_VP * modelMatrix * vertex.Position;
}

#endif

#ifdef FRAGMENT_SHADER

#extension GL_EXT_nonuniform_qualifier : require

layout (location = 0) out vec4 o_Color;

layout (location = 0) in vec3 v_WorldPosition;
layout (location = 1) in mat3 v_TBNMatrix;
layout (location = 4) in vec2 v_TexCoord;
layout (location = 5) in vec3 v_Normal;
layout (location = 6) flat in uint v_DrawId;

//in vec4 v_SpotLightSpacePositions[8];

layout (set = 0, binding = 0) readonly buffer Camera
{
	mat4 u_VP;
	mat4 u_ViewMatrix;
	vec3 u_ViewPosition;
};

layout (set = 0, binding = 1) readonly buffer NumberOfLights
{
	int u_NumberOfPointLights;
	int u_NumberOfSpotLights;
	int u_NumberOfDirectionalLights;
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

layout (set = 0, binding = 2) readonly buffer PointLights
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

layout (set = 0, binding = 3) readonly buffer SpotLights
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

layout (set = 0, binding = 4) readonly buffer DirectionalLights
{
	DirectionalLight u_DirectionalLights[30];
};

struct MaterialConstants
{
	vec3 Albedo;
	float Roughness;
	float Metallic;
	bool UseCombinedORM;
	bool UseNormalMap;
};

layout (set = 2, binding = 3) readonly buffer MaterialBuffer
{
	MaterialConstants u_MaterialConstants[];
};

layout (set = 2, binding = 0) uniform sampler2D u_Textures[];

const uint AlbedoOffset = 0;
const uint ORMOffset = 1;
const uint AmbientOcclusionOffset = 2;
const uint RoughnessOffset = 3;
const uint MetallicOffset = 4;
const uint NormalOffset = 5;

layout (set = 1, binding = 0) uniform samplerCube u_DiffuseIrradianceMap;
layout (set = 1, binding = 1) uniform samplerCube u_PreFilteredMap;
layout (set = 1, binding = 2) uniform sampler2D u_BRDFIntegrationMap;

//uniform sampler2DArray u_DirectionalLightShadowMaps;
//uniform float u_CascadePlaneDistances[5];
//uniform mat4 u_DirectionalLightSpaceMatrices[5];

//uniform sampler2DArray u_SpotLightShadowMaps;
//uniform samplerCubeArray u_PointLightShadowMaps;

vec4 TextureBindless(uint imageOffset);
vec3 CalculatePointLight(int i, vec3 normal, vec3 viewDirection, vec3 albedo, float roughness, float metallic, vec3 F0);
vec3 CalculateSpotLight(int i, vec3 normal, vec3 viewDirection, vec3 albedo, float roughness, float metallic, vec3 F0);
vec3 CalculateDirectionalLight(int i, vec3 normal, vec3 viewDirection, vec3 albedo, float roughness, float metallic, vec3 F0);

//float CalculateDirectionalLightShadowFactor(vec3 normal, vec3 lightDirection, int shadowMapIndex);
//float CalculateSpotLightShadowFactor(vec3 normal, vec3 lightDirection, int shadowMapIndex);
//float CalculatePointLightShadowFactor(vec3 normal, vec3 lightPosition, int shadowMapIndex);

float DistributionGGX(vec3 normal, vec3 halfwayDirection, float roughness);
float GeometrySchlickGGX(float NdotV, float roughness);
float GeometrySmith(vec3 normal, vec3 viewDirection, vec3 lightDirection, float roughness);
vec3 FresnelSchlick(float cosTheta, vec3 F0);
vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness);

const float PI = 3.14159265359;

void main()
{
	vec3 normal;

	if (u_MaterialConstants[v_DrawId].UseNormalMap)
	{
		normal = TextureBindless(NormalOffset).rgb * 2.0 - 1.0;
		normal = normalize(v_TBNMatrix * normal);
	}

	else
	{
		normal = normalize(v_Normal);
	}

	vec3 viewDirection = normalize(u_ViewPosition - v_WorldPosition);

	vec3 albedo = TextureBindless(AlbedoOffset).rgb * u_MaterialConstants[v_DrawId].Albedo;

	float ao;
	float roughness;
	float metallic;

	if (u_MaterialConstants[v_DrawId].UseCombinedORM)
	{
		vec3 orm = TextureBindless(ORMOffset).rgb;
		ao = orm.r;
		roughness = orm.g * u_MaterialConstants[v_DrawId].Roughness;
		metallic = orm.b * u_MaterialConstants[v_DrawId].Metallic;
	}

	else
	{
		ao = TextureBindless(AmbientOcclusionOffset).r;
		roughness = TextureBindless(RoughnessOffset).r * u_MaterialConstants[v_DrawId].Roughness;
		metallic = TextureBindless(MetallicOffset).r * u_MaterialConstants[v_DrawId].Metallic;		
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

	vec3 Ks = FresnelSchlickRoughness(max(dot(normal, viewDirection), 0.0), F0, roughness);
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
	//vec3 ambient = (diffuse + specular) * u_AmbientStrength;
	vec3 ambient = diffuse + specular;

	o_Color = vec4(Lo + ambient, 1.0);
	//o_Color = vec4(ambient, 1.0);
}

vec4 TextureBindless(uint imageOffset)
{
	return texture(u_Textures[nonuniformEXT(v_DrawId) * 6 + imageOffset], v_TexCoord);
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
	vec3 F = FresnelSchlick(max(dot(halfwayDirection, viewDirection), 0.0), F0);
	float G = GeometrySmith(normal, viewDirection, lightDirection, roughness);

	vec3 Kd = vec3(1.0) - F;
	Kd *= 1.0 - metallic;

	vec3 numerator = D * F * G;
	float denominator = 4.0 * max(dot(normal, viewDirection), 0.0) * max(dot(normal, lightDirection), 0.0) + 0.0001;

	vec3 specular = numerator / denominator;

	//return (Kd * albedo / PI + specular) * radiance * max(dot(normal, lightDirection), 0.0) * (1 - CalculatePointLightShadowFactor(normal, u_PointLights[i].Position, u_PointLights[i].ShadowMapIndex));
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
	vec3 F = FresnelSchlick(max(dot(halfwayDirection, viewDirection), 0.0), F0);
	float G = GeometrySmith(normal, viewDirection, lightDirection, roughness);

	vec3 Kd = vec3(1.0) - F;
	Kd *= 1.0 - metallic; 

	vec3 numerator = D * F * G;
	float denominator = 4.0 * max(dot(normal, viewDirection), 0.0) * max(dot(normal, lightDirection), 0.0) + 0.0001;

	vec3 specular = numerator / denominator;

	//return (Kd * albedo / PI + specular) * radiance * max(dot(normal, lightDirection), 0.0) * (1 - CalculateSpotLightShadowFactor(normal, lightDirection, u_SpotLights[i].ShadowMapIndex));
	return (Kd * albedo / PI + specular) * radiance * max(dot(normal, lightDirection), 0.0);
}

vec3 CalculateDirectionalLight(int i, vec3 normal, vec3 viewDirection, vec3 albedo, float roughness, float metallic, vec3 F0)
{
	// calculate radiance
	vec3 halfwayDirection = normalize(-u_DirectionalLights[i].Direction + viewDirection);
	vec3 radiance = u_DirectionalLights[i].Color;

	// calculate cook-torrance BRDF
	float D = DistributionGGX(normal, halfwayDirection, roughness);
	vec3 F = FresnelSchlick(max(dot(halfwayDirection, viewDirection), 0.0), F0);
	float G = GeometrySmith(normal, viewDirection, -u_DirectionalLights[i].Direction, roughness);

	vec3 Kd = vec3(1.0) - F;
	Kd *= 1.0 - metallic; 

	vec3 numerator = D * F * G;
	float denominator = 4.0 * max(dot(normal, viewDirection), 0.0) * max(dot(normal, -u_DirectionalLights[i].Direction), 0.0) + 0.0001;

	vec3 specular = numerator / denominator;

	//return (Kd * albedo / PI + specular) * radiance * max(dot(normal, -u_DirectionalLights[i].Direction), 0.0) * (1 - CalculateDirectionalLightShadowFactor(normal, -u_DirectionalLights[i].Direction, u_DirectionalLights[i].ShadowMapIndex));
	return (Kd * albedo / PI + specular) * radiance * max(dot(normal, -u_DirectionalLights[i].Direction), 0.0);
}

//float CalculateDirectionalLightShadowFactor(vec3 normal, vec3 lightDirection, int shadowMapIndex)
//{
//	if (shadowMapIndex == -1)
//	{
//		return 0.0;
//	}
//
//	vec4 worldPosition = vec4(v_WorldPosition, 1.0);
//	vec4 viewPosition = u_ViewMatrix * worldPosition;
//	float depthValue = abs(viewPosition.z);
//	
//	int layer = 4;
//
//	for (int i = 0; i < 4; i++)
//	{
//		if (depthValue < u_CascadePlaneDistances[i])
//		{
//			layer = i;
//			break;
//		}
//	}
//
//	vec4 lightSpacePosition = u_DirectionalLightSpaceMatrices[layer] * worldPosition;
//
//	float shadowFactor = 0;
//	vec3 shadowCoords = lightSpacePosition.xyz / lightSpacePosition.w * 0.5 + 0.5; // xy being texture coords and z being the depth value
//
//	if (shadowCoords.z > 1)
//	{
//		return 0.0;
//	}
//
//	float bias = max(0.05 * (1.0 - dot(normal, lightDirection)), 0.005);
//	bias *= 1 / (u_CascadePlaneDistances[layer] * 0.5);
//
//	vec2 texelSize = 1.0 / textureSize(u_DirectionalLightShadowMaps, 0).xy;
//
//	for (int x = -1; x <= 1; x++)
//	{
//		for (int y = -1; y <= 1; y++)
//		{
//			float sampledDepth = texture(u_DirectionalLightShadowMaps, vec3(shadowCoords.xy + texelSize * vec2(x, y), layer)).r;
//			shadowFactor += shadowCoords.z - bias > sampledDepth ? 1.0 : 0.0;
//		}
//	}
//	
//	return shadowFactor / 9;
//}
//
//float CalculateSpotLightShadowFactor(vec3 normal, vec3 lightDirection, int shadowMapIndex)
//{
//	if (shadowMapIndex == -1)
//	{
//		return 0.0;
//	}
//
//	float shadowFactor = 0;
//	vec3 shadowCoords = v_SpotLightSpacePositions[shadowMapIndex].xyz / v_SpotLightSpacePositions[shadowMapIndex].w * 0.5 + 0.5; // xy being texture coords and z being the depth value
//
//	if (shadowCoords.z > 1)
//	{
//		return 0.0;
//	}
//
//	float bias = max(0.05 * (1.0 - dot(normal, lightDirection)), 0.005);
//
//	vec2 texelSize = 1.0 / textureSize(u_SpotLightShadowMaps, 0).xy;
//
//	for (int x = -1; x <= 1; x++)
//	{
//		for (int y = -1; y <= 1; y++)
//		{
//			float sampledDepth = texture(u_SpotLightShadowMaps, vec3(shadowCoords.xy + texelSize * vec2(x, y), shadowMapIndex)).r;
//			shadowFactor += shadowCoords.z - bias > sampledDepth ? 1.0 : 0.0;
//		}
//	}
//	
//	return shadowFactor / 9;
//}
//
//float CalculatePointLightShadowFactor(vec3 normal, vec3 lightPosition, int shadowMapIndex)
//{
//	// There is no PCF for point lights for now as they are already super expensive
//	if (shadowMapIndex == -1)
//	{
//		return 0.0;
//	}
//
//	float shadowFactor = 0;
//
//	vec3 lightToFragment = v_WorldPosition - lightPosition;
//
//	float distanceFromLight = length(lightToFragment);
//	float bias = 0.05;
//	
//	float sampledDistance = texture(u_PointLightShadowMaps, vec4(lightToFragment, shadowMapIndex)).r;
//	sampledDistance *= 25.0;
//	shadowFactor += distanceFromLight - bias > sampledDistance ? 1.0 : 0.0;
//	
//	return shadowFactor;
//}

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

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}  

#endif