#version 460 core

layout(location = 0) in vec4 a_Position;

uniform mat4 u_VP;

void main()
{
	gl_Position = u_VP * a_Position;
}
