#version 330 

// Input attributes
in vec3 in_position;
in vec2 in_texcoord;

// Passed to fragment shader
out vec2 texcoord;

// Application data
uniform mat3 transform;
uniform mat3 projection;

// Spotter vision
out vec3 world_coords;

void main()
{
	texcoord = in_texcoord;
    world_coords = transform * vec3(in_position.xy, 1.0);
	vec3 pos = projection * world_coords;
	gl_Position = vec4(pos.xy, 0.9, 1.0);
} 