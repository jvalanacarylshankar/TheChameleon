#pragma once

#include <vector>

// internal
#include "common.hpp"

using namespace std;

class Bullets : public Entity
{
private:
	GLuint m_instance_vbo;
	//std::vector<Bullet> m_bullets; // vector of pebbles

public:
	struct Bullet
	{
		float life;
		vec2 position;
		float radius;
		vec2 velocity;
	};

	bool init();
	void destroy();
	void update(float ms);
	void draw(const mat3 &projection) override;

	// spawn bullet
	void spawn_bullet(vec2 position, float radians);

	float cooldown;

	std::vector<Bullet> m_bullets;
};