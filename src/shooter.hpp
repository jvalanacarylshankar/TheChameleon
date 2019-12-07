#pragma once

// internal
#include "common.hpp"
#include "char.hpp"
#include "bullets.hpp"

// guard type 2 : spotter
class Shooter : public Entity
{
	// shared texture
	static Texture shooter_texture;

private:
	// config
	const float config_scale = 0.075f;

	// ai
	float m_radius = 50.f;
	bool m_in_combat;

	bool m_alert_mode;

public:
	bool init();
	void destroy();
	void update(float ms);
	void draw(const mat3& projection) override;

	// movement
	void set_position(vec2 pos);
	vec2 get_position() const;
	
	void set_rotation(float rad);

	// alert
	void set_alert_mode(bool val);

	// combat
	void set_in_combat(bool val);
	bool is_in_combat() const;

	// collision
	vec2 get_bounding_box() const;

	Bullets bullets;
};