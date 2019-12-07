// header
#include "char.hpp"

// stlib
#include <cmath>
#include <string>
#include <algorithm>

Texture Char::char_texture;

using namespace std;

const int STEALTH_ANIM_DURATION = 1000;

bool Char::init(vec2 spos, Map &map)
{
	m_map = &map;

	// load sound
	if (SDL_Init(SDL_INIT_AUDIO) < 0)
	{
		fprintf(stderr, "Failed to initialize SDL Audio");
		return false;
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
	{
		fprintf(stderr, "Failed to open audio device");
		return false;
	}

	m_sfx_bump = Mix_LoadWAV(audio_path("char_bump.wav"));
	m_sfx_color_change = Mix_LoadWAV(audio_path("char_color_change.wav"));
	m_sfx_dead = Mix_LoadWAV(audio_path("char_dead.wav"));
	m_sfx_walk = Mix_LoadWAV(audio_path("char_walk.wav"));
	m_sfx_yellow = Mix_LoadWAV(audio_path("char_yellow.wav"));
	
	if (m_sfx_bump == nullptr || 
	  m_sfx_color_change == nullptr || 
	  m_sfx_dead == nullptr || 
		m_sfx_walk == nullptr || 
		m_sfx_yellow == nullptr)
	{
		fprintf(stderr, "Failed to load char sfx\n");
		return false;
	}

	// load shared texture
	if (!char_texture.is_valid())
	{
		if (!char_texture.load_from_file(textures_path("base_undead-1.png.png")))
		{
			fprintf(stderr, "Failed to load char texture!\n");
			return false;
		}
	}

	// the position corresponds to the center of the texture
	// sprite sheet calculations
	const float tw = spriteWidth / char_texture.width;
	const float th = spriteHeight / char_texture.height;
	const int numPerRow = char_texture.width / spriteWidth;
	const int numPerCol = char_texture.height / spriteHeight;
	const float tx = (frameIndex_x % numPerRow - 1) * tw;
	const float ty = (frameIndex_y / numPerCol) * th;

	float posX = 0.f;
	float posY = 0.f;


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
	if (!effect.load_from_file(shader_path("char.vs.glsl"), shader_path("char.fs.glsl")))
		return false;

	motion.position = spos;
	motion.radians = 0.f;
	motion.speed = 65.f;

	physics.scale = {-config_scale, config_scale};

	// initial values
	m_is_alive = true;
	m_color = 0;
	m_direction_change = 0;

	m_moving_right = false;
	m_moving_left = false;
	m_moving_up = false;
	m_moving_down = false;

	m_wall_up = false;
	m_wall_down = false;
	m_wall_left = false;
	m_wall_right = false;

	m_dash = false;


	return true;
}

// release all graphics resources
void Char::destroy()
{
	if (m_sfx_bump != nullptr)
		Mix_FreeChunk(m_sfx_bump);
	if (m_sfx_color_change != nullptr)
		Mix_FreeChunk(m_sfx_color_change);
	if (m_sfx_dead != nullptr)
		Mix_FreeChunk(m_sfx_dead);
	if (m_sfx_walk != nullptr)
		Mix_FreeChunk(m_sfx_walk);
	if (m_sfx_yellow != nullptr)
		Mix_FreeChunk(m_sfx_yellow);

	Mix_CloseAudio();

	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteVertexArrays(1, &mesh.vao);

	effect.release();
}

// update
void Char::update(float ms)
{
	float step = motion.speed * (ms / 1000);

	if (m_is_alive)
	{
		// speed up on dash
		if (m_dash)
			step *= 3;

		// go in random direction on dash
		if (m_dash && !m_moving_up && !m_moving_down && !m_moving_left && !m_moving_right)
		{
			int r = rand() % 4;
			// chose direction
			if (r == 0)
			{
				m_moving_up = true;
				m_direction_change = 0;
			}
			else if (r == 1)
			{
				m_moving_down = true;
				m_direction_change = 1;
			}
			else if (r == 2)
			{
				m_moving_left = true;
				m_direction_change = 2;
			}
			else if (r == 3)
			{
				m_moving_right = true;
				m_direction_change = 3;
			}
		}

		// opposite direction if blue
		// TODO
		if (m_color == 3)
		{
			if (m_moving_up && !m_wall_down)
				change_position({0.f, step});
			if (m_moving_down && !m_wall_up)
				change_position({0.f, -step});
			if (m_moving_left && !m_wall_right)
				change_position({step, 0.f});
			if (m_moving_right && !m_wall_left)
				change_position({-step, 0.f});
		}
		else
		{
			if (m_moving_up && !m_wall_up)
				change_position({0.f, -step});
			if (m_moving_down && !m_wall_down)
				change_position({0.f, step});
			if (m_moving_left && !m_wall_left)
				change_position({-step, 0.f});
			if (m_moving_right && !m_wall_right)
				change_position({step, 0.f});
		}

		if (m_moving_right)
		{
			if (physics.scale.x < 0) {
				physics.scale.x = -physics.scale.x;
			}
		}
		if (m_moving_left)
		{
			if (physics.scale.x > 0) {
				physics.scale.x = -physics.scale.x;
			}
		}

		// sprite change
		if (m_moving_down || m_moving_up || m_moving_right || m_moving_left)
		{
			if (frameIndex_x < 7)
			{
				frameIndex_x++;
			}
			else
			{
				frameIndex_x = 2;
			}
			reinitialize();
		}
		else
		{
			frameIndex_x = 1;
			frameIndex_y = 1;
			reinitialize();
		}
	}

	if (!is_moving() && !stealth_animating && !stealthed)
	{
		if (m_map->get_tile_type(get_position()) == get_color() + 1)
		{
			stealth_animating = true;
		}
	}
	else if (is_moving() && (stealthed || stealth_animating))
	{
		reset_stealth();
	}

	if (stealth_animating)
	{
		if (stealth_anim_time >= STEALTH_ANIM_DURATION)
		{
			stealthed = true;
			stealth_animating = false;
			stealth_anim_time = 0;
		}
		else
		{
			stealth_anim_time += ms;
		}
	}
}

void Char::draw(const mat3 &projection)
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

	// get uniform locations
	GLint transform_uloc = glGetUniformLocation(effect.program, "transform");
	GLint color_uloc = glGetUniformLocation(effect.program, "fcolor");
	GLint projection_uloc = glGetUniformLocation(effect.program, "projection");
	GLint color_change_uloc = glGetUniformLocation(effect.program, "color_change");
	GLint is_alive_uloc = glGetUniformLocation(effect.program, "is_alive");	
	GLint stealthed_uloc = glGetUniformLocation(effect.program, "stealthed");
	GLint stealthing_uloc = glGetUniformLocation(effect.program, "stealthing");
	GLint stealth_anim_time_uloc = glGetUniformLocation(effect.program, "stealthing_anim_time");

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
	if (char_texture.id != 0)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, char_texture.id);
	}

	// set uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float *)&transform.out);

	
	// color
	float color[] = {1.f, 1.f, 1.f};
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float *)&projection);

	float color_change = get_color();
	glUniform1f(color_change_uloc, color_change);

	glUniform1f(is_alive_uloc, is_alive());

	//Stealth stuff
	glUniform1f(stealthed_uloc, stealthed);
	glUniform1f(stealthing_uloc, stealth_animating);
	glUniform1f(stealth_anim_time_uloc, stealth_anim_time);

	// draw
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

