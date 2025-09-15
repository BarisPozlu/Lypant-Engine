#version 460

#ifdef VERTEX_SHADER

#extension GL_EXT_buffer_reference : require
#extension GL_ARB_shader_viewport_layer_array : require

layout (location = 0) out vec3 v_DirectionVector;

struct Vertex
{
	vec4 Position;
};

layout (buffer_reference) readonly buffer VertexBuffer
{
	Vertex vertices[];
};

layout (push_constant) uniform PushConstant
{
	VertexBuffer vertexBuffer;
} PushConstants;

layout (set = 1, binding = 1) uniform ViewMatrices
{
	mat4 u_ViewMatrix[6];
};

void main()
{
	gl_Layer = gl_InstanceIndex;
	Vertex vertex = PushConstants.vertexBuffer.vertices[gl_VertexIndex];
	v_DirectionVector = vertex.Position.xyz;
	gl_Position = u_ViewMatrix[gl_InstanceIndex] * vertex.Position; // already in ndc no need for a projection matrix
}

#endif

#ifdef FRAGMENT_SHADER

layout (location = 0) out vec4 o_Color;

layout (location = 0) in vec3 v_DirectionVector;

layout (set = 1, binding = 0) uniform sampler2D u_EquirectangularTexture;

vec2 CubemapDirectionToEquirectangularCoords(vec3 directionVector)
{
	vec2 uv = vec2(atan(directionVector.z, directionVector.x), asin(directionVector.y));
	uv *= vec2(0.1591, 0.3183); // scale by vec2(1 / 2PI, 1 / PI) in order to get the values between -1/2 and 1/2
	uv += 0.5;
	return uv;
}

void main()
{
	vec2 uv = CubemapDirectionToEquirectangularCoords(normalize(v_DirectionVector));
	o_Color = texture(u_EquirectangularTexture, uv);
}

#endif