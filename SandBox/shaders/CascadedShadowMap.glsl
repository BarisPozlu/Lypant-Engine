#ifdef VERTEX_SHADER

layout (location = 0) in vec4 a_Position;

uniform mat4 u_ModelMatrix;

void main()
{
	gl_Position = u_ModelMatrix * a_Position;
}

#endif

#ifdef GEOMETRY_SHADER

layout (triangles, invocations = 5) in;
layout (triangle_strip, max_vertices = 3) out;

uniform mat4 u_DirectionalLightSpaceMatrices[5];

void main()
{
	gl_Layer = gl_InvocationID;

	for (int i = 0; i < 3; i++)
	{
		gl_Position = u_DirectionalLightSpaceMatrices[gl_InvocationID] * gl_in[i].gl_Position;
		EmitVertex();
	}

	EndPrimitive();
}

#endif

#ifdef FRAGMENT_SHADER

void main()
{

}

#endif