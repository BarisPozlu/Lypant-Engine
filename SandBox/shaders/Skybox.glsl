#ifdef VERTEX_SHADER

layout (location = 0) in vec3 a_Position;

out vec3 v_DirectionVector;

layout (std140, binding = 4) uniform Camera
{
	mat4 u_VP;
	vec3 u_ViewPosition;
};

void main()
{
	v_DirectionVector = vec3(a_Position.xy, -a_Position.z);
	vec4 position = u_VP * (vec4(a_Position + u_ViewPosition, 1.0));
	gl_Position = position.xyww;
}

#endif

#ifdef FRAGMENT_SHADER

layout (location = 0) out vec4 o_Color;

in vec3 v_DirectionVector;

uniform samplerCube u_Cubemap;

void main()
{
	o_Color = texture(u_Cubemap, v_DirectionVector);
}

#endif