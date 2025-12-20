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

layout (set = 1, binding = 1) uniform DynamicPassData
{
	mat4 u_ViewMatrix[6];
    float u_Roughness;
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

layout (set = 1, binding = 1) uniform DynamicPassData
{
	mat4 u_ViewMatrix[6];
    float u_Roughness;
};

float RadicalInverse_VdC(uint bits);
vec2 Hammersley(uint i, uint sampleCount);
vec3 ImportanceSampleGGX(vec2 Xi, vec3 normal, float roughness);

const float PI = 3.14159265359;

void main()
{
    vec3 normal = normalize(v_DirectionVector);

    const uint sampleCount = 1024;
    float totalWeight = 0.0;   
    vec3 prefilteredColor = vec3(0.0);

    for(uint i = 0; i < sampleCount; i++)
    {
        vec2 Xi = Hammersley(i, sampleCount);
        vec3 halfwayDirection  = ImportanceSampleGGX(Xi, normal, u_Roughness);
        vec3 lightDirection  = normalize(2.0 * dot(normal, halfwayDirection) * halfwayDirection - normal);

        float NdotL = max(dot(normal, lightDirection), 0.0);
        if(NdotL > 0.0)
        {
            prefilteredColor += texture(u_EnvironmentMap, lightDirection).rgb * NdotL;
            totalWeight      += NdotL;
        }
    }

    o_Color = vec4(prefilteredColor / totalWeight, 1.0);
}

float RadicalInverse_VdC(uint bits) 
{
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

vec2 Hammersley(uint i, uint N)
{
    return vec2(float(i)/float(N), RadicalInverse_VdC(i));
}  

vec3 ImportanceSampleGGX(vec2 Xi, vec3 N, float roughness)
{
    float a = roughness*roughness;
	
    float phi = 2.0 * PI * Xi.x;
    float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
    float sinTheta = sqrt(1.0 - cosTheta*cosTheta);
	
    // from spherical coordinates to cartesian coordinates
    vec3 H;
    H.x = cos(phi) * sinTheta;
    H.y = sin(phi) * sinTheta;
    H.z = cosTheta;
	
    // from tangent-space vector to world-space sample vector
    vec3 up        = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
    vec3 tangent   = normalize(cross(up, N));
    vec3 bitangent = cross(N, tangent);
	
    vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
    return normalize(sampleVec);
}  

#endif