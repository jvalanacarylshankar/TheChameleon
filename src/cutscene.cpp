// header
#include "cutscene.hpp"

// stdlib
#include <algorithm>
#include <cmath>
#include <string>
#include <iostream>

Texture Cutscene::texture_dialogue_box;
Texture Cutscene::texture_dialogue_box_left;
Texture Cutscene::texture_dialogue_box_right;
Texture Cutscene::texture_dialogue_box_mid;
Texture Cutscene::enemy_texture;
//Texture Cutscene::skip_texture;
Texture Cutscene::texture_background;

using namespace std;

bool Cutscene::init()
{
	dialogue_counter = 1;
	current_cutscene_state = 4;

	// load shared texture
	if (!texture_dialogue_box.is_valid())
	{
		if (!texture_dialogue_box.load_from_file(textures_path("cutscenes/dialogue_box.png")))
		{
			fprintf(stderr, "Failed to load dialogue texture!");
			return false;
		}
	}

	if (!texture_dialogue_box_left.is_valid())
	{
		if (!texture_dialogue_box_left.load_from_file(textures_path("cutscenes/dialogue_box_face.png")))
		{
			fprintf(stderr, "Failed to load dialogue texture!");
			return false;
		}
	}

	if (!texture_dialogue_box_right.is_valid())
	{
		if (!texture_dialogue_box_right.load_from_file(textures_path("cutscenes/dialogue_box_face.png")))
		{
			fprintf(stderr, "Failed to load dialogue texture!");
			return false;
		}
	}

	if (!texture_dialogue_box_mid.is_valid())
	{
		if (!texture_dialogue_box_mid.load_from_file(textures_path("cutscenes/dialogue_placeholder.png")))
		{
			fprintf(stderr, "Failed to load dialogue texture!");
			return false;
		}
	}

	if (!enemy_texture.is_valid())
	{
		if (!enemy_texture.load_from_file(textures_path("wanderers/1.png")))
		{
			fprintf(stderr, "Failed to load enemy texture!");
			return false;
		}
	}

	if (!texture_background.is_valid())
	{
		if (!texture_background.load_from_file(textures_path("cutscenes/bg_story_0.png")))
		{
			fprintf(stderr, "Failed to load background texture!");
			return false;
		}
	}

	/* if (!skip_texture.is_valid())
	{
		if (!skip_texture.load_from_file(textures_path("skip.png")))
		{
			fprintf(stderr, "Failed to load skip texture!");
			return false;
		}
	} */

	// the position corresponds to the center of the texture
	float wr = std::max(SCREEN_WIDTH, SCREEN_HEIGHT) * 0.5f;
	float hr = std::max(SCREEN_WIDTH, SCREEN_HEIGHT) * 0.5f;

	TexturedVertex vertices[4];
	vertices[0].position = {-wr, +hr, 0.f};
	vertices[0].texcoord = {0.f, 1.f};
	vertices[1].position = {+wr, +hr, 0.f};
	vertices[1].texcoord = {1.f, 1.f};
	vertices[2].position = {+wr, -hr, 0.f};
	vertices[2].texcoord = {1.f, 0.f};
	vertices[3].position = {-wr, -hr, 0.f};
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

	physics.scale = vec2({1.f, 1.f});

	return true;
}

// release all graphics resources
void Cutscene::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteVertexArrays(1, &mesh.vao);

	effect.release();
}

void Cutscene::update() {}

bool Cutscene::is_left_dialogue()
{
	for (int i = 0; i < 43; i++)
	{
		if (left_dialogues[i] == dialogue_counter)
			return true;
	}

	return false;
}

void Cutscene::draw(const mat3 &proj) {}

