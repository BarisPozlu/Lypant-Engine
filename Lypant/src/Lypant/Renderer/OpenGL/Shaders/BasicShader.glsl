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

void main()
{
	color = vec4(0.8, 0.2, 0.3, 1.0);
}

#endif