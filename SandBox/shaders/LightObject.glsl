#ifdef VERTEX_SHADER

layout(location = 0) in vec4 a_Position;

layout (std140, binding = 4) uniform Camera
{
	mat4 u_VP;
	vec3 u_ViewPosition;
};

uniform mat4 u_ModelMatrix;

void main()
{
	gl_Position = u_VP * u_ModelMatrix * a_Position;
}

#endif

#ifdef FRAGMENT_SHADER

layout(location = 0) out vec4 color;

uniform vec3 u_LightColor;

void main()
{
	color = vec4(u_LightColor, 1.0);
}

#endif