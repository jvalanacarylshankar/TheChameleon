#define _USE_MATH_DEFINES

// header
#include "particles.hpp"

// stdlib
#include <cmath>
#include <iostream>

constexpr int NUM_SEGMENTS = 12;

constexpr float PARTICLE_SPEED = 25;
constexpr int RADIUS = 2;

bool Particles::init()
{
	std::vector<GLfloat> screen_vertex_buffer_data;
	constexpr float z = -0.1;

	for (int i = 0; i < NUM_SEGMENTS; i++)
	{
		screen_vertex_buffer_data.push_back(std::cos(float(i) / (float)NUM_SEGMENTS));
		screen_vertex_buffer_data.push_back(std::sin(M_PI * float(i) / (float)NUM_SEGMENTS));
		screen_vertex_buffer_data.push_back(z);

		screen_vertex_buffer_data.push_back(std::cos(M_PI * 2.0 * float(i + 1) / (float)NUM_SEGMENTS));
		screen_vertex_buffer_data.push_back(std::sin(M_PI * 2.0 * float(i + 1) / (float)NUM_SEGMENTS));
		screen_vertex_buffer_data.push_back(z);

		screen_vertex_buffer_data.push_back(0);
		screen_vertex_buffer_data.push_back(0);
		screen_vertex_buffer_data.push_back(z);
	}

	// clear errors
	gl_flush_errors();

	// vertex buffer creation
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, screen_vertex_buffer_data.size() * sizeof(GLfloat), screen_vertex_buffer_data.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &m_instance_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_instance_vbo);

	if (gl_has_errors())
		return false;

	// load shaders
	if (!effect.load_from_file(shader_path("particle.vs.glsl"), shader_path("particle.fs.glsl")))
		return false;

	return true;
}

// release all graphics resources
void Particles::destroy()
{
	m_particles.clear();

	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &m_instance_vbo);

	effect.release();
}

void Particles::update(float ms)
{
	for (auto &particle : m_particles)
	{
		float dt = ms / 1000;
		float step_x = 1.0 * particle.velocity.x * dt;
		float step_y = 1.0 * particle.velocity.y * dt;
		particle.position.x += step_x;
		particle.position.y += step_y;
	}
}

void Particles::spawn_particle(vec2 position, int dir)
{
	int off_x_1 = 1;
	int off_x_2 = 1;
	int off_y_1 = 1;
	int off_y_2 = 1;

	// randomize speed
	// float velocity = (float(rand()) / (float(RAND_MAX) / (800.f - 500.f))) + 500.f;

	if (dir == 0)
	{
		off_x_2 = -1;
	}
	else if (dir == 1)
	{
		off_x_2 = off_y_1 = off_y_2 = -1;
	}
	else if (dir == 2)
	{
		off_y_2 = -1;
	}
	else if (dir == 3)
	{
		off_x_1 = off_x_2 = off_y_2 = -1;
	}

	Particle particle_1;
	particle_1.position.x = position.x;
	particle_1.position.y = position.y;
	particle_1.radius = RADIUS;
	particle_1.velocity.x = off_x_1 * PARTICLE_SPEED;
	particle_1.velocity.y = off_y_1 * PARTICLE_SPEED;
	m_particles.emplace_back(particle_1);

	Particle particle_2;
	particle_2.position.x = position.x;
	particle_2.position.y = position.y;
	particle_2.radius = RADIUS;
	particle_2.velocity.x = off_x_2 * PARTICLE_SPEED;
	particle_2.velocity.y = off_y_2 * PARTICLE_SPEED;
	m_particles.emplace_back(particle_2);

	Particle particle_3;
	particle_3.position.x = position.x + 8;
	particle_3.position.y = position.y + 8;
	particle_3.radius = RADIUS;
	particle_3.velocity.x = off_x_1 * PARTICLE_SPEED;
	particle_3.velocity.y = off_y_1 * PARTICLE_SPEED;
	m_particles.emplace_back(particle_3);

	Particle particle_4;
	particle_4.position.x = position.x + 8;
	particle_4.position.y = position.y + 8;
	particle_4.radius = RADIUS;
	particle_4.velocity.x = off_x_2 * PARTICLE_SPEED;
	particle_4.velocity.y = off_y_2 * PARTICLE_SPEED;
	m_particles.emplace_back(particle_4);

	set_fade(1);
}

void Particles::draw(const mat3 &projection)
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
	GLint color_uloc = glGetUniformLocation(effect.program, "fcolor");
	GLint fade_particle_uloc = glGetUniformLocation(effect.program, "m_fade_particle");
	GLuint fade_timer_uloc = glGetUniformLocation(effect.program, "fade_timer");

	// particle color
	float color[] = {0.4f, 0.4f, 0.4f};
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float *)&projection);
	glUniform1iv(fade_particle_uloc, 1, &m_fade_particle);
	glUniform1f(fade_timer_uloc, (m_fade_time > 0) ? (float)((glfwGetTime() - m_fade_time) * 20.0f) : -1);

	// draw the screen texture on the geometry
	// set vertices
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);

	// mesh vertex positions
	// bind to attribute 0 (in_position) as in the vertex shader
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
	glVertexAttribDivisor(0, 0);

	// load up particles into buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_instance_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_particles.size() * sizeof(Particle), m_particles.data(), GL_DYNAMIC_DRAW);

	// particle translations
	// bind to attribute 1 (in_translate) as in vertex shader
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Particle), (GLvoid *)offsetof(Particle, position));
	glVertexAttribDivisor(1, 1);

	// particle radii
	// bind to attribute 2 (in_scale) as in vertex shader
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (GLvoid *)offsetof(Particle, radius));
	glVertexAttribDivisor(2, 1);

	// draw using instancing
	glDrawArraysInstanced(GL_TRIANGLES, 0, NUM_SEGMENTS * 3, m_particles.size());

	// reset divisor
	glVertexAttribDivisor(1, 0);
	glVertexAttribDivisor(2, 0);
}

// fade particle
void Particles::set_fade(int val)
{
	m_fade_particle = val;
	m_fade_time = glfwGetTime();
}

void Particles::reset_fade_time()
{
	m_fade_time = glfwGetTime();
}

int Particles::get_fade()
{
	return m_fade_particle;
}

float Particles::get_fade_time() const
{
	return glfwGetTime() - m_fade_time;
}