// header
#include "map.hpp"

// stlib
#include <cmath>
#include <iostream>

// Level Museum, Labyrinth Textures
Texture Map::wall_texture;
Texture Map::corridor_texture;
Texture Map::wall_light_texture;

/////////////////////////
// Level: Museum Textures
/////////////////////////
Texture Map::museum_bottom_left_corner_texture;
Texture Map::museum_bottom_right_corner_texture;
Texture Map::museum_top_left_corner_texture;
Texture Map::museum_top_right_corner_texture;
Texture Map::museum_top_wall_texture;
Texture Map::museum_bottom_wall_texture;
Texture Map::museum_left_wall_texture;
Texture Map::museum_right_wall_texture;
Texture Map::museum_two_walls_texture;
Texture Map::museum_wall_texture;
Texture Map::museum_top_u_texture;
Texture Map::museum_shadow_texture;
Texture Map::museum_corridor_tile_texture;
Texture Map::museum_corridor_tile_red_texture;
Texture Map::museum_corridor_tile_green_texture;
Texture Map::museum_corridor_tile_blue_texture;
Texture Map::museum_corridor_tile_yellow_texture;

/////////////////////////
// Level: Ruins Textures
/////////////////////////
Texture Map::ruins_bottom_left_corner_texture;
Texture Map::ruins_bottom_right_corner_texture;
Texture Map::ruins_top_left_corner_texture;
Texture Map::ruins_top_right_corner_texture;
Texture Map::ruins_top_wall_texture;
Texture Map::ruins_bottom_wall_texture;
Texture Map::ruins_left_wall_texture;
Texture Map::ruins_right_wall_texture;
Texture Map::ruins_two_walls_texture;
Texture Map::ruins_wall_texture;
Texture Map::ruins_top_u_texture;
Texture Map::ruins_end_cap_texture;
Texture Map::ruins_shadow_texture;

// Corridor Textures
Texture Map::corridor_texture_red;
Texture Map::corridor_texture_blue;
Texture Map::corridor_texture_green;
Texture Map::corridor_texture_yellow;

Texture Map::trophy_texture;

static constexpr float TILE_SIZE = 20.f;

// 800 * 1200
// 61 for the \n of all chars
char level_tutorial[40][61] = {
	"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW",
	"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW",
	"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW",
	"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW",
	"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW",
	"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW",
	"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW",
	"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW",
	"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW",
	"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW",
	"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW",
	"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW",
	"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW",
	"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW",
	"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW",
	"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW",
	"WSSSSSSSSSSSSWWWWWWWWWWWWSSSSSWSSSWSSSSSSWSSSSSSSSSSSSSSSSSW",
	"WCCCCCRRRRRRRSSSSSSWWWWWWCCCCCWBBBWBBCGGGWCCYYYYYYYYYYYYCCCW",
	"WCCCCCRRRRRRRRRRRRRSSSSSSCCCCCWBWBWBWCGGGSCCYYYYYYYYYYYYCCCW",
	"WCCACCRRRRRRRRRRRRRRRRRRRCCCCCWBWBWBWCGGGCCCYYYYYYYYYYYYCCCZ",
	"WCCCCCRRRRRRRRRRRRRWWWWWWCCCCCSBWBSBWCGGGWCCYYYYYYYYYYYYCCCW",
	"WCCCCCRRRRRRRWWWWWWWWWWWWCCCCCBBWBBBWCGGGWCCYYYYYYYYYYYYCCCW",
	"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW",
	"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW",
	"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW",
	"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW",
	"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW",
	"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW",
	"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW",
	"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW",
	"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW",
	"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW",
	"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW",
	"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW",
	"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW",
	"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW",
	"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW",
	"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW",
	"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW",
	"WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW"};

char map_level_1[40][61] = {
	"SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS",
	"SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS",
	"SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS",
	"SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS",
	"SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS",
	"SS7WWWWWWW8SSS7WWWWWWW8SSS7WWWWWWW8SSSSSSSSSSSSSSSSSSSSSSSSS",
	"SS7BBBRBBB8SSS7YYYRYYY8SSS7BBBRBBB8SSSSSSSSSSSSSSSSSSSSSSSSS",
	"SS7BBBRBBB8SSS7YYYRYYY8SSS7BBBRBBB8SSSSSSSSSSSSSSSSSSSSSSSSS",
	"SS7BBBRBBBWWWWWYYYRYYYWWWWWBBBRBBB8SSSSSSSSSSSSSSSSSSSSSSSSS",
	"SS7RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR8SSSSSSSSSSSSSSSSSSSSSSSSS",
	"SS7BBBRBBB35554YYYRYYY35554BBBRBBB8SSSSSSSSSSSSSSSSSSSSSSSSS",
	"SS7BBBRBBB8SSS7YYYRYYY8SSS7BBBRBBB8SSSSSSSSSSSSSSSSSSSSSSSSS",
	"SS7BBBRBBB8SSS7YYYRYYY8SSS7BBBRBBB8SSSSSSSSSSSSSSSSSSSSSSSSS",
	"SSS554R355SSSSS54YRY35SSSSS554R355SSSSSSSSSSSSSSSSSSSSSSSSSS",
	"SSSSS7R8SSSSSSSS7YRY8SSSSSSSS7R8SSSSSSSSSSSSSSSSSSSSSSSSSSSS",
	"SSSSS7R8SSSSSSSS7YRY8SSSSSSSS7R8SSSSSSSSSSSSSSSSSSSSSSSSSSSS",
	"SSSSS7R8SSSSSSSS7YRY8SSSSSSSS7R8SSSSSSSSSSSSSSSSSSSSSSSSSSSS",
	"SS7WWWRWWW8SSS7WWYRYWW8SSS7WWWRWWW8SSSSSSSSSSSSSSSSSSSSSSSSS",
	"SS7GGGRYYY8SSS7BBYRYYY8SSS7YYYRYYY8SSSSSSSSSSSSSSSSSSSSSSSSS",
	"SS7GGGRYYYWWWWWBBYRYYYWWWWWYYYRYYY8SSSSSSSSSSSSSSSSSSSSSSSSS",
	"SS7GGGRYYYBBBBBBBCCCBBBBBBBYYYRYYY8SSSSSSSSSSSSSSSSSSSSSSSSS",
	"SS7RRRRRRRRRRRRRRCZCRRRRRRRRRRRRRR8SSSSSSSSSSSSSSSSSSSSSSSSS",
	"SS7BBBRGGGBBBBBBBCCCBBBBBBBYYYRYYY8SSSSSSSSSSSSSSSSSSSSSSSSS",
	"SS7BBBRGGG35554GGGRGBB35554YYYRYYY8SSSSSSSSSSSSSSSSSSSSSSSSS",
	"SS7BBBRGGG8SSS7GGGRGBB8SSS7YYYRYYY8SSSSSSSSSSSSSSSSSSSSSSSSS",
	"SSS554R355SSSSS54GRG35SSSSS554R355SSSSSSSSSSSSSSSSSSSSSSSSSS",
	"SSSSS7R8SSSSSSSS7GRG8SSSSSSSS7R8SSSSSSSSSSSSSSSSSSSSSSSSSSSS",
	"SSSSS7R8SSSSSSSS7GRG8SSSSSSSS7R8SSSSSSSSSSSSSSSSSSSSSSSSSSSS",
	"SSSSS7R8SSSSSSSS7GRG8SSSSSSSS7R8SSSSSSSSSSSSSSSSSSSSSSSSSSSS",
	"SS7WWWRWWW8SSS7WWGRGWW8SSS7WWWRWWW8SSSSSSSSSSSSSSSSSSSSSSSSS",
	"SS7CCCRCCC8SSS7YYYRYYY8SSS7YYYRGGG8SSSSSSSSSSSSSSSSSSSSSSSSS",
	"SS7CCCRCCC8SSS7YYYRYYY8SSS7YYYRGGG8SSSSSSSSSSSSSSSSSSSSSSSSS",
	"SS7CCCRCCCWWWWWYYYRYYYWWWWWYYYRGGG8SSSSSSSSSSSSSSSSSSSSSSSSS",
	"SS7RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR8SSSSSSSSSSSSSSSSSSSSSSSSS",
	"SS7CCCRCCC35554YYYRYYY35554GGGRYYY8SSSSSSSSSSSSSSSSSSSSSSSSS",
	"SS7CACRCCC8SSS7YYYRYYY8SSS7GGGRYYY8SSSSSSSSSSSSSSSSSSSSSSSSS",
	"SS7CCCRCCC8SSS7YYYRYYY8SSS7GGGRYYY8SSSSSSSSSSSSSSSSSSSSSSSSS",
	"SSS5555555SSSSS5555555SSSSS5555555SSSSSSSSSSSSSSSSSSSSSSSSSS",
	"SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS",
	"SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS"};

