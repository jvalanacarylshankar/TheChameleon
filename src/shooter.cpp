// header
#include "shooter.hpp"

#include <cmath>
#include <string> 
#include <iostream>

// texture
Texture Shooter::shooter_texture;
using namespace std;

bool Shooter::init()
{
	// load shared texture
	if (!shooter_texture.is_valid())
	{
		if (!shooter_texture.load_from_file(textures_path("survivor-idle_shotgun_0.png")))
		{
			fprintf(stderr, "Failed to load Shooter texture!");
			return false;
		}
	}

	// the position corresponds to the center of the texture
	float wr = shooter_texture.width * 0.5f;
	float hr = shooter_texture.height * 0.5f;

	TexturedVertex vertices[4];
	vertices[0].position = { -wr, +hr, -0.0f };
	vertices[0].texcoord = { 0.f, 1.f };
	vertices[1].position = { +wr, +hr, -0.0f };
	vertices[1].texcoord = { 1.f, 1.f };
	vertices[2].position = { +wr, -hr, -0.0f };
	vertices[2].texcoord = { 1.f, 0.f };
	vertices[3].position = { -wr, -hr, -0.0f };
	vertices[3].texcoord =  {0.f, 0.f };

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

	physics.scale = { config_scale, config_scale };

	m_in_combat = false;

	return true;
}

// release all graphics resources
void Shooter::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteVertexArrays(1, &mesh.vao);

	effect.release();
}

void Shooter::update(float ms)
{
	//
}

void Shooter::draw(const mat3 &projection)
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
	glBindTexture(GL_TEXTURE_2D, shooter_texture.id);

	// set uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float *)&transform.out);
	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float *)&projection);

	// draw
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

// movement
void Shooter::set_position(vec2 pos)
{
	motion.position = pos;
}

vec2 Shooter::get_position() const
{
	return motion.position;
}

void Shooter::set_rotation(float rad)
{
	motion.radians = rad;
}

// alert
void Shooter::set_alert_mode(bool val)
{
	m_alert_mode = val;
}

// combat
void Shooter::set_in_combat(bool val)
{
	m_in_combat = val;
}

bool Shooter::is_in_combat() const
{
	return m_in_combat;
}

// collision
vec2 Shooter::get_bounding_box() const
{
	return { std::fabs(physics.scale.x) * shooter_texture.width * 0.5f, std::fabs(physics.scale.y) * shooter_texture.height * 0.5f };
}