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

uniform vec3 u_ObjectColor;

uniform vec3 u_LightPosition;
uniform vec3 u_LightColor;

uniform vec3 u_ViewPosition;

void main()
{
	vec3 ambient = 0.1 * u_LightColor;

	vec3 lightDirection = normalize(u_LightPosition - v_WorldPosition);

	vec3 diffuse = max(dot(v_Normal, lightDirection), 0.0) * u_LightColor;

	vec3 viewDirection = normalize(u_ViewPosition - v_WorldPosition);
	vec3 reflectDirection = reflect(-lightDirection, v_Normal);

	vec3 specular = pow(max(dot(viewDirection, reflectDirection), 0.0), 128) * 0.8 * u_LightColor;

	color = vec4((ambient + diffuse + specular) * u_ObjectColor, 1.0);
}

#endif