char map_level_2[40][61] = {
	"SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS",
	"SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS",
	"SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS",
	"SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS",
	"SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS",
	"SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS",
	"SSSSSSSSS7WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW8SSSSSSSSS",
	"SSSSSSSSS7RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR8SSSSSSSSS",
	"SSSSSSSSS7RBBBBBBBBBBBBRRRBBBBBRBBBBBB35554YYYYYYR8SSSSSSSSS",
	"SSSSSSSSS7RBBBBBBBBB354RRRBBBBBRBBBBBB8SSS7YYYZYYR8SSSSSSSSS",
	"SSSSSSSSS7RBB34BBBBB8S7RRRBBBBBRB354BBWWWWWYY34YYR8SSSSSSSSS",
	"SSSSSSSSS7RB3SS4BBBBWWWRRRBBBBBRB8S7RRRRRRRR3SS4YR8SSSSSSSSS",
	"SSSSSSSSS7RBW87WRRRRRRRRRR354BBRB8S7RRRRRRRRW87WYR8SSSSSSSSS",
	"SSSSSSSSS7RYYWWRRRRRRRRRRR8S7BBRB8S7GGGR34YYYWWYYR8SSSSSSSSS",
	"SSSSSSSSS7RYYYYYYYYRRRYYYYWWWBBRB8S7GGGR87RRRRRRRR8SSSSSSSSS",
	"SSSSSSSSS7R3554YYYYRRRYYYYGGGGGRBWWWGGGR87RRRRRRRR8SSSSSSSSS",
	"SSSSSSSSS7R8SS7Y354RRR354GGGGGGRGGGGGGGRWWBBBBBBBR8SSSSSSSSS",
	"SSSSSSSSS7RWWWWY8S7RRR8S7RRRRRRRRRRRRRRRRRR3554BBR8SSSSSSSSS",
	"SSSSSSSSS7RYYYYY8S7RRR8S7RRRRRRR3555554RYYY8SS7BBR8SSSSSSSSS",
	"SSSSSSSSS7RYYYYY8S7RRR8S7YYYYYYRWWWWWWWRYYYWWWWBBR8SSSSSSSSS",
	"SSSSSSSSS7RRRRRR8S7RRR8S7YYY34YRRRRRRRRRRRRRRRRRRR8SSSSSSSSS",
	"SSSSSSSSS7RRRRRR8S7RRR8S7YYY87YRRRRRRRRRRRRRR3554R8SSSSSSSSS",
	"SSSSSSSSS7R354GG8S7RRR8S7YYY87YR355554GRYYYRB8SS7R8SSSSSSSSS",
	"SSSSSSSSS7RWWWGGWWWRRRWWWYYY87YR8SSSS7GR34YRBWWWWR8SSSSSSSSS",
	"SSSSSSSSS7RBBBBBBBBBBBBBBYYY87YRWWWWWWGR87YRBGGGBR8SSSSSSSSS",
	"SSSSSSSSS7RBBBBBBBBBBBBBB355S7YRBBBBBBGRWWYRBGGGBR8SSSSSSSSS",
	"SSSSSSSSS7RBB34BBBBBBBBBBWWWWWYRBBBBBBBRYYYRB34GBR8SSSSSSSSS",
	"SSSSSSSSS7RB3SS4RRRRRRRRRRRRRRRRRRRRRRRRRRRR3SS4BR8SSSSSSSSS",
	"SSSSSSSSS7RBW87WGGG354YYYYYYYYYRYY354YY3554RW87WBR8SSSSSSSSS",
	"SSSSSSSSS7RYYWWGGGG8S7YYYY35554RYY8S7YY8SS7RBWWBBR8SSSSSSSSS",
	"SSSSSSSSS7RYYGGGGGGWWWYYYY8SSS7RYYWWWYY8SS7RGGBBBR8SSSSSSSSS",
	"SSSSSSSSS7RYYAYYYYYYYYYYYYWWWWWRYYYYYYYWWWWRGGBBBR8SSSSSSSSS",
	"SSSSSSSSS7RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR8SSSSSSSSS",
	"SSSSSSSSSS5555555555555555555555555555555555555555SSSSSSSSSS",
	"SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS",
	"SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS",
	"SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS",
	"SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS",
	"SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS",
	"SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS"};

// The Museum
char map_level_3[40][61] = {
	"SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS",
	"7WWWWWW0WWWWWWWWW0WWW0WWWWWWWWWWWWWWWWWWWWWWWWWWWW0WWWWWWWW8",
	"7BBBBBB0YYYYYYYYY0RRR0BBBBBBBBBBBBBBBBBBBBBBBBBBBB0CCCCCCCC8",
	"7BBBBBBWYYYYYYYYYWRRRWBBBBBBBBBBBBBBBBBBBBBBBBBBBB0CCCCCCCC8",
	"7BBBBBBBYYYYYYYYYRRRRRRR3555555554B3555554BB3554BBWUCCCCZCC8",
	"7BBBBBBBYYYYYYYYYRRRRRRRWWWWWWW0WWBWW0WWWWBBWWW0BBB0CCCCCCC8",
	"7BBBBBBUYYYYYYYYYURRRRRRRRRRRRR0GGGGG0CCCCCCCCCWBBBWUCCCCCC8",
	"S54BB35S5555554YY0RRR35555554RR0GGGGG0CCCCCCCCCCCCCCWCCCCCC8",
	"7WWBBWWWWWWWWWWYY0RRR0WWWWWWWRR0GGGGG0CCCCCCCCCCCCCCCCCCCCC8",
	"7RRRRRRRYYYYYYBBB0RRR0GGGGGGGGG0GGGGG0CCCCCCCCCCCCCCCCUCCCC8",
	"7RRRRRRUYYYYYYUBB0RRR0GG35555557GGGGG0CCCCCCCCCCCCCCCCW34CC8",
	"7RRRRRR0YYYYYY0BB0RRRWGGWWWWWWWWGGGGGWCCCCCCCCCCCCCCCCCWW35S",
	"7RR3555S555555S557RRRBBBBBBBBBBBBBBBBBCCCCCCCCCCCCCCCCCCCWW8",
	"7RRWWWWWWWWWWWWWW0RRRUBB35555555554BBUCCCCCCCCCCCCCCCCCCCCC8",
	"7GGGGGGGGGGGGGGGG0RRR0BBWWW87WWWWWWBB0CCCCCCCCCCCCCCCCCCCCC8",
	"S5555555554GGUYYY0RRR0RRRRRW0YYYYYYYYWUCCCCCCCCCCCCCCCCCCCC8",
	"7WWWWWWWWWWGG0YYY0RR3S554RRRWUYYYYYYYY84GGG355555555554RRR3S",
	"7BBBBBBBBBBBB0YYYWRRWWWWWURRRWUYYYYYYY0WGGUWWWWWWWWWW0WRRRW8",
	"7BBBBBBBBBBBB0YYYYYYBBBBBWURRRWUYYYYYUWGGUWRRGGGGGGGG0RRRRR8",
	"S55554BB355557YYYYYYBBBBBBWURRRWYYYYUWGGUWRRUGGGGGGGG0RRRRR8",
	"7WWWWWBBWWWWW0YYY35555554BBWURRRYYYUWGG37RR37GG3554GG854R35S",
	"7CCCCCCCCCCCC0YYY0WWWWWWWUBBWURRYYUWGGUWWRRWWGGWWWWGGWWWRWW8",
	"7CCCCCCCCCCCC0YYY0YYYYYYYWUBBW3554WGGUWRRRRRRGGGGGGGGBBBBBB8",
	"7CCCCCCCCCCCC0YYY0YYYYYYYYWUBBWWWWGGUWRRR3554GG3554GG355555S",
	"7CCCCCCCCCCCUWYYYWUYYYYYYYYWUBBBGGGUWRRRUWWWWGGWWWWGGWWWWWW8",
	"7CCCCCCCCCCUWCCCCCWUYYYYYYYY0BBBGGG0RRRUWYYYYYYYYYYYYYYYYYY8",
	"7CCCCCCCCCUWCCCCCCCWUYYYYYYYW34BG34WRRUWYYYYYYYYYYYYYYYYYYY8",
	"7CCCCCCCCC0CCCCCCCCCWYYYYYYYYWWBGWWRRR0YYYYYYYYYYYYYYYYYYYY8",
	"7CCCCCCCCUWCCCCCCCCCCUYYYYYYYYYYRRRRRUWYYYYYYYYYYYYYYYYYYYY8",
	"7CCCCCCCC0CCCCCCCCCCC85554YY3555555557YY3554BB35554GG354RR3S",
	"7CCCCCCCUWCC3555554CCW0WWWYYWWW0WWWWW0YY0WWWBBWWW0WGGW0WRRW8",
	"7CCCCCCC0CCCWWWWWWWCCC0BBBBBBBB0RRRRR0YY0BBBBBBBB0GGGG0RRRR8",
	"7CCCCCCC0CCCCCCCCCCCCCWBBBBBBBBWRRRRRWYYWBBBBBBBBWGGGGWRRRR8",
	"7CCCCCCC0CCCCCCCCCCCCCBBBBBBBBBBRRRRRRYYBBBBBBBBBBGGGGGRRRR8",
	"7CCCCCCC0CCC3555554CCCUBBBBBBBBURRRRRUYYUBBBBBBBBUGGGGURRRR8",
	"7CCCCCCC0CCCWWWWWWWCCC8554BB3557RR3557YY8554BB355S4GG3S4RR3S",
	"7CCCCCCC0CCCCCCCCCCCCCWWWWBBWWWWRRWWWWYYWWWWBBWWWWWGGWWWRRW8",
	"7CCCCCCUWCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC8",
	"7CCACC37CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC8",
	"S55555SS555555555555555555555555555555555555555555555555555S"};

