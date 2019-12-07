#define _USE_MATH_DEFINES

// header
#include "bullets.hpp"

#include <cmath>
#include <iostream>

constexpr size_t NUM_SEGMENTS = 12;
constexpr float LIFE = 5000.f;

using namespace std;

bool Bullets::init()
{
	std::vector<GLfloat> screen_vertex_buffer_data;
	constexpr float z = -0.1;

	for (int i = 0; i < NUM_SEGMENTS; i++)
	{
		screen_vertex_buffer_data.push_back(std::cos(M_PI * 2.0 * float(i) / (float)NUM_SEGMENTS));
		screen_vertex_buffer_data.push_back(std::sin(M_PI * 2.0 * float(i) / (float)NUM_SEGMENTS));
		screen_vertex_buffer_data.push_back(z);

		screen_vertex_buffer_data.push_back(std::cos(M_PI * 2.0 * float(i + 1) / (float)NUM_SEGMENTS));
		screen_vertex_buffer_data.push_back(std::sin(M_PI * 2.0 * float(i + 1) / (float)NUM_SEGMENTS));
		screen_vertex_buffer_data.push_back(z);

		screen_vertex_buffer_data.push_back(0);
		screen_vertex_buffer_data.push_back(0);
		screen_vertex_buffer_data.push_back(z);
	}

	// clearing errors
	gl_flush_errors();

	// vertex Buffer creation
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, screen_vertex_buffer_data.size() * sizeof(GLfloat), screen_vertex_buffer_data.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &m_instance_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_instance_vbo);

	if (gl_has_errors())
		return false;

	// loading shaders
	if (!effect.load_from_file(shader_path("bullet.vs.glsl"), shader_path("bullet.fs.glsl")))
		return false;

	return true;
}

// releases all graphics resources
void Bullets::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &m_instance_vbo);

	m_bullets.clear();

	effect.release();
}

void Bullets::update(float ms)
{
	vec2 g = {0, 9.8f};
	int count = 1;

	for (auto &b : m_bullets)
	{
		// s = ut + 1/2at^2
		if (b.life <= 0)
		{
			if (m_bullets.size() == count)
				m_bullets.pop_back();
			else
				m_bullets.erase(m_bullets.begin() + count);
		}
		else
		{
			b.life -= ms;
			b.position.x += b.velocity.x * (ms / 100) + 0.5f * g.x * pow(ms / 100, 2);
			b.position.y += b.velocity.y * (ms / 100) + 0.5f * g.y * pow(ms / 100, 2);
			count++;
		}
	}
}

void Bullets::draw(const mat3 &projection)
{
	// set shaders
	glUseProgram(effect.program);

	// enable alpha channel for textures
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// depth
	glEnable(GL_DEPTH_TEST);

	// get uniform locations
	GLint projection_uloc = glGetUniformLocation(effect.program, "projection");
	GLint color_uloc = glGetUniformLocation(effect.program, "color");

	// color
	float color[] = {1.f, 1.f, 1.f};
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float *)&projection);

	// draw the screen texture on the geometry
	// set vertices
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);

	// mesh vertex positions
	// bind to attribute 0 (in_position) as in the vertex shader
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
	glVertexAttribDivisor(0, 0);

	// load up bullet into buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_instance_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_bullets.size() * sizeof(Bullet), m_bullets.data(), GL_DYNAMIC_DRAW);

	// bullet translations
	// bind to attribute 1 (in_translate) as in vertex shader
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Bullet), (GLvoid *)offsetof(Bullet, position));
	glVertexAttribDivisor(1, 1);

	// bullet radii
	// bind to attribute 2 (in_scale) as in vertex shader
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Bullet), (GLvoid *)offsetof(Bullet, radius));
	glVertexAttribDivisor(2, 1);

	// draw
	glDrawArraysInstanced(GL_TRIANGLES, 0, NUM_SEGMENTS * 3, m_bullets.size());

	// reset divisor
	glVertexAttribDivisor(1, 0);
	glVertexAttribDivisor(2, 0);
}

void Bullets::spawn_bullet(vec2 pos, float rad)
{
	Bullet b;
	b.life = LIFE;
	b.position = pos;
	b.radius = 2;
	b.velocity = {20 * cos(rad), 20 * sin(rad)};

	m_bullets.emplace_back(b);
}