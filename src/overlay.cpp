// header
#include "overlay.hpp"

// stdlib
#include <iostream>

bool Overlay::init(bool in_alert_mode,  int cd)
{
	m_dead_time = -1;
	glGetIntegerv(GL_VIEWPORT, view_port);
	// Since we are not going to apply transformation to this screen geometry
	// The coordinates are set to fill the standard openGL window [-1, -1 .. 1, 1]
	// Make the size slightly larger then the screen to crop the boundary.
	static const GLfloat screen_vertex_buffer_data[] = {
		-1.05f, -1.05f, 0.0f,
		1.05f, -1.05f, 0.0f,
		-1.05f,  1.05f, 0.0f,
		-1.05f,  1.05f, 0.0f,
		1.05f, -1.05f, 0.0f,
		1.05f,  1.05f, 0.0f,
	};

	// clear errors
	gl_flush_errors();

	// vertex buffer creation
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(screen_vertex_buffer_data), screen_vertex_buffer_data, GL_STATIC_DRAW);

	if (gl_has_errors())
		return false;

	// load shaders
	if (!effect.load_from_file(shader_path("overlay.vs.glsl"), shader_path("overlay.fs.glsl")))
		return false;

	m_alert_mode = in_alert_mode;
	m_oscillation_value = 0.f;

	m_cooldown = 0;
	m_max_cooldown = cd;

	return true;
}

// release all graphics resources
void Overlay::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);

	effect.release();
}

void Overlay::draw(const mat3& projection) {
	// Enabling alpha channel for textures
	glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);

	// Setting shaders
	glUseProgram(effect.program);

	// Set screen_texture sampling to texture unit 0
	// Set clock
	GLuint screen_text_uloc = glGetUniformLocation(effect.program, "screen_texture");
	GLuint time_uloc = glGetUniformLocation(effect.program, "time");
	GLuint dead_timer_uloc = glGetUniformLocation(effect.program, "dead_timer");
	GLuint alert_mode_uloc = glGetUniformLocation(effect.program, "m_alert_mode");
	GLuint oscillation_value_uloc = glGetUniformLocation(effect.program, "m_oscillation_value");
	GLuint cooldown_value_uloc = glGetUniformLocation(effect.program, "m_cooldown");
	GLuint max_cooldown_value_uloc = glGetUniformLocation(effect.program, "m_max_cooldown");

	GLuint window_width_uloc = glGetUniformLocation(effect.program, "window_width");
	GLuint window_height_uloc = glGetUniformLocation(effect.program, "window_height");

	glUniform1i(screen_text_uloc, 0);
	glUniform1f(time_uloc, (float)(glfwGetTime() * 10.0f));
	glUniform1f(alert_mode_uloc, (m_alert_mode) ? 1.f : 0.f);
	glUniform1f(oscillation_value_uloc, m_oscillation_value);
	glUniform1i(cooldown_value_uloc, m_cooldown);
	glUniform1i(max_cooldown_value_uloc, m_max_cooldown);
	glUniform1f(dead_timer_uloc, (m_dead_time > 0) ? (float)((glfwGetTime() - m_dead_time) * 10.0f) : -1);
	glUniform1i(window_width_uloc, view_port[2]);
	glUniform1i(window_height_uloc, view_port[3]);
	// Draw the screen texture on the quad geometry
	// Setting vertices
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);

	// Bind to attribute 0 (in_position) as in the vertex shader
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// Draw
	glDrawArrays(GL_TRIANGLES, 0, 6); // 2*3 indices starting at 0 -> 2 triangles
	glDisableVertexAttribArray(0);
}

void Overlay::oscillation()
{
	if (m_oscillation_value > 0.8f) {
		m_oscillation_value = 0.f;
	}
	m_oscillation_value += 0.05f;
}

void Overlay::set_cooldown(int val)
{
	
	m_cooldown = val;
}

void Overlay::update_alert_mode(bool val)
{
	m_alert_mode = val;
}