/////////////////////////
// Level: Ruins Textures
/////////////////////////

// bottom left corner - 1
// bottom right corner - 2
// top left corner - 3
// top right corner - 4
// top wall - 5
// bottom wall - 6
// left wall - 7
// right wall - 8
// end cap - E
// shadow - S
// top u - U
// two walls - 0
// walls - W
// red - R
// green - G
// blue - B
// yellow - Y
// corridor - C
char map_level_4[40][61] = {
	"SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS",
	"7WWWWWWWWWW0WWWWWWWWWWW0WWWWWWWWWWWWW0WWWWWWWWW87WWWWWWWWWW8",
	"7YYYYYYYYYY0YYYYYYYYYYY0RRYYYYYYYYYYY0BBBBZBBBB87BBBBBBBBBB8",
	"7GGG3554GGGEYYYYYYYYYYY0RR35555554YYY8555554BBBW84BBBBBBBBB8",
	"7GGGWWWWGGGYYYYYYYYYYYY0R37WWWWWWWYYY0WWWWW84BBBW85555554BB8",
	"7GGGRRRRGGGUYYYYYYYYYYY0R0WYYYYYYYYYY0GGGGGW0BBBGWWWWWWW0BB8",
	"S5554RR35557YYYYYYYYYYY0R0RRRRRRRRRYY0GGGGGG84BBGGGGGGGBEUB8",
	"7WWWWRRWWWW0B35555555557R0RRRRRRRRRYY0GGGGGGW0BBUGGG354BB0B8",
	"7CCCCRRCCCC0BWWWWWWWWWW0R0RRRRRRRRRRR0GYGUGGG0BB854GWWWBB0B8",
	"S5554RR35557BBBBBBBBBBB0R0RRR35554RRR0GY37GGG0BBWW0GGBBBUWB8",
	"7WWWWRRWWWW85555555554B0R84RUWWWWWURR0GY0WGRG0BBRB0RGB357BB8",
	"7BBBBRRYYYY0WWWWWWWWWWB0RW0REBBBBB84R0GY0GGRG0GGRB0RGBWWWBB8",
	"S5554RR35557YYYYYYYYYYY0RR0RBBBBBBW0R0YY0GGRG0GGRBWUGGBBBBB8",
	"7WWWWRRWWWW0G35555555557RR0BBBBBBBBER0YY84GRG84GRRB0RRGG34B8",
	"7YYYYRRYYYY0GWWWWWWWWWW0RR0BBBBBUBBBR0YYW0GRRW0GGRB0RRRRW0B8",
	"S5554RR35557RRRRRRRRRRR0RR0BBB34WUBBR0YYY84RRR0GGRB0RRRRR0B8",
	"7WWWWRRWWWW85555555554R0RREBBBWWY84BR0YYYW0RRR84GRBW34RR37B8",
	"7GGGGRRYYYY0WWWWWWWWWWR0GGGGGGGGYW84REYYYY84RRW0GGRBW8557WB8",
	"S5554RR35557YYYYYYYYYYY0GG3554GGYYW0RRYYYYW0RRB84GGBBWW0WGG8",
	"7WWWWRRWWWW0Y35555555557GG0WW84GGYY0RRRYUBB84RBW84GBBBBEGGG8",
	"7YYYYRRBBBB0YWWWWWWWWWW0GUWBBW0GGYYERRRUWGBW0BBRW84BGGGGGGG8",
	"S5554RR35557BBBBBBBBBBB0G0BBBB84GYYRRRR0GGBG84BRRW0BG34GGGG8",
	"7WWWWRRWWWW85555555554B0GEBBBBWWGYYRRRR0GGGBW0BRRR84GW84GGG8",
	"7GGGGRRYYYY0WWWWWWWWWWB0BBBBBBBGGYYRRRR0GGGBG0BRRRW0GRW84GY8",
	"S5554RR35557YYYYYYYYYYY0BBBBBBBGGYYRRUR0GGGB37BRRRR0GRYW0YY8",
	"7WWWWRRWWWW0Y35555555557RR354BBGGYYRR0R0GGGB0WBRGGG0GRYY84Y8",
	"7BBBBRRBBBB0YWWWWWWWWWW0R37WWBBGGYYYY0R0GGBB0BBRRRG84RYYW0Y8",
	"S5554RR35557RRRRRRRRRRR0R87GGBB354YYY0R0GBBUWBB354GW0RYYY0Y8",
	"7WWWWRRWWWW85555555554R0R87GGB37WWYYY0R0BBG0BB37WWGG0RYYY0Y8",
	"7BBBBRRYYYY0WWWWWWWWWWR0RW84GB0WYYYUB0R0BGG0BBW0GGGG0RYY37Y8",
	"S5554RR35557YYYYYYYYYYY0RRW0GB0YYY37B0G0BGG0BBB0GGGG0RY37WY8",
	"7WWWWRRWWWW0Y35555555557RRR0GB0YY37WB0G0BGG0BBBEGGGG0R37WYY8",
	"7GGBBRRBBBB0YWWWWWWWWWW0RR37G37YY0WBB0G0BGG84BBBGGGG0R0WYYY8",
	"S5554RR35557BBBBBBBBBBB0R37WG0WYY0BBB0GWUBBWW354GGGG0R0YYYY8",
	"7WWWWRRWWWW85555555554B0R87GG0YYY84BB0GG84BBBWWW35557R84YYY8",
	"7YYYYRRGGYY0WWWWWWWWWWBER87GG0YYY87BB0GGW0BBBYYYWWWWWRW0YYY8",
	"S5554RR35557YYYYYYYYYYYYRW0GGEYY3S7BB0GGG854BBYYYYYRRRR854Y8",
	"7WWWWRRWWWW0YYYYYYYYYYYYRREGGGYYWWWBB0GGGWWWGRRRRRYYYYRWWWY8",
	"7ACYYRRBBGG0YYYYYYYYYYYYGGGGGGBBBBBBB0GGGGGGGRRRRRRRRRYYYYY8",
	"S5555555555S5555555555555555555555555S555555555555555555555S"};

// The Maze
char map_level_5[40][61] = {
	"SSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS",
	"7WWWWWWWWWWWW0WWWWW0WWWWWWWWW0WWWWWWWWWWWWWWWWWWWWWWWWWWWWW8",
	"7BBBBBBBBBBBB0CCCCC0BBGGGGGGG0GGGGGGGGGGGGGGBBBBBBBBBBBBBYY8",
	"7BBBBBBBBBBBB0CCCCC0BBGGGGGGG0GGGGGGGGGGGGGGBBBBBBBBBBBBBYY8",
	"7BB35555555557CCCCC0BB35555557GG3555555555555555555555554YY8",
	"7BBWWWWWWWWWWWCCCCC0BB0WWWWWWWGGWWWWWWW0WWWWWWWWWWWWWWWWWYY8",
	"7RRRRRRRRRRRRCCCCCC0BB0CCCCCCCCCCCCCCCC0RRRRRRRRRRRRRRRRRRR8",
	"7RRRRRRRRRRRRCCCCCC0BB0CCCCCCCCCCCCCCCC0RRRRRRRRRRRRRRRRRRR8",
	"7RR3555555554CCCCCCWBB0CCZCCCCCCCCCCCCC0RR355555555555554YY8",
	"7RRWWWWWWWWW0CCCCCCBBB0CCCCCCCCCCCCCCCC0RR0WWWWWWWWWWWWW0YY8",
	"7GGGGGGGGGGG0CCCCCCBBB0CCCCCCCCCCCCCCCC0RR0CCCCCCCCCCCCC0YY8",
	"7GGGGGGGGGGG8555555555S55555555555555550RR0CCCCCCCCCCCCC0YY8",
	"S555555554GG0WWWWWWWWWWWWWWWWWWWWWWWWWW0RRWUCCCCCCCCCCCC0YY8",
	"7WWWWWWWWWGG0BBBBBBBBBBBBBBBBBBBBBBBBBB0RRRW3555554CC355S55S",
	"7BBBBBBBBBGG0BBBBBBBBBBBBBBBBBBBBBBBBBB0RRRRWWWWWWWCCWWWWWW8",
	"7BBBBBBBBBGG0RR355555555555555555555555S54BBBBBBBBBBBBBBBYY8",
	"S555554BBUGG0RR0WWWWWWWWWWWWWWWW0WW0WW0WW0BBBBBBBBBBBBBBBYY8",
	"7WWWWWWBB0GGWRR0YYYYYYYYYYYYYYYY0GG0BB0RR8555555555555554YY8",
	"7CCCCCCCC0RRRRR0YYYYYYYYYYYYYYYY0GG0BB0RR0WWWWWWWWWWWW0WWYY8",
	"7CCCCCCCC0RRRRR0YY355555554YY3557GG0BB0RR0YYYYYYYYYYYY0CCCC8",
	"7CCCCCCCC0RR3557YY0WWWWWWWWYYWWWWGG0BB0RRWYYYYYYYYYYYY0CCCC8",
	"7CCCCCCCC0RR0WW0YY0GGGGGGGGGGGGGGGG0BB0RRRRR35555554YY0CCCC8",
	"7CCCCCCCC0RR0BB0YY0GGGGGGGGGGGGGGGG0BB0RRRRR0WWWWWWWYY0CCCC8",
	"7CCCCCCCC0RR0BB0YY0GG355555555555557BB0RR3557BBBBBBBYY0CCCC8",
	"7RR3555557BBWBBWYYWGGWW0WWWWWWWWWWWWBB0RR0WW0BBBBBBBYY0CCCC8",
	"7RRWWWWWW0BBBBBCCCCCCCC0RRRRRRRRRRRRBB0RR0RR85555555557CCCC8",
	"7RRRRRRRR0BBBBBCCCCCCCC0RRRRRRRRRRRRBB0RR0RRWWWWWW0WWWWCCCC8",
	"7RRRRRRRR855554CCCCCCCC8555555555554BB0RR0RRRRRRRR0GGYYCCCC8",
	"S555554RR0WWWW0CCCCCCCC0WWWWWWWWWWWWBBWRRWRRRRRRRR0GGYYCCCC8",
	"7WWWWW0RR0GGGG0CCCCCCCC0GGGGGGGGGGGGGGGGGGRRRRRRRR0GG355555S",
	"7YYYYY0RR0GGGG8555554BB0GGGGGGGGGGGGGGGGGGRRRRRRRR0GGWWWWWW8",
	"7YYYYY0RR854RR0WWWWW0BB0RR3555555555554BB3555554RR0GGGGGGGG8",
	"7CCURRWRRWWWRR0GGGGG0BBWRR0WWWWWWWWWWW0BB0WWWWW0RR0GGGGGGGG8",
	"7CC0RRRRRRRRRR0GGGGG0BBBRR0BBBBBBBBBBB0BB0YYYYY0RR8555554GG8",
	"7CC0RRRRRRRRRR0GGUBB0BBBRR0BBBBBBBBBBB0BB0YYYYY0RR0WWWWWWBB8",
	"7CC0GG355555557GG0BB8554RR0BB3555555557BB0YYUBB8557BBBBBBBB8",
	"7CC0GGWWWWWWWWWGG0BBWWW0RR0BBWWWWWWWWWWBBWYY0BBWWWWBBBBBBBB8",
	"7CC0GGGGGGGGGGGGG0BBBBB0RR0GGGGGGGGGGGGGGGYY0BBBBBBBBBBBBBB8",
	"7AC0GGGGGGGGGGGGG0BBBBB0RR0GGGGGGGGGGGGGGGYY0BBBBBBBBBBBBBB8",
	"S55S5555555555555S55555S55S55555555555555555S55555555555555S"
};

