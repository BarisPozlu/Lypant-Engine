#ifdef VERTEX_SHADER

layout(location = 0) in vec4 a_Position;
layout(location = 1) in vec3 a_Normal;

out vec3 v_WorldPosition;
out vec3 v_Normal;

uniform mat4 u_ModelMatrix;
uniform mat4 u_MVP;

uniform mat3 u_NormalMatrix;

void main()
{
	v_WorldPosition = vec3(u_ModelMatrix * a_Position);
	v_Normal = normalize(u_NormalMatrix * a_Normal);
	gl_Position = u_MVP * a_Position;
}

#endif

#ifdef FRAGMENT_SHADER

layout(location = 0) out vec4 color;

in vec3 v_WorldPosition;
in vec3 v_Normal;

struct PointLight
{
	vec3 Color;
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
	vec3 Color;
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
	vec3 Color;
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

uniform vec3 u_ObjectColor;

uniform vec3 u_ViewPosition;

vec3 CalculatePointLight(int i, vec3 normal, vec3 viewDirection);
vec3 CalculateSpotLight(int i, vec3 normal, vec3 viewDirection);
vec3 CalculateDirectionalLight(int i, vec3 normal, vec3 viewDirection);

void main()
{
	vec3 normal = normalize(v_Normal);
	vec3 viewDirection = normalize(u_ViewPosition - v_WorldPosition);

	vec3 result = vec3(0);

	int i;

	for (i = 0; i < u_NumberOfSpotLights; i++)
	{
		result += CalculateSpotLight(i, normal, viewDirection);
	}

	for (i = 0; i < u_NumberOfPointLights; i++)
	{
		result += CalculatePointLight(i, normal, viewDirection);
	}

	for (i = 0; i < u_NumberOfDirectionalLights; i++)
	{
		result += CalculateDirectionalLight(i, normal, viewDirection);
	}

	color = vec4(result * u_ObjectColor, 1.0);
}

vec3 CalculatePointLight(int i, vec3 normal, vec3 viewDirection)
{
	vec3 ambient =  u_PointLights[i].Ambient * u_PointLights[i].Color;

	vec3 lightDirection = normalize(u_PointLights[i].Position - v_WorldPosition);
	vec3 diffuse = max(dot(normal, lightDirection), 0) * u_PointLights[i].Color * u_PointLights[i].Diffuse;

	vec3 reflectDirection = reflect(-lightDirection, normal);

	vec3 specular = pow(max(dot(viewDirection, reflectDirection), 0), 32) * u_PointLights[i].Specular * u_PointLights[i].Color;

	float pointLightDistance = distance(v_WorldPosition, u_PointLights[i].Position);
	float attenuation = 1.0 / (1.0 + u_PointLights[i].Linear * pointLightDistance + u_PointLights[i].Quadratic * pointLightDistance * pointLightDistance);

	return (ambient + diffuse + specular) * attenuation;
}

vec3 CalculateSpotLight(int i, vec3 normal, vec3 viewDirection)
{
	vec3 ambient = u_SpotLights[i].Ambient * u_SpotLights[i].Color;

	vec3 lightDirection = normalize(u_SpotLights[i].Position - v_WorldPosition);
	float cosOfAngle = dot(lightDirection, -u_SpotLights[i].Direction);
	
	float intensity = (cosOfAngle - u_SpotLights[i].OuterCutOff) / (u_SpotLights[i].InnerCutOff - u_SpotLights[i].OuterCutOff);
	intensity = clamp(intensity, 0.0, 1.0);

	vec3 diffuse = max(dot(normal, lightDirection), 0) * u_SpotLights[i].Diffuse * u_SpotLights[i].Color;

	vec3 reflectDirection = reflect(-lightDirection, normal);
	vec3 specular = pow(max(dot(viewDirection, reflectDirection), 0), 32) * u_SpotLights[i].Specular * u_SpotLights[i].Color;

	return (ambient + diffuse + specular) * intensity;
}

vec3 CalculateDirectionalLight(int i, vec3 normal, vec3 viewDirection)
{
	vec3 ambient = u_DirectionalLights[i].Ambient * u_DirectionalLights[i].Color;
	
	vec3 diffuse = max(dot(-u_DirectionalLights[i].Direction, normal), 0) * u_DirectionalLights[i].Color * u_DirectionalLights[i].Diffuse;

	vec3 reflectDirection = reflect(-u_DirectionalLights[i].Direction, normal);

	vec3 specular = pow(max(dot(reflectDirection, viewDirection), 0), 32) * u_DirectionalLights[i].Color * u_DirectionalLights[i].Specular;

	return ambient + diffuse + specular;
}

#endif