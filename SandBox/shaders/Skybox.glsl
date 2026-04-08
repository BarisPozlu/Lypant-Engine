#version 460

#ifdef VERTEX_SHADER

#extension GL_EXT_buffer_reference : require

layout (location = 0) out vec3 v_DirectionVector;

struct Vertex
{
	vec4 Position;
};

layout (buffer_reference) readonly buffer VertexBuffer
{
	Vertex vertices[];
};

layout (buffer_reference) readonly buffer IndexBuffer
{
	uint indices[];
};

layout (push_constant) uniform PushConstants
{
	VertexBuffer vertexBuffer;
    IndexBuffer indexBuffer;
};

layout (set = 0, binding = 0) readonly buffer Camera
{
	mat4 u_VP;
	mat4 u_ViewMatrix;
	vec3 u_ViewPosition;
};

void main()
{
	uint index = indexBuffer.indices[gl_VertexIndex];
    Vertex vertex = vertexBuffer.vertices[index];

	v_DirectionVector = vertex.Position.xyz;

	vec4 position = u_VP * (vec4(vertex.Position.xyz + u_ViewPosition, 1.0));
	gl_Position = position.xyww;
}

#endif

#ifdef FRAGMENT_SHADER

layout (location = 0) out vec4 o_Color;

layout (location = 0) in vec3 v_DirectionVector;

layout (set = 1, binding = 0) uniform samplerCube u_Cubemap;

void main()
{
	o_Color = texture(u_Cubemap, v_DirectionVector);
}

#endif