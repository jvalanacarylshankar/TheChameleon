#version 330 

// input attributes
in vec3 in_position;
in vec2 in_texcoord;

// passed to fragment shader
out vec2 texcoord;

//
out vec2 vpos;

// application data
uniform mat3 transform;
uniform mat3 projection;

//
uniform bool is_alive;

// stealth 
uniform bool stealthing;
uniform float stealthing_anim_time;

void main()
{
	texcoord = in_texcoord;

	vpos = in_position.xy;
    
    if (stealthing)
    {
        vpos.x = vpos.x + (3 * sin((stealthing_anim_time * (vpos.y))/5000));
    }

	vec3 pos = projection * transform * vec3(vpos, 1.0);
	gl_Position = vec4(pos.xy, 0.10, 1.0);
}