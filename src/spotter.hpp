#pragma once

// internal
#include "common.hpp"
#include "map.hpp"
#include "char.hpp"

class Map;
class Char;

// guard type 2 : spotter
class Spotter : public Entity
{
	// shared texture
	static Texture spotter_texture;

private:
	// config
	const float config_scale = 0.25;

	// animation
	int spotter_sprite_switch = 1;
	float spotter_sprite_countdown = 1500.f;
	const float spriteWidth = 68.f;
	const float spriteHeight = 67.f;
	int frameIndex_x = 1;
	int frameIndex_y = 1;

	// detection
	float radius = 70.f;

	// alert
	bool m_alert_mode;

public:
	bool init();
	void destroy();
	void update(float ms);
	void draw(const mat3& projection) override;

	// movement
	void set_position(vec2 pos);
	vec2 get_position() const;

	// collision
	vec2 get_bounding_box() const;

	// detection
	bool is_in_sight(Char &m_char, Map& m);

	// alert
	void set_alert_mode(bool val);

	vec2 direction;
	
	void reset_direction();

private:
	float check_sgn(float value);
};