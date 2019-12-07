// header
#include "spotter.hpp"

#include <cmath>
#include <string> 
#include <iostream>

// texture
Texture Spotter::spotter_texture;
using namespace std;

const float FOV_RADIANS = 0.39269908169;

bool Spotter::init()
{
	// load shared texture
	if (!spotter_texture.is_valid())
	{
		if (!spotter_texture.load_from_file(textures_path("spotters/spotter.png")))
		{
			fprintf(stderr, "Failed to load spotter texture!");
			return false;
		}
	}

	direction = vec2({ 0.f, -1.f });
	// sprite sheet calculations
	const float tw = spriteWidth / spotter_texture.width;
	const float th = spriteHeight / spotter_texture.height;
	const int numPerRow = spotter_texture.width / spriteWidth;
	const int numPerCol = spotter_texture.height / spriteHeight;
	const float tx = (frameIndex_x % numPerRow - 1) * tw;
	const float ty = (frameIndex_y / numPerCol) * th;

	float posX = 0.f;
	float posY = -35.f;

	TexturedVertex vertices[4];
	vertices[0].position = { posX, posY, -0.0f };
	vertices[0].texcoord = { tx, ty };
	vertices[1].position = { posX + spriteWidth, posY, -0.0f };
	vertices[1].texcoord = { tx + tw, ty };
	vertices[2].position = { posX + spriteWidth, posY + spriteHeight, -0.0f };
	vertices[2].texcoord = { tx + tw, ty + th };
	vertices[3].position = { posX, posY + spriteHeight, -0.0f };
	vertices[3].texcoord = { tx, ty + th };

	// counterclockwise as it's the default opengl front winding direction
	uint16_t indices[] = { 0, 3, 1, 1, 3, 2 };

	// clear errors
	gl_flush_errors();

	// vertex buffer creation
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedVertex) * 4, vertices, GL_STATIC_DRAW);

	// index buffer creation
	glGenBuffers(1, &mesh.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * 6, indices, GL_STATIC_DRAW);

	// vertex array (container for vertex + index buffer)
	glGenVertexArrays(1, &mesh.vao);
	if (gl_has_errors())
		return false;

	// load shaders
	if (!effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl")))
		return false;

	motion.radians = 0.f;
	motion.speed = 0.f;

	physics.scale = { config_scale, config_scale };

	return true;
}

// release all graphics resources
void Spotter::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteVertexArrays(1, &mesh.vao);

	effect.release();
}

void Spotter::update(float ms)
{
	if (spotter_sprite_countdown > 0.f)
		spotter_sprite_countdown -= ms;

	vec2 directions[3] = { {0.f, -1.f}, {1.f, 0.f}, {-1.f, 0.f} };
	// sprite change
	if (spotter_sprite_countdown < 0) {

		spotter_sprite_switch >= 3 ? spotter_sprite_switch = 1 : spotter_sprite_switch++;

		if (frameIndex_x == 1) {
			frameIndex_x = 2;
		}
		else if (frameIndex_x == 2) {
			frameIndex_x = 7;
			frameIndex_y = 0;
		}
		else if (frameIndex_x == 7) {
			frameIndex_x = 1;
			frameIndex_y = 1;
		}

		init();
		direction = directions[spotter_sprite_switch - 1];
		spotter_sprite_countdown = 1500.f;
	}
}

void Spotter::draw(const mat3 &projection)
{
	// transformation
	transform.begin();
	transform.translate(motion.position);
	transform.rotate(motion.radians);
	transform.scale(physics.scale);
	transform.end();

	// set shaders
	glUseProgram(effect.program);

	// enable alpha channel for textures
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// depth
	glEnable(GL_DEPTH_TEST);

	// get uniform locations for glUniform* calls
	GLint transform_uloc = glGetUniformLocation(effect.program, "transform");
	GLint color_uloc = glGetUniformLocation(effect.program, "fcolor");
	GLint projection_uloc = glGetUniformLocation(effect.program, "projection");

	// set vertices and indices
	glBindVertexArray(mesh.vao);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);

	// input data location as in the vertex buffer
	GLint in_position_loc = glGetAttribLocation(effect.program, "in_position");
	GLint in_texcoord_loc = glGetAttribLocation(effect.program, "in_texcoord");
	glEnableVertexAttribArray(in_position_loc);
	glEnableVertexAttribArray(in_texcoord_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void *)0);
	glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void *)sizeof(vec3));

	// enable and binding texture to slot 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, spotter_texture.id);

	// set uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float *)&transform.out);
	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float *)&projection);

	// draw
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

// movement
void Spotter::set_position(vec2 position)
{
	motion.position = position;
}

vec2 Spotter::get_position() const
{
	return motion.position;
}

// collision
vec2 Spotter::get_bounding_box() const
{
	// adjusted to fit sprite sheet changes
	return { std::fabs(physics.scale.x) * spotter_texture.width * 0.5f * 0.00125f, std::fabs(physics.scale.y) * spotter_texture.height * 0.5f * 0.0014285714285f };
}

// detection
// TODO
bool Spotter::is_in_sight(Char &m_char, Map& m)
{
	vec2 char_pos = m_char.get_position();
	vec2 char_vector = vec2{ char_pos.x - motion.position.x, char_pos.y - motion.position.y };

	float dot_product = dot(char_vector, vec2{ -direction.x, -direction.y});
	float magnitude_char = sqrt(pow(char_vector.x, 2) + pow(char_vector.y, 2));

	float angle = acos(dot_product / (magnitude_char));

	bool in_fov = angle <= FOV_RADIANS;

	bool is_wall = true;
	if (magnitude_char <= radius && in_fov && !m_char.is_stealthed()) {
		is_wall = m.check_wall(motion.position, m_char.get_position());
	}

	return (magnitude_char <= radius && in_fov && !(is_wall));
}

// alert
void Spotter::set_alert_mode(bool val)
{
	m_alert_mode = val;
}

void Spotter::reset_direction() 
{
	direction = vec2({0.f, 1.f});
}

// a threshold to allow for some more fov collisions to happen
float Spotter::check_sgn(float value) 
{
	if (value > 0 && value <= 40.f) return 1.0f;
	if (value < 0 && value >= -40.f) return -1.0f;
	else return 0.f;
}