#define _USE_MATH_DEFINES

// header
#include "timer.hpp"

// stdlib
#include <cmath>
#include <iostream>
#include <string>

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_GLCOREARB
const size_t GAME_TIME = 180; //in seconds

bool Timer::init()
{
    seconds = 0;
    minutes = 0;
    return true;
}

// release all graphics resources
void Timer::destroy()
{
    glDeleteBuffers(1, &mesh.vbo);
    glDeleteBuffers(1, &mesh.vao);
    effect.release();
}

void Timer::update(float ms)
{
    // TODO Update timer
    if ((int)glfwGetTime() == 0)
    {
        seconds = 0;
        minutes = 0;
    }

    int secs_in_min = 60;
    //coundown
    seconds = (int)(GAME_TIME - glfwGetTime());
    //count up
    // seconds = (int)glfwGetTime();
    minutes = seconds / secs_in_min;
    seconds = seconds % secs_in_min;

    // fprintf(stderr, " timer - %02i:%02i \n",minutes, seconds);
}
bool Timer::is_game_over()
{
    // TODO Update timer
    if (GAME_TIME - glfwGetTime() <= 0)
    {
        return true;
    }
    return false;
}

void Timer::draw(const mat3 &projection)
{
    FT_Library ft_lib{nullptr};
    FT_Face face{nullptr};

    auto cleanup = [&]() {
        FT_Done_Face(face);
        FT_Done_FreeType(ft_lib);

        glDeleteBuffers(1, &mesh.vbo);
        glDeleteBuffers(1, &mesh.vao);
        glDeleteVertexArrays(1, &mesh.vao);

        effect.release();
    };

    // Initialize and load our freetype face
    if (FT_Init_FreeType(&ft_lib) != 0)
    {
        std::cerr << "Couldn't initialize FreeType library\n";
        cleanup();
        // return 1;
    }

    if (FT_New_Face(ft_lib, textures_path("fonts/Emulogic.ttf"), 0, &face) != 0)
    {
        std::cerr << "Unable to load FreeSans.ttf\n";
        cleanup();
        // return 1;
    }

    const double SCALEX = 2.0 / SCREEN_WIDTH;
    const double SCALEY = 2.0 / SCREEN_HEIGHT;

    // Initialize our texture and VBOs
    glGenBuffers(1, &mesh.vbo);
    glGenVertexArrays(1, &mesh.vao);

    // Initialize shader
    if (!effect.load_from_file(shader_path("timer.vs.glsl"), shader_path("timer.fs.glsl")))
        fprintf(stderr, "Timer Shaders failed to load");

    // Get shader uniforms
    glBindAttribLocation(effect.program, 0, "in_Position");
    GLuint texUniform = glGetUniformLocation(effect.program, "tex");
    GLuint colorUniform = glGetUniformLocation(effect.program, "color");

    // Bind stuff
    glActiveTexture(GL_TEXTURE0);

    glBindVertexArray(mesh.vao);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glUseProgram(effect.program);
    glUniform4f(colorUniform, 1, 1, 1, 0.7);
    glUniform1i(texUniform, 0);

    FT_Set_Pixel_Sizes(face, 0, 50);
    std::string secs;
    if (seconds < 10)
        secs = "0" + std::to_string(seconds);
    else if (seconds >= 10)
        secs = std::to_string(seconds);
    std::string mins;
    if (minutes < 10)
        mins = "0" + std::to_string(minutes);
    else if (seconds >= 10)
        mins = std::to_string(minutes);

    render_text(mins + ":" + secs, face, -0.2, 0.85, SCALEX, SCALEY);

    glfwPollEvents();

    cleanup();
}

void Timer::render_text(const std::string &str, FT_Face face, float x, float y, float sx, float sy)
{
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    const FT_GlyphSlot glyph = face->glyph;

    for (auto c : str)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER) != 0)
            continue;

        glTexImage2D(GL_TEXTURE_2D, 0, GL_R8,
                     glyph->bitmap.width, glyph->bitmap.rows,
                     0, GL_RED, GL_UNSIGNED_BYTE, glyph->bitmap.buffer);

        const float vx = x + glyph->bitmap_left * sx;
        const float vy = y + glyph->bitmap_top * sy;
        const float w = glyph->bitmap.width * sx;
        const float h = glyph->bitmap.rows * sy;

        struct
        {
            float x, y, s, t;
        } data[6] = {
            {vx, vy, 0, 0},
            {vx, vy - h, 0, 1},
            {vx + w, vy, 1, 0},
            {vx + w, vy, 1, 0},
            {vx, vy - h, 0, 1},
            {vx + w, vy - h, 1, 1}};

        glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(float), data, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        x += (glyph->advance.x >> 6) * sx;
        y += (glyph->advance.y >> 6) * sy;
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
}
