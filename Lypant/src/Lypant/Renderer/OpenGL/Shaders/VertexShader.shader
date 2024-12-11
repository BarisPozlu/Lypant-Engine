#version 460 core

layout(location = 0) in vec4 a_Position;

void main()
{
	gl_Position = a_Position;
}