bool Map::init()
{
	m_dead_time = -1;
	current_level_indicator = LEVEL_TUTORIAL;

	// load shared texture
	if (!wall_texture.is_valid())
	{
		if (!wall_texture.load_from_file(textures_path("wall_tile.png")))
		{
			fprintf(stderr, "Failed to load wall texture!");
			return false;
		}
	}

	if (!wall_light_texture.is_valid())
	{
		if (!wall_light_texture.load_from_file(textures_path("wall_tile_light.png")))
		{
			fprintf(stderr, "Failed to load wall texture!");
			return false;
		}
	}

	if (!corridor_texture.is_valid())
	{
		if (!corridor_texture.load_from_file(textures_path("corridor_tile.png")))
		{
			fprintf(stderr, "Failed to load corridor texture!");
			return false;
		}
	}

	if (!corridor_texture_red.is_valid())
	{
		if (!corridor_texture_red.load_from_file(textures_path("corridor_tile_red.png")))
		{
			fprintf(stderr, "Failed to load corridor texture!");
			return false;
		}
	}

	if (!corridor_texture_blue.is_valid())
	{
		if (!corridor_texture_blue.load_from_file(textures_path("corridor_tile_blue.png")))
		{
			fprintf(stderr, "Failed to load corridor texture!");
			return false;
		}
	}

	if (!corridor_texture_green.is_valid())
	{
		if (!corridor_texture_green.load_from_file(textures_path("corridor_tile_green.png")))
		{
			fprintf(stderr, "Failed to load corridor texture!");
			return false;
		}
	}

	if (!corridor_texture_yellow.is_valid())
	{
		if (!corridor_texture_yellow.load_from_file(textures_path("corridor_tile_yellow.png")))
		{
			fprintf(stderr, "Failed to load corridor texture!");
			return false;
		}
	}

	if (!trophy_texture.is_valid())
	{
		if (!trophy_texture.load_from_file(textures_path("trophy_texture.png")))
		{
			fprintf(stderr, "Failed to load trophy texture!");
			return false;
		}
	}

	/////////////////////////
	// Level: Museum Textures
	/////////////////////////

	if (!museum_bottom_left_corner_texture.is_valid())
	{
		if (!museum_bottom_left_corner_texture.load_from_file(textures_path("museum/bottom_left_corner.png")))
		{
			fprintf(stderr, "Failed to load bottom_left_corner_texture!");
			return false;
		}
	}

	if (!museum_bottom_right_corner_texture.is_valid())
	{
		if (!museum_bottom_right_corner_texture.load_from_file(textures_path("museum/bottom_right_corner.png")))
		{
			fprintf(stderr, "Failed to load bottom_right_corner_texture!");
			return false;
		}
	}

	if (!museum_top_left_corner_texture.is_valid())
	{
		if (!museum_top_left_corner_texture.load_from_file(textures_path("museum/top_left_corner.png")))
		{
			fprintf(stderr, "Failed to load top_left_corner_texture!");
			return false;
		}
	}

	if (!museum_top_right_corner_texture.is_valid())
	{
		if (!museum_top_right_corner_texture.load_from_file(textures_path("museum/top_right_corner.png")))
		{
			fprintf(stderr, "Failed to load top_right_corner_texture!");
			return false;
		}
	}

	if (!museum_top_wall_texture.is_valid())
	{
		if (!museum_top_wall_texture.load_from_file(textures_path("museum/top_wall.png")))
		{
			fprintf(stderr, "Failed to load top_wall_texture!");
			return false;
		}
	}

	if (!museum_bottom_wall_texture.is_valid())
	{
		if (!museum_bottom_wall_texture.load_from_file(textures_path("museum/bottom_wall.png")))
		{
			fprintf(stderr, "Failed to load bottom_wall_texture!");
			return false;
		}
	}

	if (!museum_left_wall_texture.is_valid())
	{
		if (!museum_left_wall_texture.load_from_file(textures_path("museum/left_wall.png")))
		{
			fprintf(stderr, "Failed to load left_wall_texture!");
			return false;
		}
	}

	if (!museum_right_wall_texture.is_valid())
	{
		if (!museum_right_wall_texture.load_from_file(textures_path("museum/right_wall.png")))
		{
			fprintf(stderr, "Failed to load right_wall_texture!");
			return false;
		}
	}

	if (!museum_two_walls_texture.is_valid())
	{
		if (!museum_two_walls_texture.load_from_file(textures_path("museum/two_walls.png")))
		{
			fprintf(stderr, "Failed to load two_walls_texture!");
			return false;
		}
	}

	if (!museum_wall_texture.is_valid())
	{
		if (!museum_wall_texture.load_from_file(textures_path("museum/center_wall.png")))
		{
			fprintf(stderr, "Failed to load wall_texture!");
			return false;
		}
	}

	if (!museum_top_u_texture.is_valid())
	{
		if (!museum_top_u_texture.load_from_file(textures_path("museum/top_u.png")))
		{
			fprintf(stderr, "Failed to load top_u_texture!");
			return false;
		}
	}

	if (!museum_shadow_texture.is_valid())
	{
		if (!museum_shadow_texture.load_from_file(textures_path("museum/shadow.png")))
		{
			fprintf(stderr, "Failed to load shadow_texture!");
			return false;
		}
	}

	if (!museum_corridor_tile_texture.is_valid())
	{
		if (!museum_corridor_tile_texture.load_from_file(textures_path("museum/corridor_tile.png")))
		{
			fprintf(stderr, "Failed to load museum_corridor_tile_texture!");
			return false;
		}
	}

	if (!museum_corridor_tile_red_texture.is_valid())
	{
		if (!museum_corridor_tile_red_texture.load_from_file(textures_path("museum/corridor_tile_red.png")))
		{
			fprintf(stderr, "Failed to load museum_corridor_tile_red_texture!");
			return false;
		}
	}

	if (!museum_corridor_tile_green_texture.is_valid())
	{
		if (!museum_corridor_tile_green_texture.load_from_file(textures_path("museum/corridor_tile_green.png")))
		{
			fprintf(stderr, "Failed to load museum_corridor_tile_green_texture!");
			return false;
		}
	}

	if (!museum_corridor_tile_blue_texture.is_valid())
	{
		if (!museum_corridor_tile_blue_texture.load_from_file(textures_path("museum/corridor_tile_blue.png")))
		{
			fprintf(stderr, "Failed to load museum_corridor_tile_blue_texture!");
			return false;
		}
	}

	if (!museum_corridor_tile_yellow_texture.is_valid())
	{
		if (!museum_corridor_tile_yellow_texture.load_from_file(textures_path("museum/corridor_tile_yellow.png")))
		{
			fprintf(stderr, "Failed to load museum_corridor_tile_yellow_texture!");
			return false;
		}
	}

	/////////////////////////
	// Level: Ruins Textures
	/////////////////////////

	if (!ruins_bottom_left_corner_texture.is_valid())
	{
		if (!ruins_bottom_left_corner_texture.load_from_file(textures_path("ruins/bottom_left_corner.png")))
		{
			fprintf(stderr, "Failed to load bottom_left_corner_texture!");
			return false;
		}
	}

	if (!ruins_bottom_right_corner_texture.is_valid())
	{
		if (!ruins_bottom_right_corner_texture.load_from_file(textures_path("ruins/bottom_right_corner.png")))
		{
			fprintf(stderr, "Failed to load bottom_right_corner_texture!");
			return false;
		}
	}

	if (!ruins_top_left_corner_texture.is_valid())
	{
		if (!ruins_top_left_corner_texture.load_from_file(textures_path("ruins/top_left_corner.png")))
		{
			fprintf(stderr, "Failed to load top_left_corner_texture!");
			return false;
		}
	}

	if (!ruins_top_right_corner_texture.is_valid())
	{
		if (!ruins_top_right_corner_texture.load_from_file(textures_path("ruins/top_right_corner.png")))
		{
			fprintf(stderr, "Failed to load top_right_corner_texture!");
			return false;
		}
	}

	if (!ruins_top_wall_texture.is_valid())
	{
		if (!ruins_top_wall_texture.load_from_file(textures_path("ruins/top_wall.png")))
		{
			fprintf(stderr, "Failed to load top_wall_texture!");
			return false;
		}
	}

	if (!ruins_bottom_wall_texture.is_valid())
	{
		if (!ruins_bottom_wall_texture.load_from_file(textures_path("ruins/bottom_wall.png")))
		{
			fprintf(stderr, "Failed to load bottom_wall_texture!");
			return false;
		}
	}

	if (!ruins_left_wall_texture.is_valid())
	{
		if (!ruins_left_wall_texture.load_from_file(textures_path("ruins/left_wall.png")))
		{
			fprintf(stderr, "Failed to load left_wall_texture!");
			return false;
		}
	}

	if (!ruins_right_wall_texture.is_valid())
	{
		if (!ruins_right_wall_texture.load_from_file(textures_path("ruins/right_wall.png")))
		{
			fprintf(stderr, "Failed to load right_wall_texture!");
			return false;
		}
	}

	if (!ruins_two_walls_texture.is_valid())
	{
		if (!ruins_two_walls_texture.load_from_file(textures_path("ruins/two_walls.png")))
		{
			fprintf(stderr, "Failed to load two_walls_texture!");
			return false;
		}
	}

	if (!ruins_wall_texture.is_valid())
	{
		if (!ruins_wall_texture.load_from_file(textures_path("ruins/wall.png")))
		{
			fprintf(stderr, "Failed to load wall_texture!");
			return false;
		}
	}

	if (!ruins_top_u_texture.is_valid())
	{
		if (!ruins_top_u_texture.load_from_file(textures_path("ruins/top_u.png")))
		{
			fprintf(stderr, "Failed to load top_u_texture!");
			return false;
		}
	}

	if (!ruins_end_cap_texture.is_valid())
	{
		if (!ruins_end_cap_texture.load_from_file(textures_path("ruins/end_cap.png")))
		{
			fprintf(stderr, "Failed to load end_cap_texture!");
			return false;
		}
	}

	if (!ruins_shadow_texture.is_valid())
	{
		if (!ruins_shadow_texture.load_from_file(textures_path("ruins/shadow.png")))
		{
			fprintf(stderr, "Failed to load shadow_texture!");
			return false;
		}
	}

	// vertex buffer in local coordinates
	TexturedVertex vertices[4];
	vertices[0].position = {0.f, TILE_SIZE, 0.f}; // top left
	vertices[0].texcoord = {0.f, 1.f};
	vertices[1].position = {TILE_SIZE, TILE_SIZE, 0.f}; // top right
	vertices[1].texcoord = {1.f, 1.f};
	vertices[2].position = {TILE_SIZE, 0.f, 0.f}; // bottom right
	vertices[2].texcoord = {1.f, 0.f};
	vertices[3].position = {0.f, 0.f, 0.f}; // bottom left
	vertices[3].texcoord = {0.f, 0.f};

	// counterclockwise as it's the default opengl front winding direction
	uint16_t indices[] = {0, 3, 1, 1, 3, 2};

	// clea errors
	gl_flush_errors();

	// vertex buffer creation
	glGenBuffers(1, &mesh.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedVertex) * 4, vertices, GL_STATIC_DRAW);

	// index buffer creation
	glGenBuffers(1, &mesh.ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * 6, indices, GL_STATIC_DRAW);

	// vertex array (Container for Vertex + Index buffer)
	glGenVertexArrays(1, &mesh.vao);

	// clear errors
	gl_flush_errors();

	if (gl_has_errors())
		return false;

	// load shaders
	if (!effect.load_from_file(shader_path("map.vs.glsl"), shader_path("map.fs.glsl")))
		return false;

	physics.scale = {1.0f, 1.0f};

	for (int y = 0; y < 40; y++)
	{
		for (int x = 0; x < 61; x++)
		{
			current_level[y][x] = level_tutorial[y][x];
		}
	}

	return true;
}