void Cutscene::draw(const mat3 &proj, vec2 wsize, const vec2 wpoint)
{
	float wscale = current_cutscene_state == LEVEL_TUTORIAL ? (float)SCREEN_WIDTH * ((float)SCREEN_WIDTH / wsize.x) : (float)SCREEN_WIDTH;

	vec2 d_box_trans = vec2({0.f,0.f});
	vec2 d_text_trans = vec2({0.f,0.f});
	vec2 d_face_trans_left = vec2({0.f,0.f});
	vec2 d_face_trans_right = vec2({0.f,0.f});

	vec2 d_box_scale = vec2({(float)texture_dialogue_box.width / wscale, (float)texture_dialogue_box.height / wscale});
	vec2 d_text_scale = vec2({(float)texture_dialogue_box_mid.width / wscale, (float)texture_dialogue_box_mid.height / wscale});
	vec2 d_face_scale = vec2({(float)texture_dialogue_box_left.width / wscale, (float)texture_dialogue_box_left.height / wscale});

	if (current_cutscene_state == LEVEL_TUTORIAL)
	{
		d_box_trans.x = wpoint.x + (float)texture_dialogue_box.width * (d_box_scale.x) / 2;
		d_box_trans.y = wpoint.y + (float)texture_dialogue_box.height * (d_box_scale.y * 3);

		d_text_trans.x = wpoint.x + (float)texture_dialogue_box_mid.width * (d_text_scale.x * 1.1250);
		d_text_trans.y = wpoint.y + (float)texture_dialogue_box_mid.height * (d_text_scale.y * 3);

		d_face_trans_left.x = wpoint.x + (float)texture_dialogue_box_left.width * (d_face_scale.x * 3);
		d_face_trans_left.y = wpoint.y + (float)texture_dialogue_box_left.height * (d_face_scale.y * 3); 
		d_face_trans_right.x = wpoint.x + wsize.x - (float)texture_dialogue_box_left.width * (d_face_scale.x * 3);
		d_face_trans_right.y = wpoint.y + (float)texture_dialogue_box_left.height * (d_face_scale.y * 3);
	}
	else
	{
		d_face_trans_left = vec2({(float)(texture_dialogue_box_left.width / 2), (float)(texture_dialogue_box_left.height / 2)});
		d_text_trans = vec2({(float)(texture_dialogue_box_mid.width / 2) + (float)texture_dialogue_box_left.width, (float)(texture_dialogue_box_mid.height / 2)});
		d_face_trans_right = vec2({(float)(SCREEN_WIDTH - texture_dialogue_box_right.width / 2), (float)(texture_dialogue_box_right.height / 2)});
		d_box_trans = vec2({(float)(texture_dialogue_box.width / 2), (float)(texture_dialogue_box.height / 2)});
	}

	draw_element(proj, texture_dialogue_box_left, d_face_trans_left, d_face_scale);
	draw_element(proj, texture_dialogue_box_mid, d_text_trans, d_text_scale);
	draw_element(proj, texture_dialogue_box_right, d_face_trans_right, d_face_scale);
	draw_element(proj, texture_dialogue_box, d_box_trans, d_box_scale);

	if (current_cutscene_state != LEVEL_TUTORIAL)
		draw_element(proj, texture_background, vec2({(float)(SCREEN_WIDTH / 2), (float)(SCREEN_HEIGHT / 2)}), vec2({1.f,1.f}));
}

void Cutscene::draw_element(const mat3& proj, const Texture& texture, vec2 pos, vec2 scale)
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

bool Cutscene::dialogue_done(unsigned int cutscene_state)
{
	if (cutscene_state == 4 && dialogue_counter == 27)
		return true;
	else if (cutscene_state == LEVEL_TUTORIAL && dialogue_counter == 49)
		return true;
	else if (cutscene_state == LEVEL_1_CUTSCENE && dialogue_counter == 69)
		return true;
	else if (cutscene_state == LEVEL_2_CUTSCENE && dialogue_counter == 80)
		return true;
	else if (cutscene_state == LEVEL_3_CUTSCENE && dialogue_counter == 92)
		return true;

	return false;
}

