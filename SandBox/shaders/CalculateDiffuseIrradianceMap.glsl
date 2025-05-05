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

uniform samplerCube u_EnvironmentMap;

const float PI = 3.14159265359;

void main()
{
	vec3 normal = normalize(v_DirectionVector);
	
	vec3 irradiance = vec3(0.0);
	
	vec3 up    = vec3(0.0, 1.0, 0.0);
	vec3 right = normalize(cross(up, normal));
	up         = normalize(cross(normal, right));
	
	float sampleDelta = 0.025;
	float numberOfSamples = 0.0;

	for (float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
	{
		for (float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
		{
			// spherical to cartesian (tangent space)
			vec3 tangentSample = vec3(sin(theta) * cos(phi),  sin(theta) * sin(phi), cos(theta));
			// tangent space to world
			vec3 worldSample = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal; 

			irradiance += texture(u_EnvironmentMap, worldSample).rgb * cos(theta) * sin(theta);
			numberOfSamples++;
		}
	}

	irradiance = PI * irradiance / numberOfSamples;
	o_Color = vec4(irradiance, 1.0);
}

#endif