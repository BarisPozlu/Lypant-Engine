#ifdef VERTEX_SHADER

layout (location = 0) in vec4 a_Position;

flat out int v_InstanceID;

uniform mat4 u_ModelMatrix;

void main()
{
	v_InstanceID = gl_InstanceID;
	gl_Position = u_ModelMatrix * a_Position;
}

#endif

#ifdef GEOMETRY_SHADER

layout (triangles) in;
layout (triangle_strip, max_vertices = 18) out;

flat in int v_InstanceID[3];

flat out int g_InstanceID;
out vec4 g_WorldPosition;

uniform mat4 u_PointLightSpaceMatrices[48];

void main()
{
	g_InstanceID = v_InstanceID[0];
	gl_Layer = v_InstanceID[0] * 6;

	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			g_WorldPosition = gl_in[j].gl_Position;
			gl_Position  = u_PointLightSpaceMatrices[gl_Layer] * g_WorldPosition;
			EmitVertex();
		}

		EndPrimitive();
		gl_Layer++;
	}
}

#endif

#ifdef FRAGMENT_SHADER

flat in int g_InstanceID;
in vec4 g_WorldPosition;

uniform vec3 u_PointLightPositions[8];

void main()
{
	// instead of writing the depths to the depth buffer, we are writing the normalized distances from the light
	// the far plane of the shadow projection matrix for point lights is 25 and not modifiable right now
	gl_FragDepth = length(u_PointLightPositions[g_InstanceID] - g_WorldPosition.xyz) / 25.0;
}

#endif