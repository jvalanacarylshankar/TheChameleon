#version 330

uniform sampler2D screen_texture;
uniform float time;
uniform float dead_timer;
uniform float m_alert_mode;
uniform float m_oscillation_value;
uniform int m_cooldown;
uniform int m_max_cooldown;

uniform int window_width;
uniform int window_height;

in vec2 uv;

layout(location = 0) out vec4 color;

vec4 color_shift(vec4 in_color) 
{
	float width = window_width - 300 -100;
	float decrement = width / m_max_cooldown; // Max cooldown set to 50
	float off = m_cooldown * decrement;
	float bar_width = window_width - 300 - off;
	if (m_cooldown == 0)
		bar_width = 100;
	vec4 color = in_color;
		if (gl_FragCoord.x < bar_width && gl_FragCoord.x > 100 && gl_FragCoord.y > 50 && gl_FragCoord.y < 60)
			color = vec4(1.0, 0.0, 0.0, 0.5);
	return color;
}

void main()
{
	vec2 coord = uv;
	if (m_alert_mode == 1.0)
	{
		if (uv.y > 0.5)
		{
			color = vec4(0.8, 0.0, 0.0, uv.y - m_oscillation_value);
		}
		if (uv.y < 0.5)
		{
			color = vec4(0.8, 0.0, 0.0, (1.0 - uv.y) - m_oscillation_value);
		}
	} else {
		if (uv.y > 0.5)
		{
			color = vec4(0.0, 0.0, 0.0, uv.y - 0.1);
		}
		if (uv.y < 0.5)
		{
			color = vec4(0.0, 0.0, 0.0, (1.0 - uv.y) - 0.1);
		}
	}

	// red line

	// vec4 in_color = texture(screen_texture, coord);
	color = color_shift(color);
}