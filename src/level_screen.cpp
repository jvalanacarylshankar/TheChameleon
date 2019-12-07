// header
#include "level_screen.hpp"

// stdlib
#include <algorithm>
#include <cmath>

Texture LevelScreen::tutorial;
Texture LevelScreen::level_1;
Texture LevelScreen::level_2;
Texture LevelScreen::level_3;
Texture LevelScreen::level_4;
Texture LevelScreen::level_5;
Texture LevelScreen::pointer;

bool LevelScreen::init()
{
	// load shared texture
	if (!tutorial.is_valid())
	{
		if (!tutorial.load_from_file(textures_path("level_tutorial.png")))
		{
			fprintf(stderr, "Failed to load level_tutorial texture!");
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

	if (!level_1.is_valid())
	{
		if (!level_1.load_from_file(textures_path("level_1.png")))
		{
			fprintf(stderr, "Failed to load level_1 texture!");
			return false;
		}
	}

	if (!level_2.is_valid())
	{
		if (!level_2.load_from_file(textures_path("level_2.png")))
		{
			fprintf(stderr, "Failed to load level_2 texture!");
			return false;
		}
	}

	if (!level_3.is_valid())
	{
		if (!level_3.load_from_file(textures_path("level_3.png")))
		{
			fprintf(stderr, "Failed to load level_3 texture!");
			return false;
		}
	}

	if (!level_4.is_valid())
	{
		if (!level_4.load_from_file(textures_path("level_4.png")))
		{
			fprintf(stderr, "Failed to load level_4 texture!");
			return false;
		}
	}

	if (!level_5.is_valid())
	{
		if (!level_5.load_from_file(textures_path("level_5.png")))
		{
			fprintf(stderr, "Failed to load level_5 texture!");
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
void LevelScreen::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteVertexArrays(1, &mesh.vao);

	effect.release();
}

void LevelScreen::update(unsigned int state)
{
	if (state == 0)
		m_pointer_pos = vec2({SCREEN_WIDTH / 3.f, 1 * (SCREEN_HEIGHT / 7.f)});
	else if (state == 1)
		m_pointer_pos = vec2({SCREEN_WIDTH / 3.f, 2 * (SCREEN_HEIGHT / 7.f)});
	else if (state == 2)
		m_pointer_pos = vec2({SCREEN_WIDTH / 3.f, 3 * (SCREEN_HEIGHT / 7.f)});
	else if (state == 3)
		m_pointer_pos = vec2({SCREEN_WIDTH / 3.f, 4 * (SCREEN_HEIGHT / 7.f)});
	else if (state == 4)
		m_pointer_pos = vec2({SCREEN_WIDTH / 3.f, 5 * (SCREEN_HEIGHT / 7.f)});
	else if (state == 5)
		m_pointer_pos = vec2({SCREEN_WIDTH / 3.f, 6 * (SCREEN_HEIGHT / 7.f)});
}

// TODO -- remove hardcoded pos
void LevelScreen::draw(const mat3 &proj)
{
	// pointer
	vec2 pointer_scale = vec2({pointer.width / (8 * SCREEN_WIDTH), pointer.height / (8 * SCREEN_WIDTH)});
	//vec2 m_pointer_pos
	draw_element(proj, pointer, m_pointer_pos, pointer_scale);

	// tutorial
	vec2 tutorial_pos = vec2({SCREEN_WIDTH / 2.f, 1 * (SCREEN_HEIGHT / 7.f)});
	vec2 tutorial_scale = vec2({tutorial.width / (2 * SCREEN_WIDTH), tutorial.height / (2 * SCREEN_WIDTH)});
	draw_element(proj, tutorial, tutorial_pos, tutorial_scale);

	//level_1
	vec2 level_1_pos = vec2({SCREEN_WIDTH / 2.f, 2 * (SCREEN_HEIGHT / 7.f)});
	vec2 level_1_scale = vec2({level_1.width / (2 * SCREEN_WIDTH), level_1.height / (2 * SCREEN_WIDTH)});
	draw_element(proj, level_1, level_1_pos, level_1_scale);

	//level_2
	vec2 level_2_pos = vec2({SCREEN_WIDTH / 2.f, 3 * (SCREEN_HEIGHT / 7.f)});
	vec2 level_2_scale = vec2({level_2.width / (2 * SCREEN_WIDTH), level_2.height / (2 * SCREEN_WIDTH)});
	draw_element(proj, level_2, level_2_pos, level_2_scale);

	// level_3
	vec2 level_3_pos = vec2({SCREEN_WIDTH / 2.f, 4 * (SCREEN_HEIGHT / 7.f)});
	vec2 level_3_scale = vec2({level_3.width / (2 * SCREEN_WIDTH), level_3.height / (2 * SCREEN_WIDTH)});
	draw_element(proj, level_3, level_3_pos, level_3_scale);

	// level_4
	vec2 level_4_pos = vec2({ SCREEN_WIDTH / 2.f, 5 * (SCREEN_HEIGHT / 7.f) });
	vec2 level_4_scale = vec2({ level_4.width / (2 * SCREEN_WIDTH), level_4.height / (2 * SCREEN_WIDTH) });
	draw_element(proj, level_4, level_4_pos, level_4_scale);

	// level_5
	vec2 level_5_pos = vec2({ SCREEN_WIDTH / 2.f, 6 * (SCREEN_HEIGHT / 7.f) });
	vec2 level_5_scale = vec2({ level_5.width / (2 * SCREEN_WIDTH), level_5.height / (2 * SCREEN_WIDTH) });
	draw_element(proj, level_5, level_5_pos, level_5_scale);
}

void LevelScreen::draw_element(const mat3& proj, const Texture& texture, vec2 pos, vec2 scale)
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