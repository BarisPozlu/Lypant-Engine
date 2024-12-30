#ifdef VERTEX_SHADER

layout(location = 0) in vec4 a_Position;

uniform mat4 u_MVP;

void main()
{
	gl_Position = u_MVP * a_Position;
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