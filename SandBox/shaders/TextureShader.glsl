#ifdef VERTEX_SHADER

layout(location = 0) in vec4 a_Position;
layout(location = 1) in vec2 a_TexCoord;

uniform mat4 u_MVP;

out vec2 v_TexCoord;

void main()
{
	v_TexCoord = a_TexCoord;
	gl_Position = u_MVP * a_Position;
}

#endif

#ifdef FRAGMENT_SHADER

layout(location = 0) out vec4 color;

uniform sampler2D u_TexSlot;

in vec2 v_TexCoord;

void main()
{
	color = texture(u_TexSlot, v_TexCoord);
}

#endif