void Cutscene::increment_dialogue_counter(unsigned int game_state)
{
	dialogue_counter++;
	current_cutscene_state = game_state;

	if (game_state == STORY_SCREEN)
	{
		string temp_str = "data/textures/cutscenes/story/" + to_string(dialogue_counter) + ".png";
		string s(PROJECT_SOURCE_DIR);
		s += temp_str;
		const char *path = s.c_str();

		texture_dialogue_box_mid.~Texture();
		texture_dialogue_box_mid.load_from_file(path);

		if (dialogue_counter == 1)
		{
			texture_dialogue_box_left.~Texture();
			texture_dialogue_box_left.load_from_file(textures_path("cutscenes/spotter.png"));
			texture_dialogue_box_right.~Texture();
			texture_dialogue_box_right.load_from_file(textures_path("cutscenes/wanderer.png"));

			texture_background.~Texture();
			texture_background.load_from_file(textures_path("cutscenes/bg_story_0.png"));
		}
		else if (dialogue_counter == 4)
		{
			texture_dialogue_box_left.~Texture();
			texture_dialogue_box_left.load_from_file(textures_path("cutscenes/dialogue_box_face.png"));
			texture_dialogue_box_right.~Texture();
			texture_dialogue_box_right.load_from_file(textures_path("cutscenes/roger.png"));

			texture_background.~Texture();
			texture_background.load_from_file(textures_path("cutscenes/bg_story_1.png"));
		}
		else if (dialogue_counter == 6)
		{
			texture_dialogue_box_left.~Texture();
			texture_dialogue_box_left.load_from_file(textures_path("cutscenes/pierre.png"));
		}
		else if (dialogue_counter == 14)
		{
			texture_dialogue_box_right.~Texture();
			texture_dialogue_box_right.load_from_file(textures_path("cutscenes/dialogue_box_face.png"));
		}
		else if (dialogue_counter == 18)
		{
			texture_dialogue_box_right.~Texture();
			texture_dialogue_box_right.load_from_file(textures_path("cutscenes/intel.png"));
		}
	}

	else if (game_state == LEVEL_TUTORIAL)
	{
		string temp_str = "data/textures/cutscenes/tutorial/" + to_string(dialogue_counter) + ".png";
		string s(PROJECT_SOURCE_DIR);
		s += temp_str;
		const char *path = s.c_str();

		texture_dialogue_box_mid.~Texture();
		texture_dialogue_box_mid.load_from_file(path);

		if (dialogue_counter == 28)
		{
			texture_dialogue_box_left.~Texture();
			texture_dialogue_box_left.load_from_file(textures_path("cutscenes/pierre.png"));
			texture_dialogue_box_right.~Texture();
			texture_dialogue_box_right.load_from_file(textures_path("cutscenes/intel.png"));
		}
	}

	else if (game_state == LEVEL_1_CUTSCENE)
	{
		string temp_str = "data/textures/cutscenes/level1/" + to_string(dialogue_counter) + ".png";
		string s(PROJECT_SOURCE_DIR);
		s += temp_str;
		const char *path = s.c_str();

		texture_dialogue_box_mid.~Texture();
		texture_dialogue_box_mid.load_from_file(path);

		if (dialogue_counter == 50)
		{
			texture_dialogue_box_left.~Texture();
			texture_dialogue_box_left.load_from_file(textures_path("cutscenes/pierre.png"));
			texture_dialogue_box_right.~Texture();
			texture_dialogue_box_right.load_from_file(textures_path("cutscenes/intel.png"));

			texture_background.~Texture();
			texture_background.load_from_file(textures_path("cutscenes/bg_level1_0.png"));
		}
		else if (dialogue_counter == 58)
		{
			texture_dialogue_box_right.~Texture();
			texture_dialogue_box_right.load_from_file(textures_path("cutscenes/wanderer.png"));
		}
		else if (dialogue_counter == 59)
		{
			texture_dialogue_box_right.~Texture();
			texture_dialogue_box_right.load_from_file(textures_path("cutscenes/intel.png"));
		}
		else if (dialogue_counter == 67)
		{
			texture_dialogue_box_right.~Texture();
			texture_dialogue_box_right.load_from_file(textures_path("cutscenes/dialogue_box_face.png"));
		}
	}

	else if (game_state == LEVEL_2_CUTSCENE)
	{
		string temp_str = "data/textures/cutscenes/level2/" + to_string(dialogue_counter) + ".png";
		string s(PROJECT_SOURCE_DIR);
		s += temp_str;
		const char *path = s.c_str();
		
		texture_dialogue_box_mid.~Texture();
		texture_dialogue_box_mid.load_from_file(path);

		if (dialogue_counter == 70)
		{
			texture_dialogue_box_left.~Texture();
			texture_dialogue_box_left.load_from_file(textures_path("cutscenes/pierre.png"));
			texture_dialogue_box_right.~Texture();
			texture_dialogue_box_right.load_from_file(textures_path("cutscenes/intel.png"));

			//skip_texture.~Texture();
			//skip_texture.load_from_file(textures_path("skip.png"));
			texture_background.~Texture();
			texture_background.load_from_file(textures_path("cutscenes/bg_level2_0.png"));
		}
		else if (dialogue_counter == 76)
		{
			texture_dialogue_box_right.~Texture();
			texture_dialogue_box_right.load_from_file(textures_path("cutscenes/spotter.png"));
		}
		else if (dialogue_counter == 77)
		{
			texture_dialogue_box_right.~Texture();
			texture_dialogue_box_right.load_from_file(textures_path("cutscenes/intel.png"));
		}
	}

	else if (game_state == LEVEL_3_CUTSCENE)
	{
		string temp_str = "data/textures/cutscenes/level3/" + to_string(dialogue_counter) + ".png";
		string s(PROJECT_SOURCE_DIR);
		s += temp_str;
		const char *path = s.c_str();

		texture_dialogue_box_mid.~Texture();
		texture_dialogue_box_mid.load_from_file(path);

		if (dialogue_counter == 81)
		{
			texture_dialogue_box_left.~Texture();
			texture_dialogue_box_left.load_from_file(textures_path("cutscenes/pierre.png"));
			texture_dialogue_box_right.~Texture();
			texture_dialogue_box_right.load_from_file(textures_path("cutscenes/shooter.png"));

			//skip_texture.~Texture();
			//skip_texture.load_from_file(textures_path("skip.png"));
			texture_background.~Texture();
			texture_background.load_from_file(textures_path("cutscenes/bg_level3_0.png"));
		}
		else if (dialogue_counter == 83)
		{
			texture_dialogue_box_right.~Texture();
			texture_dialogue_box_right.load_from_file(textures_path("cutscenes/intel.png"));
		}
	}
}

