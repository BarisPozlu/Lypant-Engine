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

layout (buffer_reference) readonly buffer IndexBuffer
{
	uint indices[];
};

layout (push_constant) uniform PushConstants
{
	VertexBuffer vertexBuffer;
    IndexBuffer indexBuffer;
};

void main()
{
	uint index = indexBuffer.indices[gl_VertexIndex];
    Vertex vertex = vertexBuffer.vertices[index];

	v_TexCoord = vertex.TexCoord;
	gl_Position = vec4(vertex.Position, 0, 1);
}

#endif

#ifdef FRAGMENT_SHADER

layout (location = 0) out vec4 o_Color;

layout (location = 0) in vec2 v_TexCoord;

layout (set = 1, binding = 0) uniform sampler2D u_SceneTexture;

//uniform sampler2D u_BloomTexture;

//uniform bool u_IsBloomEnabled;
//uniform float u_Exposure;

void main()
{
	vec3 color = texture(u_SceneTexture, v_TexCoord).rgb;
//	if (u_IsBloomEnabled)
//	{
//		vec3 bloomSample = texture(u_BloomTexture, v_TexCoord).rgb;
//		color = mix(color, bloomSample, 0.04);
//	}
	
	//color = vec3(1.0) - exp(-color * u_Exposure); // tone mapping
	color = vec3(1.0) - exp(-color * 1); // tone mapping
	o_Color = vec4(pow(color, vec3(1.0 / 2.2)), 1.0); // gamma correction
}

#endif