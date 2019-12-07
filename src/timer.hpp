#pragma once

// internal
#include "common.hpp"
#include "constants.hpp"

//Freetype
#include <ft2build.h>
#include FT_FREETYPE_H
// stdlib
#include <vector>

class Timer : public Entity
{
public:
	bool init();
	void destroy();
	void update(float ms);
	void draw(const mat3 &projection) override;
	void render_text(const std::string &str, FT_Face face, float x, float y, float sx, float sy);
	bool is_game_over();

private:
	GLuint m_instance_vbo;
	int seconds = 0;
	int minutes = 0;
};