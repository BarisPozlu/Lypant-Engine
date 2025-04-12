#ifdef VERTEX_SHADER

layout (location = 0) in vec4 a_Position;
layout (location = 1) in vec2 a_TexCoord;

out vec2 v_TexCoord;

void main()
{
	v_TexCoord = a_TexCoord;
	gl_Position = a_Position;
}

#endif

#ifdef FRAGMENT_SHADER

layout (location = 0) out vec4 o_Color;

in vec2 v_TexCoord;

uniform sampler2D u_SceneTexture;
uniform float u_Exposure;

void main()
{
	vec3 color = texture(u_SceneTexture, v_TexCoord).rgb;
	color = vec3(1.0) - exp(-color * u_Exposure); // tone mapping
	o_Color = vec4(pow(color, vec3(1.0 / 2.2)), 1.0); // gamma correction
}

#endif