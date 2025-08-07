#version 460

#ifdef VERTEX_SHADER

#extension GL_EXT_buffer_reference : require

struct Vertex
{
	vec2 Position;
};

layout (buffer_reference) readonly buffer VertexBuffer
{
	Vertex vertices[];
};

layout (push_constant) uniform PushConstant
{
	VertexBuffer vertexBuffer;
} PushConstants;

void main()
{
	gl_Position = vec4(PushConstants.vertexBuffer.vertices[gl_VertexIndex].Position, 0, 1);
}

#endif

#ifdef FRAGMENT_SHADER

layout (location = 0) out vec4 o_Color;

void main()
{
	o_Color = vec4(0.5, 0.5, 0.5, 1.0);
}

#endif