// release all graphics resources
void Map::destroy()
{
	glDeleteBuffers(1, &mesh.vbo);
	glDeleteBuffers(1, &mesh.ibo);
	glDeleteVertexArrays(1, &mesh.vao);

	effect.release();
}

void Map::draw(const mat3 &projection)
{
	switch (get_current_map())
	{
	case LEVEL_TUTORIAL:
		draw_level_tutorial(projection);
		break;
	case LEVEL_1:
		draw_level_1(projection);
		break;
	case LEVEL_2:
		draw_level_2(projection);
		break;
	case LEVEL_3:
		draw_level_3(projection);
		break;
	case LEVEL_4:
		draw_level_4(projection);
		break;
	case LEVEL_5:
		draw_level_5(projection);
		break;
	default:
		draw_level_tutorial(projection);
		break;
	}
}

void Map::draw_level_tutorial(const mat3 &projection)
{
	translation_tile = vec2({0.0, 0.0});
	for (int y = 0; y < 40; y++)
	{
		// Increment the row
		for (int x = 0; x < 61; x++)
		{
			if (current_level[y][x] == 'W')
			{
				// Draw a Wall
				draw_element(projection, wall_texture);
			}
			else if (current_level[y][x] == 'S')
			{
				// Draw a Shadow Wall
				draw_element(projection, wall_light_texture);
			}
			else if ((current_level[y][x] == 'C') || (current_level[y][x] == 'A'))
			{
				// Draw a Corridor
				draw_element(projection, corridor_texture);
			}
			else if ((current_level[y][x] == 'Z'))
			{
				// Draw a Corridor
				draw_element(projection, trophy_texture);
			}
			else if (current_level[y][x] == 'R')
			{
				// Draw a Corridor
				draw_element(projection, corridor_texture_red);
			}
			else if (current_level[y][x] == 'B')
			{
				// Draw a Corridor
				draw_element(projection, corridor_texture_blue);
			}
			else if (current_level[y][x] == 'G')
			{
				// Draw a Corridor
				draw_element(projection, corridor_texture_green);
			}
			else if (current_level[y][x] == 'Y')
			{
				// Draw a Corridor
				draw_element(projection, corridor_texture_yellow);
			}

			translation_tile.x += TILE_SIZE;
		}
		translation_tile.x = 0.0;
		translation_tile.y += TILE_SIZE;
	}
}