////////////////////
// ALIVE
////////////////////

bool Char::is_alive() const
{
	return m_is_alive;
}

void Char::kill()
{
	if (m_is_alive)
		Mix_PlayChannel(1, m_sfx_dead, 0);
	m_is_alive = false;
}

////////////////////
// COLLISION
////////////////////

// aabb-aabb collision
bool Char::collision(vec2 pos, vec2 box)
{
	vec2 char_pos = motion.position;
	vec2 char_box = get_bounding_box();
	bool collision_x_right = (char_pos.x + char_box.x) >= (pos.x - box.x) && (char_pos.x + char_box.x) <= (pos.x + box.x);
	bool collision_x_left = (char_pos.x - char_box.x) >= (pos.x - box.x) && (char_pos.x - char_box.x) <= (pos.x + box.x);
	bool collision_y_top = (char_pos.y + char_box.y) >= (pos.y - box.y) && (char_pos.y + char_box.y) <= (pos.y + box.y);
	bool collision_y_down = (char_pos.y - char_box.y) >= (pos.y - box.y) && (char_pos.y - char_box.y) <= (pos.y + box.y);

	if ((char_pos.x + char_box.x) >= (pos.x + box.x) && (char_pos.x - char_box.x) <= (pos.x - box.x))
		return collision_y_top || collision_y_down;

	if ((char_pos.y + char_box.y) >= (pos.y + box.y) && (char_pos.y - char_box.y) <= (pos.y - box.y))
		return collision_x_right || collision_x_left;

	return (collision_x_right || collision_x_left) && (collision_y_top || collision_y_down);
}

// TODO
bool Char::is_colliding(Bullets &b)
{
	vec2 char_pos = motion.position;
	vec2 char_box = get_bounding_box();

	for (auto &bullet : b.m_bullets)
	{
		vec2 pos = bullet.position;
		vec2 box = {bullet.radius, bullet.radius};

		// bullet collision
		if (char_pos.y - char_box.y < pos.y &&
			char_pos.y + char_box.y > pos.y &&
			char_pos.x - char_box.x < pos.x &&
			char_pos.x + char_box.x > pos.x)
		{
			bullet.life = 0.f;
			return true;
		}
	}
	return false;
}

bool Char::is_colliding(const Shooter &s)
{
	vec2 pos = s.get_position();
	vec2 box = s.get_bounding_box();
	return collision(pos, box);
}

bool Char::is_colliding(const Spotter &s)
{
	vec2 pos = s.get_position();
	vec2 box = s.get_bounding_box();
	return collision(pos, box);
}

bool Char::is_colliding(const Wanderer &w)
{
	vec2 pos = w.get_position();
	vec2 box = w.get_bounding_box();
	return collision(pos, box);
}

