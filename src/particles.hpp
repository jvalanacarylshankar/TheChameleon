#pragma once

// internal
#include "common.hpp"

// stdlib
#include <vector>

class Particles : public Entity
{
public:
	struct Particle
	{
		vec2 position;
		vec2 velocity;
		float radius;
	};

	bool init();
	void destroy();
	void update(float ms);
	void draw(const mat3 &projection) override;

	void spawn_particle(vec2 position, int direction);

	// fade particles
	void set_fade(int val);
	void reset_fade_time();
	int get_fade();
	float get_fade_time() const;

private:
	GLuint m_instance_vbo;			   // vbo for instancing particles
	std::vector<Particle> m_particles; // vector of particles
	float m_fade_time;
	int m_fade_particle;
};