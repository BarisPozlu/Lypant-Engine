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

layout (location = 0) out vec3 o_Color;

in vec2 v_TexCoord;

uniform sampler2D u_SourceTexture;
uniform vec2 u_TexelSize;

void main()
{
    float x = u_TexelSize.x;
    float y = u_TexelSize.y;

    // Take 13 samples around current texel ('e' being the current texel)
    // a - b - c
    // - j - k -
    // d - e - f
    // - l - m -
    // g - h - i

    vec3 a = texture(u_SourceTexture, vec2(v_TexCoord.x - 2*x, v_TexCoord.y + 2*y)).rgb;
    vec3 b = texture(u_SourceTexture, vec2(v_TexCoord.x,       v_TexCoord.y + 2*y)).rgb;
    vec3 c = texture(u_SourceTexture, vec2(v_TexCoord.x + 2*x, v_TexCoord.y + 2*y)).rgb;

    vec3 d = texture(u_SourceTexture, vec2(v_TexCoord.x - 2*x, v_TexCoord.y)).rgb;
    vec3 e = texture(u_SourceTexture, vec2(v_TexCoord.x,       v_TexCoord.y)).rgb;
    vec3 f = texture(u_SourceTexture, vec2(v_TexCoord.x + 2*x, v_TexCoord.y)).rgb;

    vec3 g = texture(u_SourceTexture, vec2(v_TexCoord.x - 2*x, v_TexCoord.y - 2*y)).rgb;
    vec3 h = texture(u_SourceTexture, vec2(v_TexCoord.x,       v_TexCoord.y - 2*y)).rgb;
    vec3 i = texture(u_SourceTexture, vec2(v_TexCoord.x + 2*x, v_TexCoord.y - 2*y)).rgb;

    vec3 j = texture(u_SourceTexture, vec2(v_TexCoord.x - x, v_TexCoord.y + y)).rgb;
    vec3 k = texture(u_SourceTexture, vec2(v_TexCoord.x + x, v_TexCoord.y + y)).rgb;
    vec3 l = texture(u_SourceTexture, vec2(v_TexCoord.x - x, v_TexCoord.y - y)).rgb;
    vec3 m = texture(u_SourceTexture, vec2(v_TexCoord.x + x, v_TexCoord.y - y)).rgb;

    o_Color = e*0.125;
    o_Color += (a+c+g+i)*0.03125;
    o_Color += (b+d+f+h)*0.0625;
    o_Color += (j+k+l+m)*0.125;
}

#endif