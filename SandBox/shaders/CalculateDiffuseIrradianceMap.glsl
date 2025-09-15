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

layout (set = 1, binding = 0) uniform samplerCube u_EnvironmentMap;

const float PI = 3.14159265359;

void main()
{
	vec3 normal = normalize(v_DirectionVector);
	
	vec3 irradiance = vec3(0.0);
	
	vec3 up    = vec3(0.0, 1.0, 0.0);
	vec3 right = normalize(cross(up, normal));
	up         = normalize(cross(normal, right));
	
	float sampleDelta = 0.025;
	float numberOfSamples = 0.0;

	for (float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
	{
		for (float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
		{
			// spherical to cartesian (tangent space)
			vec3 tangentSample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
			// tangent space to world
			vec3 worldSample = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal; 

			irradiance += texture(u_EnvironmentMap, worldSample).rgb * cos(theta) * sin(theta);
			numberOfSamples++;
		}
	}

	irradiance = PI * irradiance / numberOfSamples;
	o_Color = vec4(irradiance, 1.0);
}

#endif