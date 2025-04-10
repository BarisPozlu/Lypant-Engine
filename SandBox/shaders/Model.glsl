#ifdef VERTEX_SHADER

layout (location = 0) in vec4 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec3 a_Tangent;
layout (location = 3) in vec2 a_TexCoord;

out vec3 v_WorldPosition;
out mat3 v_TBNMatrix;
out vec2 v_TexCoord;

layout (std140, binding = 4) uniform Camera
{
	mat4 u_VP;
	vec3 u_ViewPosition;
};

uniform mat4 u_ModelMatrix;
uniform mat3 u_NormalMatrix;

void main()
{
	v_WorldPosition = vec3(u_ModelMatrix * a_Position);

	vec3 normal = normalize(u_NormalMatrix * a_Normal);

	vec3 tangent = normalize(u_NormalMatrix * a_Tangent);
	tangent = normalize(tangent - dot(tangent, normal) * normal);

	vec3 bitangent = cross(normal, tangent);

	v_TBNMatrix = mat3(tangent, bitangent, normal);

	v_TexCoord = a_TexCoord;
	gl_Position = u_VP * u_ModelMatrix * a_Position;
}

#endif

#ifdef FRAGMENT_SHADER

layout (location = 0) out vec4 color;

in vec3 v_WorldPosition;
in mat3 v_TBNMatrix;
in vec2 v_TexCoord;

layout (std140, binding = 4) uniform Camera
{
	mat4 u_VP;
	vec3 u_ViewPosition;
};

struct PointLight
{
	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
	vec3 Position;
	float Linear;
	float Quadratic;
};

layout (std140, binding = 0) uniform PointLights
{
	PointLight u_PointLights[30];
};

struct SpotLight
{
	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
	vec3 Position;
	vec3 Direction;
	float OuterCutOff;
	float InnerCutOff;
};

layout (std140, binding = 1) uniform SpotLights
{
	SpotLight u_SpotLights[30];
};

struct DirectionalLight
{
	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
	vec3 Direction;
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

uniform sampler2D u_DiffuseMap;
uniform sampler2D u_SpecularMap;
uniform sampler2D u_NormalMap;

vec3 CalculatePointLight(int i, vec3 normal, vec3 viewDirection, vec3 materialDiffuse, vec3 materialSpecular);
vec3 CalculateSpotLight(int i, vec3 normal, vec3 viewDirection, vec3 materialDiffuse, vec3 materialSpecular);
vec3 CalculateDirectionalLight(int i, vec3 normal, vec3 viewDirection, vec3 materialDiffuse, vec3 materialSpecular);

void main()
{
	vec3 normal = texture(u_NormalMap, v_TexCoord).rgb * 2.0 - 1.0;
	normal = normalize(v_TBNMatrix * normal);
	vec3 viewDirection = normalize(u_ViewPosition - v_WorldPosition);
	vec3 materialDiffuse = vec3(texture(u_DiffuseMap, v_TexCoord));
	vec3 materialSpecular = vec3(texture(u_SpecularMap, v_TexCoord));

	vec3 result = vec3(0);

	int i;

	for (i = 0; i < u_NumberOfPointLights; i++)
	{
		result += CalculatePointLight(i, normal, viewDirection, materialDiffuse, materialSpecular);
	}

	for (i = 0; i < u_NumberOfSpotLights; i++)
	{
		result += CalculateSpotLight(i, normal, viewDirection, materialDiffuse, materialSpecular);
	}

	for (i = 0; i < u_NumberOfDirectionalLights; i++)
	{
		result += CalculateDirectionalLight(i, normal, viewDirection, materialDiffuse, materialSpecular);
	}

	color = vec4(result, 1.0);
}

vec3 CalculatePointLight(int i, vec3 normal, vec3 viewDirection, vec3 materialDiffuse, vec3 materialSpecular)
{
	vec3 ambient = u_PointLights[i].Ambient * materialDiffuse;

	vec3 lightDirection = normalize(u_PointLights[i].Position - v_WorldPosition);
	vec3 diffuse = max(dot(normal, lightDirection), 0) * materialDiffuse * u_PointLights[i].Diffuse;

	vec3 halfwayDirection = normalize(lightDirection + viewDirection);
	vec3 specular = pow(max(dot(halfwayDirection, normal), 0), 64) * materialSpecular * u_PointLights[i].Specular;

	float pointLightDistance = distance(v_WorldPosition, u_PointLights[i].Position);
	float attenuation = 1.0 / (u_PointLights[i].Linear * pointLightDistance + u_PointLights[i].Quadratic * pointLightDistance * pointLightDistance);

	return (ambient + diffuse + specular) * attenuation;
}

vec3 CalculateSpotLight(int i, vec3 normal, vec3 viewDirection, vec3 materialDiffuse, vec3 materialSpecular)
{
	vec3 ambient = u_SpotLights[i].Ambient * materialDiffuse;

	vec3 lightDirection = normalize(u_SpotLights[i].Position - v_WorldPosition);
	float cosOfAngle = dot(lightDirection, -u_SpotLights[i].Direction);
	
	float intensity = (cosOfAngle - u_SpotLights[i].OuterCutOff) / (u_SpotLights[i].InnerCutOff - u_SpotLights[i].OuterCutOff);
	intensity = clamp(intensity, 0.0, 1.0);

	vec3 diffuse = max(dot(normal, lightDirection), 0) * materialDiffuse * u_SpotLights[i].Diffuse;

	vec3 halfwayDirection = normalize(lightDirection + viewDirection);
	vec3 specular = pow(max(dot(halfwayDirection, normal), 0), 64) * materialSpecular * u_SpotLights[i].Specular;

	return (ambient + diffuse + specular) * intensity;
}

vec3 CalculateDirectionalLight(int i, vec3 normal, vec3 viewDirection, vec3 materialDiffuse, vec3 materialSpecular)
{
	vec3 ambient = u_DirectionalLights[i].Ambient * materialDiffuse;
	
	vec3 diffuse = max(dot(-u_DirectionalLights[i].Direction, normal), 0) * materialDiffuse * u_DirectionalLights[i].Diffuse;

	vec3 halfwayDirection = normalize(-u_DirectionalLights[i].Direction + viewDirection);
	vec3 specular = pow(max(dot(halfwayDirection, normal), 0), 64) * materialSpecular * u_DirectionalLights[i].Specular;

	return ambient + diffuse + specular;
}

#endif