vec2 Char::get_bounding_box() const
{
	return { std::fabs(physics.scale.x) * char_texture.width * 0.5f * 0.10625f, std::fabs(physics.scale.y) * char_texture.height * 0.5f * 0.07046875f };
}

void Char::set_wall_collision(char direction, bool value)
{
	if (direction == 'R')
		m_wall_right = value;
	else if (direction == 'L')
		m_wall_left = value;
	else if (direction == 'U')
		m_wall_up = value;
	else if (direction == 'D')
		m_wall_down = value;
}

bool Char::is_wall_collision()
{
	return m_wall_down || m_wall_left || m_wall_right || m_wall_up;
}

bool Char::is_in_range(Wanderer &w, Map& m)
{
	vec2 pos = w.get_position();
	vec2 box = w.get_bounding_box();
	float dx = motion.position.x - w.get_position().x;
	float dy = motion.position.y - w.get_position().y;
	float d_sq = dx * dx + dy * dy;
	float other_r = std::max(w.get_bounding_box().x, w.get_bounding_box().y);
	float my_r = std::max(physics.scale.x, physics.scale.y);
	float r = std::max(other_r, my_r);
	r *= 5.f;

	bool is_wall = false;
	if (d_sq < r*r)
		is_wall = m.check_wall(motion.position, w.get_position());

	if ((d_sq < r * r) && !(is_wall))
		return true;
	return false;
}

bool Char::is_in_alert_mode_range(Wanderer &w)
{
	vec2 pos = w.get_position();
	vec2 box = w.get_bounding_box();
	float dx = motion.position.x - w.get_position().x;
	float dy = motion.position.y - w.get_position().y;
	float d_sq = dx * dx + dy * dy;
	float other_r = std::max(w.get_bounding_box().x, w.get_bounding_box().y);
	float my_r = std::max(physics.scale.x, physics.scale.y);
	float r = std::max(other_r, my_r);
	r *= 12.f;

	if (d_sq < r * r)
		return true;
	return false;
}

////////////////////
// MOVEMENT
////////////////////

void Char::set_direction(char direction, bool value)
{
	// prevent direction change upon dash consequence
	if (m_dash)
	{
		if (value)
			return;
	}

	else if (direction == 'U')
		m_moving_up = value;
	else if (direction == 'D')
		m_moving_down = value;
	else if (direction == 'R')
		m_moving_right = value;
	else if (direction == 'L')
		m_moving_left = value;
}

void Char::set_position(vec2 pos)
{
	motion.position = pos;
}

void Char::change_position(vec2 offset)
{
	motion.position.x += offset.x;
	motion.position.y += offset.y;
}

vec2 Char::get_position() const
{
	return motion.position;
}

float Char::get_speed() const
{
	return motion.speed;
}

vec2 Char::get_velocity()
{
	vec2 res = {0.f, 0.f};
	m_moving_up ? res.y = -1.f : res.y;
	m_moving_down ? res.y = 1.f : res.y;
	m_moving_left ? res.x = -1.f : res.x;
	m_moving_right ? res.x = 1.f : res.x;

	return res;
}

bool Char::is_moving() const
{
	return m_moving_up || m_moving_down || m_moving_left || m_moving_right;
}

// up: 0, down: 1, left: 2, right: 3
void Char::change_direction(int dir)
{
	m_direction_change = dir;
}

int Char::get_direction() const
{
	return m_direction_change;
}

////////////////////
// COLOR CHANGE
////////////////////

// 1: red, 2: green; 3: blue; 4: yellow;
void Char::set_color(int color)
{
	if (m_color != color)
	{
		Mix_PlayChannel(-1, m_sfx_color_change, 0);
		m_color = color;
		if (stealthed || stealth_animating)
		{
			reset_stealth();
		}
	}
	if (color == 4)
		Mix_PlayChannel(-1, m_sfx_yellow, 0);
}

int Char::get_color() const
{
	return m_color;
}

////////////////////
// CONSEQUENCE
////////////////////

void Char::set_dash(bool value)
{
	if (m_dash && !value)
		Mix_PlayChannel(-1, m_sfx_bump, 0);
	m_dash = value;

	if (!value)
		m_dash = m_moving_up = m_moving_down = m_moving_left = m_moving_right = value;
}

bool Char::is_dashing()
{
	return m_dash;
}

////////////////////
// MISC
////////////////////

void Char::set_rotation(float radians)
{
	motion.radians = radians;
}

void Char::reinitialize()
{
	// the position corresponds to the center of the texture
	// sprite sheet calculations
	const float tw = spriteWidth / char_texture.width;
	const float th = spriteHeight / char_texture.height;
	const int numPerRow = char_texture.width / spriteWidth;
	const int numPerCol = char_texture.height / spriteHeight;
	const float tx = (frameIndex_x % numPerRow - 1) * tw;
	const float ty = (frameIndex_y / numPerCol) * th;

	float posX = -15.5f;
	float posY = -40.f;

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
}

void Char::reset_stealth()
{
	stealthed = false;
	stealth_animating = false;
	stealth_anim_time = 0;
}

bool Char::is_stealthed()
{
	return stealthed || stealth_animating;
}

