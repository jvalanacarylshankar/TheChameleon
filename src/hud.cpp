// header
#include "hud.hpp"
#include <algorithm>
#include <cmath>
#include <string>
#include <iostream>

Texture Hud::hud;
Texture Hud::tooltip;

using namespace std;

bool Hud::init()
{

  // load shared texture
  if (!hud.is_valid())
  {
    if (!hud.load_from_file(textures_path("hud.png")))
    {
      fprintf(stderr, "Failed to load hud texture!");
      return false;
    }
  }

  if (!tooltip.is_valid())
  {
    if (!tooltip.load_from_file(textures_path("tooltip_red.png")))
    {
      fprintf(stderr, "Failed to load tooltip texture!");
      return false;
    }
  }

	// the position corresponds to the center of the texture
	float wr = std::max(SCREEN_WIDTH, SCREEN_HEIGHT) * 0.5f;
	float hr = std::max(SCREEN_WIDTH, SCREEN_HEIGHT) * 0.5f;

  TexturedVertex vertices[4];
  vertices[0].position = {-wr, +hr, -0.0f};
  vertices[0].texcoord = {0.f, 1.f};
  vertices[1].position = {+wr, +hr, -0.02f};
  vertices[1].texcoord = {1.f, 1.f};
  vertices[2].position = {+wr, -hr, -0.02f};
  vertices[2].texcoord = {1.f, 0.f};
  vertices[3].position = {-wr, -hr, -0.02f};
  vertices[3].texcoord = {0.f, 0.f};

  uint16_t indices[] = {0, 3, 1, 1, 3, 2};

  // clear errors
  gl_flush_errors();

  // vertex buffer creation
  glGenBuffers(1, &mesh.vbo);
  glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(TexturedVertex) * 4, vertices, GL_STATIC_DRAW);

  // index buffer creation
  glGenBuffers(1, &mesh.ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t) * 6, indices, GL_STATIC_DRAW);

  // vertex array (container for vertex + index buffer)
  glGenVertexArrays(1, &mesh.vao);
  if (gl_has_errors())
    return false;

  // load shaders
  if (!effect.load_from_file(shader_path("textured.vs.glsl"), shader_path("textured.fs.glsl")))
    return false;

  motion.position = {1095.f, 95.f};

  red_tooltip_position = {1040.f, 40.f};
  blue_tooltip_position = {990.f, 75.f};
  yellow_tooltip_position = {1057.f, 75.f};
  green_tooltip_position = {1030.f, 120.f};

  show_red_tooltip = false;
  show_blue_tooltip = false;
  show_yellow_tooltip = false;
  show_green_tooltip = false;

  physics.scale = {0.14f, 0.14f};
  tooltip_scale = {0.2f, 0.075f};

  return true;
}

// release all graphics resources
void Hud::destroy()
{
  glDeleteBuffers(1, &mesh.vbo);
  glDeleteBuffers(1, &mesh.ibo);
  glDeleteVertexArrays(1, &mesh.vao);

	effect.release();
}

void Hud::update(unsigned int game_state, vec2 char_position)
{
  if (game_state % 1000 == 0)
  {
    motion.position = {char_position.x + 95.f, char_position.y + 55.f};
    physics.scale = {0.035f, 0.035f};
    red_tooltip_position = {motion.position.x - 15, motion.position.y - 16.5f};
    blue_tooltip_position = {motion.position.x - 27, motion.position.y - 4};
    yellow_tooltip_position = {motion.position.x - 11, motion.position.y - 7};
    green_tooltip_position = {motion.position.x - 17, motion.position.y + 9.5f};
    tooltip_scale = {0.055f, 0.015f};
  }
}

