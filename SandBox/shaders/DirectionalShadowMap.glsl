#ifdef VERTEX_SHADER

layout (location = 0) in vec4 a_Position;

out int v_Layer;

uniform mat4 u_ModelMatrix;

void main()
{
	v_Layer = gl_InstanceID;
	gl_Position = u_ModelMatrix * a_Position;
}

#endif

#ifdef GEOMETRY_SHADER

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in int v_Layer[3];

uniform mat4 u_SpotLightSpaceMatrices[8];

void main()
{
	gl_Layer = v_Layer[0];

	for (int i = 0; i < 3; i++)
	{
		gl_Position = u_SpotLightSpaceMatrices[gl_Layer] * gl_in[i].gl_Position;
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