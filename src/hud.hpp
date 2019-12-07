#pragma once

// internal
#include "common.hpp"
#include "constants.hpp"

// hud
class Hud : public Entity
{
	// shared texture
	static Texture hud;
	static Texture tooltip;

	vec2 red_tooltip_position;
	vec2 blue_tooltip_position;
  vec2 yellow_tooltip_position;
  vec2 green_tooltip_position;

	bool show_red_tooltip;
  bool show_blue_tooltip;
  bool show_yellow_tooltip;
  bool show_green_tooltip;

  vec2 tooltip_scale;

public:
	bool init();
	void destroy();
	void update(unsigned int game_state, vec2 char_position);
	void draw(const mat3& projection) override;

  void set_tooltip(char color, bool value);
};