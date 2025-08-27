#version 460

#ifdef VERTEX_SHADER

#extension GL_EXT_buffer_reference : require

layout (location = 0) out vec2 v_TexCoord;

struct Vertex
{
	vec2 Position;
	vec2 TexCoord;
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
	Vertex vertex = PushConstants.vertexBuffer.vertices[gl_VertexIndex];
	v_TexCoord = vertex.TexCoord;
	gl_Position = vec4(vertex.Position, 0, 1);
}

#endif

#ifdef FRAGMENT_SHADER

layout (location = 0) out vec4 o_Color;

layout (location = 0) in vec2 v_TexCoord;

layout (set = 0, binding = 0) uniform sampler2D u_Texture;

layout (set = 0, binding = 1) uniform TestBuffer
{
	vec3 u_Color;
};

void main()
{
	o_Color = vec4(texture(u_Texture, v_TexCoord).rgb * u_Color, 1.0);
}

#endif