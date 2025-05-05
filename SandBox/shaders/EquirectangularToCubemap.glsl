#ifdef VERTEX_SHADER

layout (location = 0) in vec3 a_Position;

out vec3 v_DirectionVector;

uniform mat4 u_ViewMatrix;

void main()
{
	v_DirectionVector = vec3(a_Position.xy, -a_Position.z);
	gl_Position = u_ViewMatrix * vec4(a_Position, 1.0); // already in ndc no need for a projection matrix
}

#endif

#ifdef FRAGMENT_SHADER

layout (location = 0) out vec4 o_Color;

in vec3 v_DirectionVector;

uniform sampler2D u_EquirectangularTexture;

vec2 CubemapDirectionToEquirectangularCoords(vec3 directionVector)
{
	vec2 uv = vec2(atan(directionVector.z, directionVector.x), asin(directionVector.y));
	uv *= vec2(0.1591, 0.3183); // scale by vec2(1 / 2PI, 1 / PI) in order to get the values between -1/2 and 1/2
	uv += 0.5;
	return uv;
}

void main()
{
	vec2 uv = CubemapDirectionToEquirectangularCoords(normalize(v_DirectionVector));
	o_Color = texture(u_EquirectangularTexture, uv);
}

#endif