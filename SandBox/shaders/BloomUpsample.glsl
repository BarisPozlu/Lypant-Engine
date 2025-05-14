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
uniform float u_FilterRadius;
uniform float u_AspectRatio;

void main()
{
    float x = u_FilterRadius;
    float y = u_FilterRadius * u_AspectRatio;

    // Take 9 samples around current texel ('e' being the current texel)
    // a - b - c
    // d - e - f
    // g - h - i

    vec3 a = texture(u_SourceTexture, vec2(v_TexCoord.x - x, v_TexCoord.y + y)).rgb;
    vec3 b = texture(u_SourceTexture, vec2(v_TexCoord.x,     v_TexCoord.y + y)).rgb;
    vec3 c = texture(u_SourceTexture, vec2(v_TexCoord.x + x, v_TexCoord.y + y)).rgb;

    vec3 d = texture(u_SourceTexture, vec2(v_TexCoord.x - x, v_TexCoord.y)).rgb;
    vec3 e = texture(u_SourceTexture, vec2(v_TexCoord.x,     v_TexCoord.y)).rgb;
    vec3 f = texture(u_SourceTexture, vec2(v_TexCoord.x + x, v_TexCoord.y)).rgb;

    vec3 g = texture(u_SourceTexture, vec2(v_TexCoord.x - x, v_TexCoord.y - y)).rgb;
    vec3 h = texture(u_SourceTexture, vec2(v_TexCoord.x,     v_TexCoord.y - y)).rgb;
    vec3 i = texture(u_SourceTexture, vec2(v_TexCoord.x + x, v_TexCoord.y - y)).rgb;

    // Apply weighted distribution, by using a 3x3 tent filter:
    //  1   | 1 2 1 |
    // -- * | 2 4 2 |
    // 16   | 1 2 1 |
    o_Color = e*4.0;
    o_Color += (b+d+f+h)*2.0;
    o_Color += (a+c+g+i);
    o_Color *= 1.0 / 16.0;
}

#endif