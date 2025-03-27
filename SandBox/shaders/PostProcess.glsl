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

layout (location = 0) out vec4 color;

in vec2 v_TexCoord;

uniform sampler2D u_SceneTexture;

void main()
{
	vec3 sampledColor = vec3(texture(u_SceneTexture, v_TexCoord));
	color = vec4(pow(sampledColor, vec3(1.0 / 2.2)), 1.0);
}

#endif