void Map::draw_level_1(const mat3 &projection)
{
	translation_tile = vec2({0.0, 0.0});
	for (int y = 0; y < 40; y++)
	{
		// Increment the row
		for (int x = 0; x < 61; x++)
		{
			if (current_level[y][x] == '1')
			{
				draw_element(projection, museum_bottom_left_corner_texture);
			}
			else if (current_level[y][x] == '2')
			{
				draw_element(projection, museum_bottom_right_corner_texture);
			}
			else if (current_level[y][x] == '3')
			{
				draw_element(projection, museum_top_left_corner_texture);
			}
			else if (current_level[y][x] == '4')
			{
				draw_element(projection, museum_top_right_corner_texture);
			}
			else if (current_level[y][x] == '5')
			{
				draw_element(projection, museum_top_wall_texture);
			}
			else if (current_level[y][x] == '6')
			{
				draw_element(projection, museum_bottom_wall_texture);
			}
			else if (current_level[y][x] == '7')
			{
				draw_element(projection, museum_left_wall_texture);
			}
			else if (current_level[y][x] == '8')
			{
				draw_element(projection, museum_right_wall_texture);
			}
			else if (current_level[y][x] == 'S')
			{
				draw_element(projection, museum_shadow_texture);
			}
			else if (current_level[y][x] == 'U')
			{
				draw_element(projection, museum_top_u_texture);
			}
			else if (current_level[y][x] == '0')
			{
				draw_element(projection, museum_two_walls_texture);
			}
			else if (current_level[y][x] == 'W')
			{
				draw_element(projection, museum_wall_texture);
			}
			else if ((current_level[y][x] == 'C') || (current_level[y][x] == 'A'))
			{
				// Draw a Corridor
				draw_element(projection, museum_corridor_tile_texture);
			}
			else if ((current_level[y][x] == 'Z'))
			{
				// Draw a Corridor
				draw_element(projection, trophy_texture);
			}
			else if (current_level[y][x] == 'R')
			{
				// Draw a Corridor
				draw_element(projection, museum_corridor_tile_red_texture);
			}
			else if (current_level[y][x] == 'B')
			{
				// Draw a Corridor
				draw_element(projection, museum_corridor_tile_blue_texture);
			}
			else if (current_level[y][x] == 'G')
			{
				// Draw a Corridor
				draw_element(projection, museum_corridor_tile_green_texture);
			}
			else if (current_level[y][x] == 'Y')
			{
				// Draw a Corridor
				draw_element(projection, museum_corridor_tile_yellow_texture);
			}

			translation_tile.x += TILE_SIZE;
		}
		translation_tile.x = 0.0;
		translation_tile.y += TILE_SIZE;
	}
}

void Map::draw_level_2(const mat3 &projection)
{
	translation_tile = vec2({0.0, 0.0});
	for (int y = 0; y < 40; y++)
	{
		// Increment the row
		for (int x = 0; x < 61; x++)
		{
			if (current_level[y][x] == '1')
			{
				draw_element(projection, museum_bottom_left_corner_texture);
			}
			else if (current_level[y][x] == '2')
			{
				draw_element(projection, museum_bottom_right_corner_texture);
			}
			else if (current_level[y][x] == '3')
			{
				draw_element(projection, museum_top_left_corner_texture);
			}
			else if (current_level[y][x] == '4')
			{
				draw_element(projection, museum_top_right_corner_texture);
			}
			else if (current_level[y][x] == '5')
			{
				draw_element(projection, museum_top_wall_texture);
			}
			else if (current_level[y][x] == '6')
			{
				draw_element(projection, museum_bottom_wall_texture);
			}
			else if (current_level[y][x] == '7')
			{
				draw_element(projection, museum_left_wall_texture);
			}
			else if (current_level[y][x] == '8')
			{
				draw_element(projection, museum_right_wall_texture);
			}
			else if (current_level[y][x] == 'S')
			{
				draw_element(projection, museum_shadow_texture);
			}
			else if (current_level[y][x] == 'U')
			{
				draw_element(projection, museum_top_u_texture);
			}
			else if (current_level[y][x] == '0')
			{
				draw_element(projection, museum_two_walls_texture);
			}
			else if (current_level[y][x] == 'W')
			{
				draw_element(projection, museum_wall_texture);
			}
			else if ((current_level[y][x] == 'C') || (current_level[y][x] == 'A'))
			{
				// Draw a Corridor
				draw_element(projection, museum_corridor_tile_texture);
			}
			else if ((current_level[y][x] == 'Z'))
			{
				// Draw a Corridor
				draw_element(projection, trophy_texture);
			}
			else if (current_level[y][x] == 'R')
			{
				// Draw a Corridor
				draw_element(projection, museum_corridor_tile_red_texture);
			}
			else if (current_level[y][x] == 'B')
			{
				// Draw a Corridor
				draw_element(projection, museum_corridor_tile_blue_texture);
			}
			else if (current_level[y][x] == 'G')
			{
				// Draw a Corridor
				draw_element(projection, museum_corridor_tile_green_texture);
			}
			else if (current_level[y][x] == 'Y')
			{
				// Draw a Corridor
				draw_element(projection, museum_corridor_tile_yellow_texture);
			}

			translation_tile.x += TILE_SIZE;
		}
		translation_tile.x = 0.0;
		translation_tile.y += TILE_SIZE;
	}
}

void Map::draw_level_3(const mat3 &projection)
{
	translation_tile = vec2({0.0, 0.0});
	for (int y = 0; y < 40; y++)
	{
		// Increment the row
		for (int x = 0; x < 61; x++)
		{
			if (current_level[y][x] == '1')
			{
				draw_element(projection, museum_bottom_left_corner_texture);
			}
			else if (current_level[y][x] == '2')
			{
				draw_element(projection, museum_bottom_right_corner_texture);
			}
			else if (current_level[y][x] == '3')
			{
				draw_element(projection, museum_top_left_corner_texture);
			}
			else if (current_level[y][x] == '4')
			{
				draw_element(projection, museum_top_right_corner_texture);
			}
			else if (current_level[y][x] == '5')
			{
				draw_element(projection, museum_top_wall_texture);
			}
			else if (current_level[y][x] == '6')
			{
				draw_element(projection, museum_bottom_wall_texture);
			}
			else if (current_level[y][x] == '7')
			{
				draw_element(projection, museum_left_wall_texture);
			}
			else if (current_level[y][x] == '8')
			{
				draw_element(projection, museum_right_wall_texture);
			}
			else if (current_level[y][x] == 'S')
			{
				draw_element(projection, museum_shadow_texture);
			}
			else if (current_level[y][x] == 'U')
			{
				draw_element(projection, museum_top_u_texture);
			}
			else if (current_level[y][x] == '0')
			{
				draw_element(projection, museum_two_walls_texture);
			}
			else if (current_level[y][x] == 'W')
			{
				draw_element(projection, museum_wall_texture);
			}
			else if ((current_level[y][x] == 'C') || (current_level[y][x] == 'A'))
			{
				// Draw a Corridor
				draw_element(projection, museum_corridor_tile_texture);
			}
			else if ((current_level[y][x] == 'Z'))
			{
				// Draw a Corridor
				draw_element(projection, trophy_texture);
			}
			else if (current_level[y][x] == 'R')
			{
				// Draw a Corridor
				draw_element(projection, museum_corridor_tile_red_texture);
			}
			else if (current_level[y][x] == 'B')
			{
				// Draw a Corridor
				draw_element(projection, museum_corridor_tile_blue_texture);
			}
			else if (current_level[y][x] == 'G')
			{
				// Draw a Corridor
				draw_element(projection, museum_corridor_tile_green_texture);
			}
			else if (current_level[y][x] == 'Y')
			{
				// Draw a Corridor
				draw_element(projection, museum_corridor_tile_yellow_texture);
			}

			translation_tile.x += TILE_SIZE;
		}
		translation_tile.x = 0.0;
		translation_tile.y += TILE_SIZE;
	}
}

void Map::draw_level_4(const mat3 &projection)
{
	translation_tile = vec2({0.0, 0.0});
	for (int y = 0; y < 40; y++)
	{
		// Increment the row
		for (int x = 0; x < 61; x++)
		{
			if (current_level[y][x] == '1')
			{
				draw_element(projection, ruins_bottom_left_corner_texture);
			}
			else if (current_level[y][x] == '2')
			{
				draw_element(projection, ruins_bottom_right_corner_texture);
			}
			else if (current_level[y][x] == '3')
			{
				draw_element(projection, ruins_top_left_corner_texture);
			}
			else if (current_level[y][x] == '4')
			{
				draw_element(projection, ruins_top_right_corner_texture);
			}
			else if (current_level[y][x] == '5')
			{
				draw_element(projection, ruins_top_wall_texture);
			}
			else if (current_level[y][x] == '6')
			{
				draw_element(projection, ruins_bottom_wall_texture);
			}
			else if (current_level[y][x] == '7')
			{
				draw_element(projection, ruins_left_wall_texture);
			}
			else if (current_level[y][x] == '8')
			{
				draw_element(projection, ruins_right_wall_texture);
			}
			else if (current_level[y][x] == 'E')
			{
				draw_element(projection, ruins_end_cap_texture);
			}
			else if (current_level[y][x] == 'S')
			{
				draw_element(projection, ruins_shadow_texture);
			}
			else if (current_level[y][x] == 'U')
			{
				draw_element(projection, ruins_top_u_texture);
			}
			else if (current_level[y][x] == '0')
			{
				draw_element(projection, ruins_two_walls_texture);
			}
			else if (current_level[y][x] == 'W')
			{
				draw_element(projection, ruins_wall_texture);
			}
			else if ((current_level[y][x] == 'C') || (current_level[y][x] == 'A'))
			{
				// Draw a Corridor
				draw_element(projection, corridor_texture);
			}
			else if ((current_level[y][x] == 'Z'))
			{
				// Draw a Corridor
				draw_element(projection, trophy_texture);
			}
			else if (current_level[y][x] == 'R')
			{
				// Draw a Corridor
				draw_element(projection, corridor_texture_red);
			}
			else if (current_level[y][x] == 'B')
			{
				// Draw a Corridor
				draw_element(projection, corridor_texture_blue);
			}
			else if (current_level[y][x] == 'G')
			{
				// Draw a Corridor
				draw_element(projection, corridor_texture_green);
			}
			else if (current_level[y][x] == 'Y')
			{
				// Draw a Corridor
				draw_element(projection, corridor_texture_yellow);
			}

			translation_tile.x += TILE_SIZE;
		}
		translation_tile.x = 0.0;
		translation_tile.y += TILE_SIZE;
	}
}