// TODO -- duplicate usage of program
// -- duplicated draw code for both the indicate and tooltip
// -- note, could potentially remove the tooltip
void Hud::draw(const mat3 &projection)
{
  // hud
  // transformation
  transform.begin();
  transform.translate(motion.position);
  transform.rotate(motion.radians);
  transform.scale(physics.scale);
  transform.end();

  // set shaders
  glUseProgram(effect.program);

  // enable alpha channel for textures
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // depth
  glDisable(GL_DEPTH_TEST);

  // get uniform locations for glUniform* calls
  GLint transform_uloc = glGetUniformLocation(effect.program, "transform");
  GLint color_uloc = glGetUniformLocation(effect.program, "fcolor");
  GLint projection_uloc = glGetUniformLocation(effect.program, "projection");

  // set vertices and indices
  glBindVertexArray(mesh.vao);
  glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);

  // input data location as in the vertex buffer
  GLint in_position_loc = glGetAttribLocation(effect.program, "in_position");
  GLint in_texcoord_loc = glGetAttribLocation(effect.program, "in_texcoord");
  glEnableVertexAttribArray(in_position_loc);
  glEnableVertexAttribArray(in_texcoord_loc);
  glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void *)0);
  glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void *)sizeof(vec3));

  // enable and binding texture to slot 0
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, hud.id);

  // set uniform values to the currently bound program
  glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float *)&transform.out);
  float color[] = {1.f, 1.f, 1.f};
  glUniform3fv(color_uloc, 1, color);
  glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float *)&projection);

  // draw
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);

  // tooltip
  // TODO
  transform.begin();
  if (show_red_tooltip)
  {
    transform.translate(red_tooltip_position);
  }
  else if (show_yellow_tooltip)
  {
    transform.translate(yellow_tooltip_position);
  }
  else if (show_blue_tooltip)
  {
    transform.translate(blue_tooltip_position);
  }
  else if (show_green_tooltip)
  {
    transform.translate(green_tooltip_position);
  }
  transform.scale(tooltip_scale);
  transform.end();

  // set shaders
  glUseProgram(effect.program);

  // enable alpha channel for textures
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // depth
  glDisable(GL_DEPTH_TEST);

  // get uniform locations for glUniform* calls
  transform_uloc = glGetUniformLocation(effect.program, "transform");
  color_uloc = glGetUniformLocation(effect.program, "fcolor");
  projection_uloc = glGetUniformLocation(effect.program, "projection");

  // set vertices and indices
  glBindVertexArray(mesh.vao);
  glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ibo);

  // input data location as in the vertex buffer
  in_position_loc = glGetAttribLocation(effect.program, "in_position");
  in_texcoord_loc = glGetAttribLocation(effect.program, "in_texcoord");
  glEnableVertexAttribArray(in_position_loc);
  glEnableVertexAttribArray(in_texcoord_loc);
  glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void *)0);
  glVertexAttribPointer(in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex), (void *)sizeof(vec3));

  // enable and binding texture to slot 0
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, tooltip.id);

  // set uniform values to the currently bound program
  glUniformMatrix3fv(transform_uloc, 1, GL_FALSE, (float *)&transform.out);
  glUniform3fv(color_uloc, 1, color);
  glUniformMatrix3fv(projection_uloc, 1, GL_FALSE, (float *)&projection);

  // draw
  if (show_yellow_tooltip || show_red_tooltip || show_green_tooltip || show_blue_tooltip)
  {
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
  }
}

// TODO
void Hud::set_tooltip(char color, bool value)
{
  /* switch (color)
  {
  case 'R':
    show_red_tooltip = value;
    if (value)
    {
      tooltip.~Texture();
      tooltip.load_from_file(textures_path("tooltip_red.png"));
    }
    break;
  case 'G':
    show_green_tooltip = value;
    if (value)
    {
      tooltip.~Texture();
      tooltip.load_from_file(textures_path("tooltip_green.png"));
    }
    break;
  case 'B':
    show_blue_tooltip = value;
    if (value)
    {
      tooltip.~Texture();
      tooltip.load_from_file(textures_path("tooltip_blue.png"));
    }
    break;
  case 'Y':
    show_yellow_tooltip = value;
    if (value)
    {
      tooltip.~Texture();
      tooltip.load_from_file(textures_path("tooltip_yellow.png"));
    }
    break;
  } */
}