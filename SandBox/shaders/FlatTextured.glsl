#ifdef VERTEX_SHADER

layout (location = 0) in vec4 a_Position;
layout (location = 3) in vec2 a_TexCoord;

out vec2 v_TexCoord;

layout (std140, binding = 4) uniform Camera
{
	mat4 u_VP;
	vec3 u_ViewPosition;
};

uniform mat4 u_ModelMatrix;

void main()
{
	v_TexCoord = a_TexCoord;
	gl_Position = u_VP * u_ModelMatrix * a_Position;
}

#endif

#ifdef FRAGMENT_SHADER

layout (location = 0) out vec4 o_Color;

in vec2 v_TexCoord;

uniform sampler2D u_AlbedoMap;

void main()
{
	o_Color = texture(u_AlbedoMap, v_TexCoord);
}

#endif