void Map::draw_level_5(const mat3 &projection)
{
	translation_tile = vec2({ 0.0, 0.0 });
	for (int y = 0; y < 40; y++)
	{
		// Increment the row
		for (int x = 0; x < 61; x++)
		{
			if (current_level[y][x] == '1')
			{
				draw_element(projection, ruins_bottom_left_corner_texture);
			}
			else if (current_level[y][x] == '2')
			{
				draw_element(projection, ruins_bottom_right_corner_texture);
			}
			else if (current_level[y][x] == '3')
			{
				draw_element(projection, ruins_top_left_corner_texture);
			}
			else if (current_level[y][x] == '4')
			{
				draw_element(projection, ruins_top_right_corner_texture);
			}
			else if (current_level[y][x] == '5')
			{
				draw_element(projection, ruins_top_wall_texture);
			}
			else if (current_level[y][x] == '6')
			{
				draw_element(projection, ruins_bottom_wall_texture);
			}
			else if (current_level[y][x] == '7')
			{
				draw_element(projection, ruins_left_wall_texture);
			}
			else if (current_level[y][x] == '8')
			{
				draw_element(projection, ruins_right_wall_texture);
			}
			else if (current_level[y][x] == 'E')
			{
				draw_element(projection, ruins_end_cap_texture);
			}
			else if (current_level[y][x] == 'S')
			{
				draw_element(projection, ruins_shadow_texture);
			}
			else if (current_level[y][x] == 'U')
			{
				draw_element(projection, ruins_top_u_texture);
			}
			else if (current_level[y][x] == '0')
			{
				draw_element(projection, ruins_two_walls_texture);
			}
			else if (current_level[y][x] == 'W')
			{
				draw_element(projection, ruins_wall_texture);
			}
			else if ((current_level[y][x] == 'C') || (current_level[y][x] == 'A'))
			{
				// Draw a Corridor
				draw_element(projection, corridor_texture);
			}
			else if ((current_level[y][x] == 'Z'))
			{
				// Draw a Corridor
				draw_element(projection, trophy_texture);
			}
			else if (current_level[y][x] == 'R')
			{
				// Draw a Corridor
				draw_element(projection, corridor_texture_red);
			}
			else if (current_level[y][x] == 'B')
			{
				// Draw a Corridor
				draw_element(projection, corridor_texture_blue);
			}
			else if (current_level[y][x] == 'G')
			{
				// Draw a Corridor
				draw_element(projection, corridor_texture_green);
			}
			else if (current_level[y][x] == 'Y')
			{
				// Draw a Corridor
				draw_element(projection, corridor_texture_yellow);
			}

			translation_tile.x += TILE_SIZE;
		}
		translation_tile.x = 0.0;
		translation_tile.y += TILE_SIZE;
	}
}

void Map::draw_element(const mat3& projection, const Texture& texture)
{
	// transformation
	transform.begin();
	transform.translate(translation_tile);
	transform.scale(physics.scale);
	transform.end();

	// set shaders
	glUseProgram(effect.program);

	// enable alpha channel for textures
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// depth
	glEnable(GL_DEPTH_TEST);

	// get uniform locations for glUniform* calls
	GLint transform_uloc = glGetUniformLocation(effect.program, "transform");
	GLint color_uloc = glGetUniformLocation(effect.program, "fcolor");
	GLint projection_uloc = glGetUniformLocation(effect.program, "projection");
	GLint flash_map_uloc = glGetUniformLocation(effect.program, "flash_map");
	GLuint flash_timer_uloc = glGetUniformLocation(effect.program, "flash_timer");
	GLint spotter_location_uloc = glGetUniformLocation(effect.program, "spotter_loc");
	GLint vision_direction_uloc = glGetUniformLocation(effect.program, "vision_direction");

	// set vertices and indices
	glBindVertexArray(mesh.vao);
	glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);

	// input data location as in the vertex buffer
	GLint in_position_loc = glGetAttribLocation(effect.program, "in_position");
	GLint in_texcoord_loc = glGetAttribLocation(effect.program, "in_texcoord");
	glEnableVertexAttribArray(in_position_loc);
	glEnableVertexAttribArray(in_texcoord_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)0);
	glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void*)sizeof(vec3));

	// enable and binding texture to slot 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture.id);

	// set uniform values to the currently bound program
	glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float*)& transform.out);
	float color[] = { 1.f, 1.f, 1.f };
	glUniform3fv(color_uloc, 1, color);
	glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float*)& projection);
	glUniform1iv(flash_map_uloc, 1, &flash_map);
	glUniform1f(flash_timer_uloc, (m_flash_time > 0) ? (float)((glfwGetTime() - m_flash_time) * 10.0f) : -1);

	float closest_spotter_loc[] = { 0,0,0 };
	float spotter_look_direction[] = { 0,0,0 };
	
	if (m_spotters)
	{
		for (int i = 0; i < m_spotters->size(); i++)
		{
			vec2 pos = m_spotters->at(i).get_position();
			if ((abs(pos.x - translation_tile.x) < 100) && (abs(pos.y - translation_tile.y) < 100))
			{
				closest_spotter_loc[0] = pos.x;
				closest_spotter_loc[1] = pos.y;

				vec2 look_dir = m_spotters->at(i).direction;
				spotter_look_direction[0] = -look_dir.x;
				spotter_look_direction[1] = -look_dir.y;
				break;
			}
		}
	}

	glUniform3fv(spotter_location_uloc, 1, closest_spotter_loc);
	glUniform3fv(vision_direction_uloc, 1, spotter_look_direction);

	// draw
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

void Map::check_wall(Char &ch, const float ms)
{
	if (!ch.is_moving())
	{
		ch.set_wall_collision('U', false);
		ch.set_wall_collision('D', false);
		ch.set_wall_collision('L', false);
		ch.set_wall_collision('R', false);
		return;
	}

	// ch info
	vec2 pos = ch.get_position();
	vec2 box = ch.get_bounding_box();
	vec2 dir = ch.get_velocity();
	float step = ch.is_dashing() ? 2 * ch.get_speed() * (ms / 1000) : ch.get_speed() * (ms / 1000);

	// get 4 corners of char
	vec2 pos_top_left = {pos.x - box.x, pos.y - box.y};
	vec2 pos_top_right = {pos.x + box.x, pos.y - box.y};
	vec2 pos_bottom_left = {pos.x - box.x, pos.y + box.y};
	vec2 pos_bottom_right = {pos.x + box.x, pos.y + box.y};

	// initialize
	int tile_x_top_left = 0;
	int tile_y_top_left = 0;
	int tile_x_top_right = 0;
	int tile_y_top_right = 0;
	int tile_x_bottom_left = 0;
	int tile_y_bottom_left = 0;
	int tile_x_bottom_right = 0;
	int tile_y_bottom_right = 0;

	// correction
	float d = 0.f;

	// blue color
	if (ch.get_color() == 3)
	{
		dir.x = -dir.x;
		dir.y = -dir.y;
	}

	// up
	if (dir.y < 0)
	{
		tile_x_top_left = (int)pos_top_left.x / (int)TILE_SIZE;
		tile_y_top_left = (int)(pos_top_left.y - step) / (int)TILE_SIZE;
		tile_x_top_right = (int)pos_top_right.x / (int)TILE_SIZE;
		tile_y_top_right = (int)(pos_top_right.y - step) / (int)TILE_SIZE;
		if (is_wall_texture(current_level[tile_y_top_left][tile_x_top_left]) ||
			is_wall_texture(current_level[tile_y_top_right][tile_x_top_right]))
		{
			d = TILE_SIZE - (pos_top_left.y - (tile_y_top_left * TILE_SIZE));
			ch.change_position({0.f, d});
			ch.set_wall_collision('U', true);
		}
		else
		{
			ch.set_wall_collision('U', false);
		}
	}
	// down
	else if (dir.y > 0)
	{
		tile_x_bottom_left = (int)pos_bottom_left.x / (int)TILE_SIZE;
		tile_y_bottom_left = (int)(pos_bottom_left.y + step) / (int)TILE_SIZE;
		tile_x_bottom_right = (int)pos_bottom_right.x / (int)TILE_SIZE;
		tile_y_bottom_right = (int)(pos_bottom_right.y + step) / (int)TILE_SIZE;
		if (is_wall_texture(current_level[tile_y_bottom_left][tile_x_bottom_left]) ||
			is_wall_texture(current_level[tile_y_bottom_right][tile_x_bottom_right]))
		{
			d = (tile_y_bottom_left * TILE_SIZE) - pos_bottom_left.y - 0.001f;
			ch.change_position({0.f, d});
			ch.set_wall_collision('D', true);
		}
		else
		{
			ch.set_wall_collision('D', false);
		}
	}
	// left
	if (dir.x < 0)
	{
		tile_x_top_left = (int)(pos_top_left.x - step) / (int)TILE_SIZE;
		tile_y_top_left = (int)pos_top_left.y / (int)TILE_SIZE;
		tile_x_bottom_left = (int)(pos_bottom_left.x - step) / (int)TILE_SIZE;
		tile_y_bottom_left = (int)pos_bottom_left.y / (int)TILE_SIZE;
		if (is_wall_texture(current_level[tile_y_top_left][tile_x_top_left]) ||
			is_wall_texture(current_level[tile_y_bottom_left][tile_x_bottom_left]))
		{
			d = TILE_SIZE - (pos_top_left.x - (tile_x_top_left * TILE_SIZE));
			ch.change_position({d, 0.f});
			ch.set_wall_collision('L', true);
		}
		else
		{
			ch.set_wall_collision('L', false);
		}
	}
	// right
	else if (dir.x > 0)
	{
		tile_x_top_right = (int)(pos_top_right.x + step) / (int)TILE_SIZE;
		tile_y_top_right = (int)pos_top_right.y / (int)TILE_SIZE;
		tile_x_bottom_right = (int)(pos_bottom_right.x + step) / (int)TILE_SIZE;
		tile_y_bottom_right = (int)pos_bottom_right.y / (int)TILE_SIZE;
		if (is_wall_texture(current_level[tile_y_top_right][tile_x_top_right]) ||
			is_wall_texture(current_level[tile_y_bottom_right][tile_x_bottom_right]))
		{
			d = (tile_x_top_right * TILE_SIZE) - pos_top_right.x - 0.001f;
			ch.change_position({d, 0.f});
			ch.set_wall_collision('R', true);
		}
		else
		{
			ch.set_wall_collision('R', false);
		}
	}
}

