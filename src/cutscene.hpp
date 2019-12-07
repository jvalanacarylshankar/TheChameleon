#pragma once

// internal
#include "common.hpp"
#include "constants.hpp"

// start screen
class Cutscene : public Entity
{
	// shared texture
	static Texture texture_dialogue_box;
	static Texture texture_dialogue_box_left;
	static Texture texture_dialogue_box_right;
	static Texture texture_dialogue_box_mid;
	static Texture enemy_texture;
	//static Texture skip_texture;
	static Texture texture_background;

  unsigned int dialogue_counter;

	int left_dialogues[43] = {
		1, 3, 6, 7, 10, 14, 15, 17, 19, 20, 26, 27, 30, 31, 34,
		37, 39, 41, 43, 46, 47, 51, 53, 55, 57, 59, 61, 63, 67,
		68, 69, 71, 73, 75, 78, 80, 81, 82, 84, 86, 88, 90, 92
	};

	unsigned int current_cutscene_state;

public:
	bool init();
	void destroy();
	void update();

	void draw(const mat3& proj) override;
	void draw(const mat3& proj, vec2 wsize, const vec2 wpoint);
	void draw_element(const mat3& proj, const Texture& texture, vec2 pos, vec2 scale);

  bool dialogue_done(unsigned int cutscene_state);
  void increment_dialogue_counter(unsigned int cutscene_state);
	void set_dialogue_counter(unsigned int cutscene_state, unsigned int counter_value);
	bool is_left_dialogue();
};