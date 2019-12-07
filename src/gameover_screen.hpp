#pragma once

// internal
#include "common.hpp"
#include "constants.hpp"

// gameover screen
class GameoverScreen : public Entity
{
	// shared texture
	static Texture pointer;
	static Texture game_over;
	static Texture retry;
	static Texture main_menu;
	static Texture piere_gameover;

private:
	vec2 pointer_pos;

public:
	bool init();
	void destroy();
	void update(unsigned int game_state);

	void draw(const mat3& projection) override;
	void draw_element(const mat3& projection, const Texture& texture, vec2 pos, vec2 scale);
};