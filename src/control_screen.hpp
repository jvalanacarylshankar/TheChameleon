#pragma once

// internal
#include "common.hpp"
#include "constants.hpp"

// control screen
class ControlScreen : public Entity
{
	// shared texture
	static Texture control_screen;

public:
	bool init();
	void destroy();
	void draw(const mat3 &projection) override;
};