#version 460 core

layout(location = 0) out vec4 color;

uniform sampler2D u_TexSlot;

in vec2 v_TexCoord;

void main()
{
	color = texture(u_TexSlot, v_TexCoord);
}
