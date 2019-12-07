#pragma once

// internal
#include "common.hpp"
#include "map.hpp"
#include "shooter.hpp"
#include "spotter.hpp"
#include "wanderer.hpp"
#include "bullets.hpp"

// stlib
#include <vector>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>

class Shooter;
class Spotter;
class Wanderer;
class Bullets;
class Map;

class Char : public Entity
{
	// shared texture
	static Texture char_texture;

private:
	// config
	const float config_scale = 0.35f;

	bool m_is_alive;
	
	// key press
	bool m_moving_right;
	bool m_moving_left;
	bool m_moving_up;
	bool m_moving_down;

	// color
	int m_color;

	// TODO -- wall collision
	bool m_wall_up;
	bool m_wall_down;
	bool m_wall_left;
	bool m_wall_right;

	// dash
	bool m_dash;
	int m_direction_change;

	// animation
	int sprite_switch = 1;
	float sprite_countdown = 50.f;
	const float spriteWidth = 34;
	const float spriteHeight = 67;
	int frameIndex_x = 1;
	int frameIndex_y = 1;
	void reinitialize();

	// Stealthing Animations
	bool stealth_animating = false;
	float stealth_anim_time = 0;
	bool stealthed = false;
	Map* m_map;

	// sound
	Mix_Chunk *m_sfx_bump;
	Mix_Chunk *m_sfx_color_change;
	Mix_Chunk *m_sfx_dead;
	Mix_Chunk *m_sfx_walk;
	Mix_Chunk *m_sfx_yellow;

public:
	bool init(vec2 pos, Map &map);
	void destroy();
	void update(float ms);
	void draw(const mat3 &projection) override;

	// alive
	bool is_alive() const;
	void kill();

	// collision
	bool collision(vec2 pos, vec2 box);
	bool is_colliding(Bullets &b);
	bool is_colliding(const Shooter &s);
	bool is_colliding(const Spotter &s);
	bool is_colliding(const Wanderer &w);
	bool is_in_range(Wanderer &w, Map& map);
	bool is_in_alert_mode_range(Wanderer &w);
	vec2 get_bounding_box() const;

	// wall collision
	void set_wall_collision(char dir, bool val);
	bool is_wall_collision();

	// movement
	void set_direction(char dir, bool val);
	void set_position(vec2 pos);
	void change_position(vec2 off);
	vec2 get_position() const;
	float get_speed() const;
	vec2 get_velocity();
	bool is_moving() const;

	// dash
	void change_direction(int c);
	int get_direction() const;

	// color change
	void set_color(int color);
	int get_color() const;

	// dash
	void set_dash(bool val);
	bool is_dashing();

	void set_rotation(float rad);

	//stealth helpers
	void reset_stealth();
	bool is_stealthed();
};