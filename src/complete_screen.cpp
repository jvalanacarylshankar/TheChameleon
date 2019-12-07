// header
#include "complete_screen.hpp"

// stdlib
#include <algorithm>
#include <cmath>

Texture CompleteScreen::pointer;
Texture CompleteScreen::game_done;
Texture CompleteScreen::main_menu;
Texture CompleteScreen::quit;
Texture CompleteScreen::piere_win;

bool CompleteScreen::init()
{
	// load shared texture
	if (!pointer.is_valid())
	{
		if (!pointer.load_from_file(textures_path("pointer.png")))
		{
			fprintf(stderr, "Failed to load pointer texture!");
			return false;
		}
	}

	if (!game_done.is_valid())
	{
		if (!game_done.load_from_file(textures_path("congratulations.png")))
		{
			fprintf(stderr, "Failed to load game done texture!");
			return false;
		}
	}

	if (!main_menu.is_valid())
	{
		if (!main_menu.load_from_file(textures_path("main_menu.png")))
		{
			fprintf(stderr, "Failed to load main menu texture!");
			return false;
		}
	}

	if (!quit.is_valid())
	{
		if (!quit.load_from_file(textures_path("quit.png")))
		{
			fprintf(stderr, "Failed to load quit texture!");
			return false;
		}
	}

	if (!piere_win.is_valid())
	{
		if (!piere_win.load_from_file(textures_path("piere_win.png")))
		{
			fprintf(stderr, "Failed to load piere win texture!");
			return false;
		}
	}

	// the position corresponds to the center of the texture
	float wr = std::max(SCREEN_WIDTH, SCREEN_HEIGHT) * 0.5f;
	float hr = std::max(SCREEN_WIDTH, SCREEN_HEIGHT) * 0.5f;

	TexturedVertex vertices[4];
	vertices[0].position = {-wr, +hr, -0.0f};
	vertices[0].texcoord = {0.f, 1.f};
	vertices[1].position = {+wr, +hr, -0.02f};
	vertices[1].texcoord = {1.f, 1.f};
	vertices[2].position = {+wr, -hr, -0.02f};
	vertices[2].texcoord = {1.f, 0.f};
	vertices[3].position = {-wr, -hr, -0.02f};
	vertices[3].texcoord = {0.f, 0.f};

	uint16_t indices[] = {0, 3, 1, 1, 3, 2};

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

	motion.position = vec2({0.f, 0.f});

	physics.scale = vec2({1.0f, 1.0f});

	return true;
}

// release all graphics resources
void CompleteScreen::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteVertexArrays(1, &mesh.vao);

	effect.release();
}

void CompleteScreen::update(unsigned int state)
{
	if (state == 2)
		pointer_pos = vec2({SCREEN_WIDTH * 1.3f / 2.5f, 2 * (SCREEN_HEIGHT / 4.f)});
	else if (state == 3)
		pointer_pos = vec2({SCREEN_WIDTH * 1.3f / 2.5f, 3 * (SCREEN_HEIGHT / 4.f)});
}

// TODO -- remove hardcoded pos
void CompleteScreen::draw(const mat3 &proj)
{
	// pointer
	vec2 pointer_scale = vec2({pointer.width / (8 * SCREEN_WIDTH), pointer.height / (8 * SCREEN_WIDTH)});
	//vec2 m_pointer_pos
	draw_element(proj, pointer, pointer_pos, pointer_scale);

	// game done
	vec2 game_done_pos = vec2({SCREEN_WIDTH / 2.f, 1 * (SCREEN_HEIGHT / 4.f)});
	vec2 game_done_scale = vec2({game_done.width * 1.5f / (2 * SCREEN_WIDTH), game_done.height * 1.5f / (2 * SCREEN_WIDTH)});
	draw_element(proj, game_done, game_done_pos, game_done_scale);

	// piere win over
	vec2 piere_win_scale = vec2({piere_win.width / (2.2f * SCREEN_WIDTH), piere_win.height / (2.0f * SCREEN_WIDTH)});
	vec2 piere_win_pos = vec2({300.f, SCREEN_HEIGHT / 1.5f});
	draw_element(proj, piere_win, piere_win_pos, piere_win_scale);

	// main menu
	vec2 main_menu_pos = vec2({850.f, 2 * (SCREEN_HEIGHT / 4.f)});
	vec2 main_menu_scale = vec2({main_menu.width / (2 * SCREEN_WIDTH), main_menu.height / (2 * SCREEN_WIDTH)});
	draw_element(proj, main_menu, main_menu_pos, main_menu_scale);

	// quit
	vec2 quit_pos = vec2({850.f, 3 * (SCREEN_HEIGHT / 4.f)});
	vec2 quit_scale = vec2({quit.width / (2 * SCREEN_WIDTH), quit.height / (2 * SCREEN_WIDTH)});
	draw_element(proj, quit, quit_pos, quit_scale);
}

void CompleteScreen::draw_element(const mat3& proj, const Texture& texture, vec2 pos, vec2 scale)
{
	// transformation
	transform.begin();
	transform.translate(pos);
	transform.scale(scale);
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
	glBindTexture(GL_TEXTURE_2D, texture.id);

	// set uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float *)&transform.out);
	float color[] = {1.f, 1.f, 1.f};
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float *)&proj);

	// draw
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}