#version 330

// From vertex shader
in vec2 texcoord;

// Application data
uniform sampler2D sampler0;
uniform vec3 fcolor;
uniform int flash_map;
uniform float flash_timer;

// Output color
layout(location = 0) out  vec4 color;

// Spotter Vision
in vec3 world_coords;
uniform vec3 spotter_loc;
uniform vec3 vision_direction;

void main()
{
	color = vec4(fcolor, 1.0) * texture(sampler0, vec2(texcoord.x, texcoord.y));
    
    // flashlight
    vec2 char_vector = vec2( world_coords.x - spotter_loc.x, world_coords.y - spotter_loc.y );
    
    float dot_product = dot(char_vector, vision_direction.xy);
    float distance = length(char_vector);
    float angle = acos(dot_product/distance);
    
    if (distance < 70 && angle < 0.39269908169)
    {
        color = color + (vec4(0.5,0.5,0.5,0.0) * (distance/70));
    }
    
	if (flash_map == 1 && flash_timer > 0)
		if (gl_FragCoord.x < 2400 && gl_FragCoord.y < 1600)
			color += 0.5 * flash_timer * vec4(0.1, 0.1, 0.1, 0);
}