void Cutscene::set_dialogue_counter(unsigned int cutscene_state, unsigned int counter_value)
{
	dialogue_counter = counter_value;
	current_cutscene_state = cutscene_state;

	if (cutscene_state == STORY_SCREEN && counter_value == 1)
	{
		texture_dialogue_box_mid.~Texture();
		texture_dialogue_box_mid.load_from_file(textures_path("cutscenes/story/1.png"));
		texture_dialogue_box_left.~Texture();
		texture_dialogue_box_left.load_from_file(textures_path("cutscenes/spotter.png"));
		texture_dialogue_box_right.~Texture();
		texture_dialogue_box_right.load_from_file(textures_path("cutscenes/wanderer.png"));

		//skip_texture.~Texture();
		//skip_texture.load_from_file(textures_path("skip.png"));

		texture_background.~Texture();
		texture_background.load_from_file(textures_path("cutscenes/bg_story_0.png"));
	}
	else if (cutscene_state == LEVEL_TUTORIAL && counter_value == 28)
	{
		texture_dialogue_box_mid.~Texture();
		texture_dialogue_box_mid.load_from_file(textures_path("cutscenes/tutorial/28.png"));
		texture_dialogue_box_left.~Texture();
		texture_dialogue_box_left.load_from_file(textures_path("cutscenes/pierre.png"));
		texture_dialogue_box_right.~Texture();
		texture_dialogue_box_right.load_from_file(textures_path("cutscenes/intel.png"));
	}
	else if (cutscene_state == LEVEL_1_CUTSCENE && counter_value == 50)
	{
		texture_dialogue_box_mid.~Texture();
		texture_dialogue_box_mid.load_from_file(textures_path("cutscenes/level1/50.png"));
		texture_dialogue_box_left.~Texture();
		texture_dialogue_box_left.load_from_file(textures_path("cutscenes/pierre.png"));
		texture_dialogue_box_right.~Texture();
		texture_dialogue_box_right.load_from_file(textures_path("cutscenes/intel.png"));

		//skip_texture.~Texture();
		//skip_texture.load_from_file(textures_path("skip.png"));
		texture_background.~Texture();
		texture_background.load_from_file(textures_path("cutscenes/bg_level1_0.png"));
	}
	else if (cutscene_state == LEVEL_2_CUTSCENE && counter_value == 70)
	{
		texture_dialogue_box_mid.~Texture();
		texture_dialogue_box_mid.load_from_file(textures_path("cutscenes/level2/70.png"));
		texture_dialogue_box_left.~Texture();
		texture_dialogue_box_left.load_from_file(textures_path("cutscenes/pierre.png"));
		texture_dialogue_box_right.~Texture();
		texture_dialogue_box_right.load_from_file(textures_path("cutscenes/intel.png"));

		texture_background.~Texture();
		texture_background.load_from_file(textures_path("cutscenes/bg_level2_0.png"));
	}
	else if (cutscene_state == LEVEL_3_CUTSCENE && counter_value == 81)
	{
		texture_dialogue_box_mid.~Texture();
		texture_dialogue_box_mid.load_from_file(textures_path("cutscenes/level3/81.png"));
		texture_dialogue_box_left.~Texture();
		texture_dialogue_box_left.load_from_file(textures_path("cutscenes/pierre.png"));
		texture_dialogue_box_right.~Texture();
		texture_dialogue_box_right.load_from_file(textures_path("cutscenes/intel.png"));

		//skip_texture.~Texture();
		//skip_texture.load_from_file(textures_path("skip.png"));
		texture_background.~Texture();
		texture_background.load_from_file(textures_path("cutscenes/bg_level3_0.png"));
	}
}