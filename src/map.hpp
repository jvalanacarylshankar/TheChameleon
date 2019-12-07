#pragma once

// internal
#include "common.hpp"
#include "constants.hpp"
#include "spotter.hpp"

#include <vector>

#include "char.hpp"
class Char;
class Spotter;

class Map : public Entity
{
	// shared texture
	static Texture wall_texture;
	static Texture wall_light_texture;

	static Texture corridor_texture;
	static Texture corridor_texture_red;
	static Texture corridor_texture_blue;
	static Texture corridor_texture_green;
	static Texture corridor_texture_yellow;

	// Level Museum Texture
	static Texture museum_bottom_left_corner_texture;
	static Texture museum_bottom_right_corner_texture;
	static Texture museum_top_left_corner_texture;
	static Texture museum_top_right_corner_texture;
	static Texture museum_top_wall_texture;
	static Texture museum_bottom_wall_texture;
	static Texture museum_left_wall_texture;
	static Texture museum_right_wall_texture;
	static Texture museum_two_walls_texture;
	static Texture museum_wall_texture;
	static Texture museum_top_u_texture;
	static Texture museum_shadow_texture;
	static Texture museum_corridor_tile_texture;
	static Texture museum_corridor_tile_red_texture;
	static Texture museum_corridor_tile_green_texture;
	static Texture museum_corridor_tile_blue_texture;
	static Texture museum_corridor_tile_yellow_texture;

	// Level Ruins Textures	
	static Texture ruins_bottom_left_corner_texture;
	static Texture ruins_bottom_right_corner_texture;
	static Texture ruins_top_left_corner_texture;
	static Texture ruins_top_right_corner_texture;
	static Texture ruins_top_wall_texture;
	static Texture ruins_bottom_wall_texture;
	static Texture ruins_left_wall_texture;
	static Texture ruins_right_wall_texture;
	static Texture ruins_two_walls_texture;
	static Texture ruins_wall_texture;
	static Texture ruins_top_u_texture;
	static Texture ruins_end_cap_texture;
	static Texture ruins_shadow_texture;

	static Texture trophy_texture;

private:
	vec2 translation_tile;
	float m_dead_time;
	float m_flash_time;
	int flash_map;
	char current_level[40][61];

	int current_level_indicator;

	//Spotters
	std::vector<Spotter>* m_spotters;

public:
	bool init();
	void destroy();

	// draw tiles
	void draw(const mat3 &projection) override;
	void draw_level_tutorial(const mat3& projection);
	void draw_level_1(const mat3& projection);
	void draw_level_2(const mat3& projection);
	void draw_level_3(const mat3& projection);
	void draw_level_4(const mat3& projection);
	void draw_level_5(const mat3& projection);
	void draw_element(const mat3 &projection, const Texture &texture);

	void set_current_map(int level);
	int get_current_map();
	vec2 get_spawn_pos() const;

	// color detection
	int get_tile_type(vec2 pos);

	// wall collision
	void check_wall(Char &ch, const float ms);
	bool check_wall(vec2 spotter_pos, vec2 char_pos);

	// char dead time getters and setters .. ported over from water
	void set_char_dead();
	void reset_char_dead_time();
	float get_char_dead_time() const;

	// flash
	void set_flash(int value);
	void reset_flash_time();
	int get_flash();
	float get_flash_time() const;

	// Pathing helper functions
	vec2 get_tile_center_coords(vec2 tile_indices);
	vec2 get_grid_coords(vec2 position);
	bool is_wall(vec2 grid_coords);

	bool is_wall_texture(char tile);

	void set_spotter_list(std::vector<Spotter>& spotters);
};