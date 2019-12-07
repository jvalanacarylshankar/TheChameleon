#version 330

uniform vec3 fcolor;
uniform int m_fade_particle;
uniform float fade_timer;
// Output color
layout(location = 0) out  vec4 color;

void main()
{
	// out_color = vec4(color, 1.0);
	color = vec4(fcolor, 1.0);
	if (m_fade_particle == 1 && fade_timer > 0)
		if (gl_FragCoord.x < 2400 && gl_FragCoord.y < 1600)
			color -= 0.1 * fade_timer * vec4(0.1, 0.1, 0.1, 0);
}
