// header
#include "start_screen.hpp"

// stdlib
#include <algorithm>
#include <cmath>

Texture StartScreen::start_game;
Texture StartScreen::controls;
Texture StartScreen::levels;
Texture StartScreen::quit;
Texture StartScreen::game_title;
Texture StartScreen::pointer;

bool StartScreen::init()
{
	// load shared texture
	if (!start_game.is_valid())
	{
		if (!start_game.load_from_file(textures_path("start_game.png")))
		{
			fprintf(stderr, "Failed to load start texture!");
			return false;
		}
	}

	if (!pointer.is_valid())
	{
		if (!pointer.load_from_file(textures_path("pointer.png")))
		{
			fprintf(stderr, "Failed to load pointer texture!");
			return false;
		}
	}

	if (!controls.is_valid())
	{
		if (!controls.load_from_file(textures_path("controls.png")))
		{
			fprintf(stderr, "Failed to load controls texture!");
			return false;
		}
	}

	if (!levels.is_valid())
	{
		if (!levels.load_from_file(textures_path("levels.png")))
		{
			fprintf(stderr, "Failed to load levels texture!");
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

	if (!game_title.is_valid())
	{
		if (!game_title.load_from_file(textures_path("game_title.png")))
		{
			fprintf(stderr, "Failed to load title texture!");
			return false;
		}
	}

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
void StartScreen::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteVertexArrays(1, &mesh.vao);

	effect.release();
}

void StartScreen::update(unsigned int state)
{
	if (state == 0)
		pointer_pos = vec2({SCREEN_WIDTH / 2.f, 1 * (SCREEN_HEIGHT / 5)});
	else if (state == 1)
		pointer_pos = vec2({SCREEN_WIDTH / 2.f, 2 * (SCREEN_HEIGHT / 5)});
	else if (state == 2)
		pointer_pos = vec2({SCREEN_WIDTH / 2.f, 3 * (SCREEN_HEIGHT / 5)});
	else if (state == 3)
		pointer_pos = vec2({SCREEN_WIDTH / 2.f, 4 * (SCREEN_HEIGHT / 5)});
}

// -- edit images
// -- remove hardcoded locs
// -- remove hardcoded scale
void StartScreen::draw(const mat3 &projection)
{
	// pointer
	vec2 pointer_scale = vec2({pointer.width / (8 * SCREEN_WIDTH), pointer.height / (8 * SCREEN_WIDTH)});
	//vec2 pointer_pos
	draw_element(projection, pointer, pointer_pos, pointer_scale);

	// start
	vec2 start_game_scale = vec2({start_game.width / (2 * SCREEN_WIDTH), start_game.height / (2 * SCREEN_WIDTH)});
	vec2 start_game_pos = vec2({850.f, 1 * (SCREEN_HEIGHT / 5.f)});
	draw_element(projection, start_game, start_game_pos, start_game_scale);

	// controls
	vec2 controls_scale = vec2({controls.width / (2 * SCREEN_WIDTH), controls.height / (2 * SCREEN_WIDTH)});
	vec2 controls_pos = vec2({850.f, 2 * (SCREEN_HEIGHT / 5.f)});
	draw_element(projection, controls, controls_pos, controls_scale);
	
	// levels
	vec2 levels_scale = vec2({levels.width / (2 * SCREEN_WIDTH), levels.height / (2 * SCREEN_WIDTH)});
	vec2 levels_pos = vec2({850.f, 3 * (SCREEN_HEIGHT / 5.f)});
	draw_element(projection, levels, levels_pos, levels_scale);

	// quit
	vec2 quit_scale = vec2({quit.width / (2 * SCREEN_WIDTH), quit.height / (2 * SCREEN_WIDTH)});
	vec2 quit_pos = vec2({850.f, 4 * (SCREEN_HEIGHT / 5.f)});
	draw_element(projection, quit, quit_pos, quit_scale);

	// game title
	vec2 game_title_scale = vec2({game_title.width / SCREEN_WIDTH, game_title.height / SCREEN_WIDTH});
	vec2 game_title_pos = vec2({350.f, SCREEN_HEIGHT / 2.f});
	draw_element(projection, game_title, game_title_pos, game_title_scale);
}

void StartScreen::draw_element(const mat3& proj, const Texture& texture, vec2 pos, vec2 scale)
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