void Map::set_current_map(int level)
{
	glfwSetTime(0);
	switch (level)
	{
	case LEVEL_TUTORIAL:
		for (int y = 0; y < 40; y++)
		{
			for (int x = 0; x < 61; x++)
			{
				current_level[y][x] = level_tutorial[y][x];
			}
		}
		current_level_indicator = LEVEL_TUTORIAL;
		break;
	case LEVEL_1:
		for (int y = 0; y < 40; y++)
		{
			for (int x = 0; x < 61; x++)
			{
				current_level[y][x] = map_level_1[y][x];
			}
		}
		current_level_indicator = LEVEL_1;
		break;
	case LEVEL_2:
		for (int y = 0; y < 40; y++)
		{
			for (int x = 0; x < 61; x++)
			{
				current_level[y][x] = map_level_2[y][x];
			}
		}
		current_level_indicator = LEVEL_2;
		break;
	case LEVEL_3:
		for (int y = 0; y < 40; y++)
		{
			for (int x = 0; x < 61; x++)
			{
				current_level[y][x] = map_level_3[y][x];
			}
		}
		current_level_indicator = LEVEL_3;
		break;
	case LEVEL_4:
		for (int y = 0; y < 40; y++)
		{
			for (int x = 0; x < 61; x++)
			{
				current_level[y][x] = map_level_4[y][x];
			}
		}
		current_level_indicator = LEVEL_4;
		break;
	case LEVEL_5:
		for (int y = 0; y < 40; y++)
		{
			for (int x = 0; x < 61; x++)
			{
				current_level[y][x] = map_level_5[y][x];
			}
		}
		current_level_indicator = LEVEL_5;
		break;
	}
}

int Map::get_current_map()
{
	return current_level_indicator;
}

vec2 Map::get_spawn_pos() const
{
	vec2 res = vec2({0.f, 0.f});

	for (int y = 0; y < 40; y++)
	{
		for (int x = 0; x < 61; x++)
		{
			if (current_level[y][x] == 'A')
			{
				res.x = x * TILE_SIZE + TILE_SIZE / 2;
				res.y = y * TILE_SIZE + TILE_SIZE / 2;
				return res;
			}
		}
	}
}

int Map::get_tile_type(vec2 pos)
{
	int x = (int)pos.x / (int)TILE_SIZE;
	int y = (int)pos.y / (int)TILE_SIZE;

	if (is_wall_texture(current_level[y][x]))
		return 1;
	else if (current_level[y][x] == 'C')
		return 6;
	else if (current_level[y][x] == 'A')
		return 6;
	else if (current_level[y][x] == 'Z')
		return 100;
	else if (current_level[y][x] == 'R')
		return 2;
	else if (current_level[y][x] == 'G')
		return 3;
	else if (current_level[y][x] == 'B')
		return 4;
	else if (current_level[y][x] == 'Y')
		return 5;
	else
		return 0;
}

bool Map::is_wall_texture(char tile)
{
	if (tile == '1' || tile == '2' || tile == '3' || tile == '4' || tile == '5' || tile == '6' || tile == '7' || tile == '8' ||
		tile == 'E' || tile == 'N' || tile == 'M' || tile == 'S' || tile == 'U' || tile == '0' || tile == 'W')
	{
		return true;
	}
	else
	{
		return false;
	}
}

////////////////////
// DEATH ANIM
////////////////////

void Map::set_char_dead()
{
	m_dead_time = glfwGetTime();
}

void Map::reset_char_dead_time()
{
	m_dead_time = -1;
}

float Map::get_char_dead_time() const
{
	return glfwGetTime() - m_dead_time;
}

////////////////////
// FLASH
////////////////////

void Map::set_flash(int value)
{
	flash_map = value;
	m_flash_time = glfwGetTime();
}

void Map::reset_flash_time()
{
	m_flash_time = glfwGetTime();
}

int Map::get_flash()
{
	return flash_map;
}
float Map::get_flash_time() const
{
	return glfwGetTime() - m_flash_time;
}

////////////////////
// PATHING
////////////////////

vec2 Map::get_tile_center_coords(vec2 tile_indices)
{
	return vec2{(tile_indices.x * TILE_SIZE) + TILE_SIZE / 2, (tile_indices.y * TILE_SIZE) + TILE_SIZE / 2};
}

vec2 Map::get_grid_coords(vec2 position)
{
	int truncated_x = (int)position.x / (int)TILE_SIZE;
	int truncated_y = (int)position.y / (int)TILE_SIZE;
	return vec2{(float)truncated_x, (float)truncated_y};
}

bool Map::is_wall(vec2 grid_coords)
{
	int x = grid_coords.x;
	int y = grid_coords.y;
	return is_wall_texture(current_level[y][x]);
}

bool Map::check_wall(vec2 spotter_pos, vec2 char_pos)
{
	bool top_right = false;
	bool top_left = false;
	bool bottom_right = false;
	bool bottom_left = false;
	int sPosX = (int)spotter_pos.x / 20;
	int sPosY = (int)spotter_pos.y / 20;

	int cPosX = (int)char_pos.x / 20;
	int cPosY = (int)char_pos.y / 20;

	if ((sPosX < cPosX) && (sPosY < cPosY))
	{

		for (int posX = sPosX; posX <= cPosX; posX++)
		{
			for (int posY = sPosY; posY <= cPosY; posY++)
			{
				top_right = top_right || is_wall_texture(current_level[posY][posX]);
			}
		}
	}
	else if ((sPosX < cPosX) && (sPosY > cPosY))
	{
		for (int posX = sPosX; posX <= cPosX; posX++)
		{
			for (int posY = sPosY; posY >= cPosY; posY--)
			{
				bottom_left = bottom_left || is_wall_texture(current_level[posY][posX]);
			}
		}
	}
	else if ((sPosX > cPosX) && (sPosY > cPosY))
	{
		for (int posX = sPosX; posX >= cPosX; posX--)
		{
			for (int posY = sPosY; posY >= cPosY; posY--)
			{
				bottom_right = bottom_right || is_wall_texture(current_level[posY][posX]);
			}
		}
	}
	else if ((sPosX > cPosX) && (sPosY < cPosY))
	{
		for (int posX = sPosX; posX >= cPosX; posX--)
		{
			for (int posY = sPosY; posY <= cPosY; posY++)
			{
				top_left = top_left || is_wall_texture(current_level[posY][posX]);
			}
		}
	}
	else if ((sPosX == cPosX) && (sPosY < cPosY))
	{
		for (int posY = sPosY; posY <= cPosY; posY++)
		{
			top_left = top_left || is_wall_texture(current_level[posY][sPosX]);
		}
	}
	else if ((sPosX == cPosX) && (sPosY > cPosY))
	{
		for (int posY = sPosY; posY >= cPosY; posY--)
		{
			top_left = top_left || is_wall_texture(current_level[posY][sPosX]);
		}
	}
	else if ((sPosX < cPosX) && (sPosY == cPosY))
	{
		for (int posX = sPosX; posX <= cPosY; posX++)
		{
			top_left = top_left || is_wall_texture(current_level[sPosY][posX]);
		}
	}
	else if ((sPosX > cPosX) && (sPosY == cPosY))
	{
		for (int posX = sPosX; posX >= cPosX; posX--)
		{
			top_left = top_left || is_wall_texture(current_level[sPosY][posX]);
		}
	}

	/*if (is_wall)
		printf("wall collision works!\n");*/

	//printf("wall detection : %d\n", (top_right || top_left || bottom_right || bottom_left));
	return (top_right || top_left || bottom_right || bottom_left);
}

void Map::set_spotter_list(std::vector<Spotter>& spotters)
{
	m_spotters = &spotters;
}