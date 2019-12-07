#pragma once

// internal
#include "common.hpp"
#include "constants.hpp"

// level screen
class LevelScreen : public Entity
{
	// shared texture
	static Texture pointer;
	static Texture tutorial;
	static Texture level_1;
	static Texture level_2;
	static Texture level_3;
	static Texture level_4;
	static Texture level_5;

private:
	vec2 m_pointer_pos;

public:
	bool init();
	void destroy();
	void update(unsigned int game_state);

	void draw(const mat3& projection) override;
	void draw_element(const mat3& projection, const Texture& texture, vec2 pos, vec2 scale);
};