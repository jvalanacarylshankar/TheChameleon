// header
#include "world.hpp"

// stlib
#include <string.h>
#include <cassert>
#include <sstream>
#include <iostream>

namespace
{
// TODO -- need to remove
const size_t MAX_SPOTTERS = 5;
const size_t MAX_SHOOTERS = 5;

const size_t MAX_COOLDOWN = 50;
const size_t MAX_ALERT_MODE_COOLDOWN = 100;

const float FADE_TIME = 0.7;
const float FLASH_TIME = 1.5;
float pause_time = 0.f;

// TODO -- need to remove after settings locs
vector<vec2> spotter_loc;
vector<vec2> spotter_loc_level_2;
vec2 shooter_loc[5];

namespace
{
void glfw_err_cb(int error, const char *desc)
{
	fprintf(stderr, "%d: %s", error, desc);
}
} // namespace
} // namespace

World::World() : m_control(0),
				 m_current_game_state(0),
				 m_current_level_state(0),
				 m_current_pause_state(0),
				 m_current_game_won_state(2),
				 m_current_game_over_state(1),
				 m_paused(false),
				 m_game_state(START_SCREEN)
{
	// send rng with random device
	m_rng = std::default_random_engine(std::random_device()());
}

World::~World()
{
}

// initialization
bool World::init()
{
	// TODO -- need static spawn of spotters per level
	//spotter_loc[0] = m_map.get_tile_center_coords(vec2{ 1, 3 });
	spotter_loc.push_back({100, 100});
	spotter_loc.push_back({SCREEN_WIDTH - 100, 100});
	spotter_loc.push_back({100, SCREEN_HEIGHT - 100});
	spotter_loc.push_back({SCREEN_WIDTH - 100, SCREEN_HEIGHT - 100});
	spotter_loc.push_back({800, 500});

	// Level 2
	spotter_loc_level_2.push_back({ (15 * 20) + 10, (13 * 20) + 10 });
	spotter_loc_level_2.push_back({ (47 * 20) + 10, (15 * 20) + 10 });
	spotter_loc_level_2.push_back({ (29 * 20) + 10, (17 * 20) + 10 });
	spotter_loc_level_2.push_back({ (18 * 20) + 10, (26 * 20) + 10 });
	spotter_loc_level_2.push_back({ (38 * 20) + 10, (26 * 20) + 10 });

	// TODO
	shooter_loc[0] = {100 + 100, 100 + 50};
	shooter_loc[1] = {SCREEN_WIDTH - 50, 100 + 50};
	shooter_loc[2] = {150, SCREEN_HEIGHT - 150};
	shooter_loc[3] = {SCREEN_WIDTH - 50, SCREEN_HEIGHT - 50};
	shooter_loc[4] = {850, 550};

	// GLFW / OGL Initialization
	// Core Opengl 3.
	glfwSetErrorCallback(glfw_err_cb);
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW");
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, 1);
#if __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwWindowHint(GLFW_RESIZABLE, 0);
	m_window = glfwCreateWindow((int)SCREEN_WIDTH, (int)SCREEN_HEIGHT, "The Chameleon", nullptr, nullptr);
	if (m_window == nullptr)
		return false;

	glfwMakeContextCurrent(m_window);
	glfwSwapInterval(1); // vsync

	// load OpenGL function pointers
	gl3w_init();

	// set callbacks to member functions (that's why the redirect is needed)
	// input is handled using GLFW, for more info see
	// http://www.glfw.org/docs/latest/input_guide.html
	glfwSetWindowUserPointer(m_window, this);
	auto key_redirect = [](GLFWwindow *wnd, int _0, int _1, int _2, int _3) { ((World *)glfwGetWindowUserPointer(wnd))->on_key(wnd, _0, _1, _2, _3); };
	auto cursor_pos_redirect = [](GLFWwindow *wnd, double _0, double _1) { ((World *)glfwGetWindowUserPointer(wnd))->on_mouse_move(wnd, _0, _1); };
	glfwSetKeyCallback(m_window, key_redirect);
	glfwSetCursorPosCallback(m_window, cursor_pos_redirect);

	// create a frame buffer
	m_frame_buffer = 0;
	glGenFramebuffers(1, &m_frame_buffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_frame_buffer);

	// for some high DPI displays (ex. Retina Display on Macbooks)
	// https://stackoverflow.com/questions/36672935/why-retina-screen-coordinate-value-is-twice-the-value-of-pixel-value
	int fb_width, fb_height;
	glfwGetFramebufferSize(m_window, &fb_width, &fb_height);
	m_screen_scale = static_cast<float>(fb_width) / SCREEN_WIDTH;

	// initialize the screen texture
	m_screen_tex.create_from_screen(m_window);

	// load music and sounds
	if (SDL_Init(SDL_INIT_AUDIO) < 0)
	{
		fprintf(stderr, "Failed to initialize SDL Audio");
		return false;
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
	{
		fprintf(stderr, "Failed to open audio device");
		return false;
	}

	m_background_music = Mix_LoadMUS(audio_path("music.wav"));
	m_sfx_alert = Mix_LoadWAV(audio_path("sfx_alert.wav"));
	m_sfx_click = Mix_LoadWAV(audio_path("sfx_click.wav"));
	m_sfx_get_trophy = Mix_LoadWAV(audio_path("sfx_get_trophy.wav"));
	m_sfx_pause = Mix_LoadWAV(audio_path("sfx_pause.wav"));
	m_sfx_resume = Mix_LoadWAV(audio_path("sfx_resume.wav"));

	if (m_background_music == nullptr ||
		m_sfx_alert == nullptr ||
		m_sfx_click == nullptr ||
		m_sfx_get_trophy == nullptr ||
		m_sfx_pause == nullptr ||
		m_sfx_resume == nullptr)
	{
		fprintf(stderr, "Failed to load sounds\n");
		return false;
	}

	// play background music
	Mix_VolumeMusic(60);
	Mix_FadeInMusic(m_background_music, -1, 1000);
	fprintf(stderr, "Loaded music\n");

	m_alert_mode_cooldown = MAX_ALERT_MODE_COOLDOWN;
	m_cooldown = MAX_COOLDOWN;
	m_current_speed = 1.f;
	m_recent_dash = false;
	m_spawn_particles = false;

	return m_start_screen.init() &&
		   m_map.init() &&
		   m_char.init(m_map.get_spawn_pos(), m_map) &&
		   m_control_screen.init() &&
		   m_level_screen.init() &&
		   m_pause_screen.init() &&
		   m_cutscene.init() &&
		   m_hud.init() &&
		   m_overlay.init(m_alert_mode, MAX_COOLDOWN) &&
		   m_particles_emitter.init() &&
		   m_complete_screen.init() &&
		   m_gameover_screen.init() &&
		   m_timer.init();
}

// release all the associated resources
void World::destroy()
{
	glDeleteFramebuffers(1, &m_frame_buffer);

	if (m_background_music != nullptr)
		Mix_FreeMusic(m_background_music);
	if (m_sfx_alert != nullptr)
		Mix_FreeChunk(m_sfx_alert);
	if (m_sfx_get_trophy != nullptr)
		Mix_FreeChunk(m_sfx_get_trophy);
	if (m_sfx_pause != nullptr)
		Mix_FreeChunk(m_sfx_pause);
	if (m_sfx_resume != nullptr)
		Mix_FreeChunk(m_sfx_resume);

	Mix_CloseAudio();

	m_char.destroy();
	m_map.destroy();
	m_overlay.destroy();
	m_particles_emitter.destroy();
	for (auto &shooter : m_shooters)
		shooter.destroy();
	m_wanderers.clear();
	for (auto &spotter : m_spotters)
		spotter.destroy();
	m_spotters.clear();
	for (auto &wanderer : m_wanderers)
		wanderer.destroy();
	m_wanderers.clear();
	m_start_screen.destroy();
	m_control_screen.destroy();
	m_level_screen.destroy();
	m_pause_screen.destroy();
	m_cutscene.destroy();
	m_complete_screen.destroy();
	m_gameover_screen.destroy();
	m_hud.destroy();

	glfwDestroyWindow(m_window);
}

// update our game world
bool World::update(float ms)
{
	int w, h;
	glfwGetFramebufferSize(m_window, &w, &h);

	m_start_screen.update(m_current_game_state);
	m_level_screen.update(m_current_level_state);
	m_complete_screen.update(m_current_game_won_state);
	m_gameover_screen.update(m_current_game_over_state);
	m_cutscene.update();
	m_pause_screen.update(m_current_pause_state);

	//////////////////////
	// COOLDOWN
	//////////////////////
	if (!m_paused)
	{
		if (m_alert_mode_cooldown < MAX_ALERT_MODE_COOLDOWN)
		{
			m_alert_mode_cooldown++;
		}
		else
		{
			bool stay_alert = false;
			// unalert shooters
			for (auto &shooter : m_shooters)
				shooter.set_alert_mode(false);

			// unalert spotters
			for (auto &spotter : m_spotters)
				spotter.set_alert_mode(false);
			// unalert wanderers
			for (auto &wanderer : m_wanderers)
			{
				if (m_alert_mode)
				{
					if (m_char.is_in_range(wanderer, m_map))
					{
						// fprintf(stderr, "alert mode active and in range \n");
						stay_alert = true;
					}
					else
					{
						wanderer.set_alert_mode(false);
					}
				}
			}

			// if (stay_alert)
			// {
			// 	alert_mode = true;
			// }
			// else
			// {
			// 	alert_mode = false;
			// }
			m_alert_mode = stay_alert;
		}

		// IF ALERT MODE OVERLAY
		m_overlay.update_alert_mode(m_alert_mode);
		if (m_alert_mode)
		{
			m_overlay.oscillation();
		}

		// cooldown update
		if (!(m_cooldown >= MAX_COOLDOWN))
		{
			m_cooldown++;
			m_overlay.set_cooldown(m_cooldown);
		}

		//////////////////////
		// COLLISION
		//////////////////////

		// collision, char-wall
		m_map.check_wall(m_char, ms);

		// collision, char-wanderer
		for (const auto &wanderer : m_wanderers)
		{
			if (m_char.is_colliding(wanderer) && is_char_detectable())
			{
				if (m_char.is_alive())
				{
					m_map.set_char_dead();
				}
				m_char.kill();
				break;
			}
		}

		// collision, char-trophy
		if (m_map.get_tile_type(m_char.get_position()) == 100)
		{
			if (m_char.is_alive())
			{
				Mix_HaltMusic();
				Mix_PlayChannel(-1, m_sfx_get_trophy, 0);
				advance_to_cutscene();
				m_char.kill();
				return true;
			}
			m_char.kill();
		}

		// collision, char-spotter
		for (const auto &spotter : m_spotters)
		{
			if (m_char.is_colliding(spotter) && is_char_detectable())
			{
				if (m_char.is_alive())
				{
					m_map.set_char_dead();
				}
				m_char.kill();
				break;
			}
		}

		// proximity, char-shooter
		for (auto &shooter : m_shooters)
		{
			if (m_char.is_colliding(shooter) && is_char_detectable())
			{
				if (m_char.is_alive())
				{
					if (!m_alert_mode)
						Mix_PlayChannel(-1, m_sfx_alert, 0);
					m_alert_mode = true;
					m_alert_mode_cooldown = 0;

					// rotate to char
					float angle = atan2((m_char.get_position().y - shooter.get_position().y), (m_char.get_position().x - shooter.get_position().x));
					shooter.set_rotation(angle);

					// SHOOTING AND COOLDOWN
					shooter.set_in_combat(true);
					shooter.bullets.cooldown -= 30.f;
					if (shooter.bullets.cooldown < 0.f)
					{
						shooter.bullets.spawn_bullet(shooter.get_position(), angle);
						shooter.bullets.cooldown = 1500.f;
					}
				}
				break;
			}
		}

		// proximity, spotter
		for (auto &spotter : m_spotters)
		{
			if (spotter.is_in_sight(m_char, m_map) && is_char_detectable())
			{
				if (m_char.is_alive())
				{
					if (!m_alert_mode)
						Mix_PlayChannel(-1, m_sfx_alert, 0);
					m_alert_mode = true;
					spotter.set_alert_mode(m_alert_mode);
					m_alert_mode_cooldown = 0;
				}
				break;
			}
		}

		//////////////////////
		// UPDATE
		//////////////////////
		//update timer
		if (m_game_state == LEVEL_1 || m_game_state == LEVEL_2 || m_game_state == LEVEL_3 || m_game_state == LEVEL_4 || m_game_state == LEVEL_5)
			m_timer.update(ms);
		// update char
		m_char.update(ms);
		m_hud.update(m_game_state, m_char.get_position());

		// update wanderers
		for (auto &wanderer : m_wanderers)
		{
			wanderer.update(ms * m_current_speed);
			if (m_alert_mode)
			{
				if (m_char.is_in_alert_mode_range(wanderer))
				{
					// fprintf(stderr, "alert mode active and in range \n");
					wanderer.set_alert_mode(true);
				}
				else
				{
					wanderer.set_alert_mode(false);
				}
			}
			else
			{
				if (m_char.is_in_range(wanderer, m_map) && is_char_detectable() && !(m_char.is_dashing()))
				{
					// fprintf(stderr, "alert mode active and in range \n");
					wanderer.set_alert_mode(true);
				}
				else
				{
					wanderer.set_alert_mode(false);
				}
			}
		}

		// update spotters
		for (auto &spotter : m_spotters)
		{
			spotter.update(ms * m_current_speed);
		}

		// update shooter
		for (auto &shooter : m_shooters)
		{
			// TODO -- wrong location for proper code flow
			shooter.set_alert_mode(m_alert_mode);

			// TODO
			shooter.update(ms * m_current_speed);
			// angle to shooter, alternative solution to save bullet angle as part of bullet struct

			// TODO
			float angle = atan2((m_char.get_position().y - shooter.get_position().y), (m_char.get_position().x - shooter.get_position().x));

			// TODO -- wrong location for collision and code flow
			if (shooter.is_in_combat())
			{
				shooter.bullets.update(ms * m_current_speed);
				if (m_char.is_colliding(shooter.bullets))
				{
					m_char.set_color(0);
					m_cooldown = 0;
					// m_char.change_position({15.f * cos(angle), 15.f * sin(angle)});
					if ((angle >= -M_PI / 4) && (angle <= M_PI / 4))
								{
									m_char.change_direction(2);
									m_char.set_direction('R', true);

								}
							else if ((angle > M_PI / 4) && (angle <= 3* M_PI / 4))
								{
									m_char.change_direction(1);
									m_char.set_direction('D', true);
								}
							else if ((angle > 3 * M_PI / 4) || (angle <= 3 * -M_PI / 4))
								{
									m_char.change_direction(3);
									m_char.set_direction('L', true);
								}
							else if ((angle > 3 * -M_PI / 4) && (angle < -M_PI/4))
								{
									m_char.change_direction(0);
									m_char.set_direction('U', true);
								}
							m_char.set_dash(true);
				}
			}
		}

		//////////////////////
		// DYNAMIC SPAWN
		//////////////////////

		if (m_particles_emitter.get_fade_time() > FADE_TIME)
		{
			m_particles_emitter.reset_fade_time();
			m_particles_emitter.set_fade(0);
			m_particles_emitter.destroy();
			m_particles_emitter.init();
		}

		if (m_char.is_dashing())
		{
			if (m_char.is_wall_collision())
			{
				m_char.set_dash(false);
				m_recent_dash = true;
				m_spawn_particles = true;
				m_particles_emitter.spawn_particle(m_char.get_position(), m_char.get_direction());
				//fprintf(stderr, "DIRECTION CHANGE - %d", m_char.get_direction());
			}
			else
			{
				m_char.set_dash(true);
			}
		}
		if (m_recent_dash)
		{
			m_recent_dash = false;
			m_cooldown = 0;
			// fprintf(stderr, "DIRECTION CHANGE - %d", m_char.get_direction());
			if (m_char.get_direction() == 0)
				m_char.change_position({0.f, 5.f});
			else if (m_char.get_direction() == 1)
				m_char.change_position({0.f, -5.f});
			else if (m_char.get_direction() == 2)
				m_char.change_position({5.f, 0.f});
			else if (m_char.get_direction() == 3)
				m_char.change_position({-5.f, 0.f});
		}

		//particles update
		m_particles_emitter.update(ms);
		// Spawning new Particles
		if (m_spawn_particles)
		{
			//fprintf(stderr, "spawn pebble called");
			m_spawn_particles = false;
		}

		//////////////////////
		// CONSEQUENCES
		//////////////////////

		// yellow
		if (m_map.get_flash_time() > FLASH_TIME)
		{
			m_map.reset_flash_time();
			m_map.set_flash(0);
		}

		// red
		if (m_char.is_dashing())
			if (m_char.is_wall_collision())
				m_char.set_dash(false);
	}

	//////////////////////
	// RESET LEVEL
	//////////////////////
	if (m_game_state == LEVEL_1 || m_game_state == LEVEL_2 || m_game_state == LEVEL_3 || m_game_state == LEVEL_4 || m_game_state == LEVEL_5)
		if (m_timer.is_game_over())
			m_game_state = LOSE_SCREEN;

	if (!m_char.is_alive() && m_map.get_char_dead_time() > 2)
	{
		reset_game();
	}

	if (m_game_state == LEVEL_1)
	{

		//////////////////////
		// DYNAMIC SPAWN
		//////////////////////
		if (!m_paused)
		{
			// spawn wanderer
			while (m_wanderers.size() < wanderer_paths_level_1.size())
			{
				if (!spawn_wanderer(wanderer_paths_level_1[m_wanderers.size()]))
					return false;

				Wanderer& new_wanderer = m_wanderers.back();
			}
		}
	}
	else if (m_game_state == LEVEL_2)
	{

		//////////////////////
		// DYNAMIC SPAWN
		//////////////////////

		if (!m_paused)
		{
			// spawn spotter
			// TODO -- need static spawns of spotters
			while (m_spotters.size() < spotter_loc_level_2.size())
			{
				if (!spawn_spotter())
					return false;

				Spotter& new_spotter = m_spotters.back();

				new_spotter.set_position(spotter_loc_level_2[m_spotters.size() - 1]);

				if (m_spotters.size() == spotter_loc_level_2.size())
				{
					m_map.set_spotter_list(m_spotters);
				}
			}

			// spawn wanderer
			while (m_wanderers.size() < wanderer_paths_level_2.size())
			{
				if (!spawn_wanderer(wanderer_paths_level_2[m_wanderers.size()]))
					return false;

				Wanderer& new_wanderer = m_wanderers.back();
			}
		}
	}
	else if (m_game_state == LEVEL_3)
	{

		//////////////////////
		// DYNAMIC SPAWN
		//////////////////////

		if (!m_paused)
		{
			// spawn wanderer
			while (m_wanderers.size() < wanderer_paths.size())
			{
				if (!spawn_wanderer(wanderer_paths[m_wanderers.size()]))
					return false;

				Wanderer &new_wanderer = m_wanderers.back();
			}
		}
	}
	else if (m_game_state == LEVEL_4)
	{

		//////////////////////
		// DYNAMIC SPAWN
		//////////////////////

		if (!m_paused)
		{
			// spawn spotter
			// TODO -- need static spawns of spotters
			if (m_spotters.size() < spotter_loc.size())
			{
				if (!spawn_spotter())
					return false;

				Spotter &new_spotter = m_spotters.back();

				new_spotter.set_position(spotter_loc[m_spotters.size() - 1]);

				if (m_spotters.size() == spotter_loc.size())
				{
					m_map.set_spotter_list(m_spotters);
				}
			}

			// spawn wanderer
			while (m_wanderers.size() < wanderer_paths_2.size())
			{
				if (!spawn_wanderer(wanderer_paths_2[m_wanderers.size()]))
					return false;

				Wanderer &new_wanderer = m_wanderers.back();
			}
		}
	}
	else if (m_game_state == LEVEL_5)
	{
		//////////////////////
		// DYNAMIC SPAWN
		//////////////////////

		if (!m_paused)
		{
			// spawn spotter
			// TODO -- need static spawn of spotters per level
			if (m_spotters.size() < spotter_loc.size())
			{
				if (!spawn_spotter())
					return false;

				Spotter &new_spotter = m_spotters.back();

				new_spotter.set_position(spotter_loc[m_spotters.size() - 1]);

				if (m_spotters.size() == spotter_loc.size())
				{
					m_map.set_spotter_list(m_spotters);
				}
			}

			// spawn shooter
			if (m_shooters.size() < MAX_SHOOTERS)
			{
				if (!spawn_shooter())
					return false;
				Shooter &new_shooter = m_shooters.back();

				new_shooter.set_position(shooter_loc[m_shooters.size() - 1]);
			}

			// spawn wanderer
			while (m_wanderers.size() < wanderer_paths_3.size())
			{
				if (!spawn_wanderer(wanderer_paths_3[m_wanderers.size()]))
					return false;

				Wanderer &new_wanderer = m_wanderers.back();
			}
		}
	}

	return true;
}

// render
// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/
void World::draw()
{

	// fprintf(stderr, "Timer - %f", glfwGetTime());
	// clear error buffer
	gl_flush_errors();

	// get size of window
	int w, h;
	glfwGetFramebufferSize(m_window, &w, &h);

	// update window title with points
	std::stringstream title_ss;
	title_ss << "The Chameleon";
	glfwSetWindowTitle(m_window, title_ss.str().c_str());

	// first render to the custom framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// clear backbuffer
	glViewport(0, 0, w, h);
	glDepthRange(0.00001, 10);
	const float clear_color[3] = {0.3f, 0.3f, 0.3f};
	glClearColor(clear_color[0], clear_color[1], clear_color[2], 1.0);
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mat3 projection_2D = calculateProjectionMatrix(w, h);

	// game state
	switch (m_game_state)
	{
	case START_SCREEN:
		m_start_screen.draw(projection_2D);
		break;
	case CONTROL_SCREEN:
		m_control_screen.draw(projection_2D);
		break;
	case LEVEL_SCREEN:
		m_level_screen.draw(projection_2D);
		break;
	case STORY_SCREEN:
		m_cutscene.draw(projection_2D, m_screen_size, m_screen_point);
		break;
	case LEVEL_TUTORIAL:
		// draw map
		m_map.draw(projection_2D);
		m_cutscene.draw(projection_2D, m_screen_size, m_screen_point);

		if (m_map.get_flash() == 0)
		{
			m_char.draw(projection_2D);
			m_particles_emitter.draw(projection_2D);
		}

		m_hud.draw(projection_2D);
		//m_hud.draw(projection_2D, m_screen_size, m_screen_point);
		// draw timer
		// m_timer.draw(projection_2D);

		// bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_screen_tex.id);
		break;
	case LEVEL_1_CUTSCENE:
		m_cutscene.draw(projection_2D, m_screen_size, m_screen_point);
		break;
	case LEVEL_2_CUTSCENE:
		m_cutscene.draw(projection_2D, m_screen_size, m_screen_point);
		break;
	case LEVEL_3_CUTSCENE:
		m_cutscene.draw(projection_2D, m_screen_size, m_screen_point);
		break;
	case LEVEL_1:
		// draw map
		m_map.draw(projection_2D);
		if (m_map.get_flash() == 0)
		{
			// draw entities
			for (auto& wanderer : m_wanderers)
				wanderer.draw(projection_2D);
			// draw entities
			m_char.draw(projection_2D);
			m_particles_emitter.draw(projection_2D);
		}

		m_overlay.draw(projection_2D);
		m_hud.draw(projection_2D);
//m_hud.draw(projection_2D, m_screen_size, m_screen_point);
		// draw timer
		m_timer.draw(projection_2D);

		// bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_screen_tex.id);
		break;
	case LEVEL_2:
		// draw map
		m_map.draw(projection_2D);
		if (m_map.get_flash() == 0)
		{
			// draw entities
			for (auto& wanderer : m_wanderers)
				wanderer.draw(projection_2D);
			for (auto& spotter : m_spotters)
				spotter.draw(projection_2D);
			m_char.draw(projection_2D);
			m_particles_emitter.draw(projection_2D);
		}

		m_overlay.draw(projection_2D);
		m_hud.draw(projection_2D);
//m_hud.draw(projection_2D, m_screen_size, m_screen_point);
		// draw timer
		m_timer.draw(projection_2D);

		// bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_screen_tex.id);
		break;
	case LEVEL_3:
		// draw map
		m_map.draw(projection_2D);
		if (m_map.get_flash() == 0)
		{
			// draw entities
			for (auto &wanderer : m_wanderers)
				wanderer.draw(projection_2D);
			m_char.draw(projection_2D);
			m_particles_emitter.draw(projection_2D);
		}

		m_overlay.draw(projection_2D);
		m_hud.draw(projection_2D);
//m_hud.draw(projection_2D, m_screen_size, m_screen_point);
		// draw timer
		m_timer.draw(projection_2D);

		// bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_screen_tex.id);
		break;
	case LEVEL_4:
		// draw map
		m_map.draw(projection_2D);

		if (m_map.get_flash() == 0)
		{
			// draw entities
			for (auto &spotter : m_spotters)
				spotter.draw(projection_2D);
			for (auto &wanderer : m_wanderers)
				wanderer.draw(projection_2D);
			m_char.draw(projection_2D);
			m_particles_emitter.draw(projection_2D);
		}

		m_overlay.draw(projection_2D);
		m_hud.draw(projection_2D);
//m_hud.draw(projection_2D, m_screen_size, m_screen_point);
		// draw timer
		m_timer.draw(projection_2D);

		// bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_screen_tex.id);
		break;
	case LEVEL_5:
		// draw map
		m_map.draw(projection_2D);

		if (m_map.get_flash() == 0)
		{
			// draw entities
			for (auto &spotter : m_spotters)
				spotter.draw(projection_2D);
			for (auto &wanderer : m_wanderers)
				wanderer.draw(projection_2D);
			for (auto &shooter : m_shooters)
			{
				shooter.draw(projection_2D);
				if (shooter.is_in_combat())
				{
					shooter.bullets.draw(projection_2D);
				}
			}
			m_char.draw(projection_2D);
			m_particles_emitter.draw(projection_2D);
		}

		m_overlay.draw(projection_2D);
		m_hud.draw(projection_2D);
//m_hud.draw(projection_2D, m_screen_size, m_screen_point);
		// draw timer
		m_timer.draw(projection_2D);

		// bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_screen_tex.id);
		break;
	case WIN_SCREEN:
		m_complete_screen.draw(projection_2D);
		break;
	case LOSE_SCREEN:
		m_gameover_screen.draw(projection_2D);
		break;
	case PAUSE_SCREEN:
		m_pause_screen.draw(projection_2D);
		break;
	case QUIT:
		destroy();
		exit(0);
	}

	// present
	glfwSwapBuffers(m_window);
}

mat3 World::calculateProjectionMatrix(int width, int height)
{
	float left = 0.f; // *-0.5;
	float top = 0.f;  // (float)h * -0.5;
	float right = 0.f;
	float bottom = 0.f;

	if (m_game_state != START_SCREEN && m_game_state % 1000 == 0)
	{
		left = m_char.get_position().x - ((float)width / (PROJECTION_SCALE * m_screen_scale));
		top = m_char.get_position().y - ((float)height / (PROJECTION_SCALE * m_screen_scale));
		right = m_char.get_position().x + ((float)width / (PROJECTION_SCALE * m_screen_scale));
		bottom = m_char.get_position().y + ((float)height / (PROJECTION_SCALE * m_screen_scale));
	}
	else
	{
		right = (float)width / m_screen_scale;   // *0.5;
		bottom = (float)height / m_screen_scale; // *0.5;
	}

	// overlay reference info
	m_screen_size = vec2({right - left, bottom - top});
	m_screen_point = vec2({left, top});

	float sx = 2.f / (right - left);
	float sy = 2.f / (top - bottom);
	float tx = -(right + left) / (right - left);
	float ty = -(top + bottom) / (top - bottom);
	return {{sx, 0.f, 0.f}, {0.f, sy, 0.f}, {tx, ty, 1.f}};
}

bool World::is_over() const
{
	return glfwWindowShouldClose(m_window);
}

// spawn spotter
bool World::spawn_spotter()
{
	Spotter spotter;
	if (spotter.init())
	{
		m_spotters.emplace_back(spotter);
		return true;
	}
	fprintf(stderr, "Failed to spawn spotter");
	return false;
}

// spawn spotter
bool World::spawn_shooter()
{
	Shooter shooter;
	if (shooter.init())
	{
		shooter.bullets.init();
		m_shooters.emplace_back(shooter);
		return true;
	}
	fprintf(stderr, "Failed to spawn spotter");
	return false;
}

// spawn wanderer
bool World::spawn_wanderer(std::vector<vec2> path)
{
	Wanderer wanderer;
	if (wanderer.init(path, m_map, m_char))
	{
		m_wanderers.emplace_back(wanderer);
		return true;
	}
	fprintf(stderr, "Failed to spawn wanderer");
	return false;
}

// key callback function
void World::on_key(GLFWwindow *, int key, int, int action, int mod)
{
	// start screen, control screen, story screen
	if (m_game_state != PAUSE_SCREEN && m_game_state != LEVEL_1 && m_game_state != LEVEL_2 && m_game_state != LEVEL_3 && m_game_state != LEVEL_4 && m_game_state != LEVEL_5)
	{
		if (m_game_state == START_SCREEN)
		{
			if (action == GLFW_PRESS && key == GLFW_KEY_DOWN)
				if (m_current_game_state < 3)
					m_current_game_state++;

			if (action == GLFW_PRESS && key == GLFW_KEY_UP)
				if (m_current_game_state > 0)
					m_current_game_state--;
		}
		else if (m_game_state == LEVEL_SCREEN)
		{
			if (action == GLFW_PRESS && key == GLFW_KEY_DOWN)
				if (m_current_level_state < 5)
					m_current_level_state++;

			if (action == GLFW_PRESS && key == GLFW_KEY_UP)
				if (m_current_level_state > 0)
					m_current_level_state--;
		}
		else if (m_game_state == WIN_SCREEN)
		{
			if (action == GLFW_PRESS && key == GLFW_KEY_DOWN)
				if (m_current_game_won_state == MAIN_MENU)
					m_current_game_won_state = QUIT;

			if (action == GLFW_PRESS && key == GLFW_KEY_UP)
				if (m_current_game_won_state == QUIT)
					m_current_game_won_state = MAIN_MENU;
		}
		else if (m_game_state == LOSE_SCREEN)
		{
			if (action == GLFW_PRESS && key == GLFW_KEY_DOWN)
				if (m_current_game_over_state == RESTART)
					m_current_game_over_state = MAIN_MENU;

			if (action == GLFW_PRESS && key == GLFW_KEY_UP)
				if (m_current_game_over_state == MAIN_MENU)
					m_current_game_over_state = RESTART;
		}

		if (action == GLFW_PRESS && key == GLFW_KEY_ENTER)
		{
			if (m_game_state == STORY_SCREEN)
			{
				if (m_cutscene.dialogue_done(m_game_state))
				{
					m_game_state = LEVEL_TUTORIAL;
					m_map.set_current_map(LEVEL_TUTORIAL);
					m_char.set_position(m_map.get_spawn_pos());
					m_cutscene.increment_dialogue_counter(m_game_state);

					Mix_PlayChannel(-1, m_sfx_click, 0);
				}
				else
					m_cutscene.increment_dialogue_counter(m_game_state);
			}
			else if (m_game_state == LEVEL_TUTORIAL)
			{
				if (m_cutscene.dialogue_done(m_game_state))
				{
					m_game_state = LEVEL_1_CUTSCENE;
					m_char.set_position(m_map.get_spawn_pos());
					m_cutscene.increment_dialogue_counter(m_game_state);
				}
				else
					m_cutscene.increment_dialogue_counter(m_game_state);
			}
			else if (m_game_state == LEVEL_1_CUTSCENE)
			{
				if (m_cutscene.dialogue_done(m_game_state))
				{
					m_game_state = LEVEL_1;
					m_level = m_game_state;
					m_map.set_current_map(LEVEL_1);
					m_char.set_position(m_map.get_spawn_pos());
					m_cutscene.increment_dialogue_counter(m_game_state);
				}
				else
					m_cutscene.increment_dialogue_counter(m_game_state);
			}
			else if (m_game_state == LEVEL_2_CUTSCENE)
			{
				if (m_cutscene.dialogue_done(m_game_state))
				{
					m_game_state = LEVEL_2;
					m_level = m_game_state;
					m_map.set_current_map(LEVEL_2);
					m_char.set_position(m_map.get_spawn_pos());
					m_cutscene.increment_dialogue_counter(m_game_state);

					Mix_VolumeMusic(25);
				}
				else
					m_cutscene.increment_dialogue_counter(m_game_state);
			}
			else if (m_game_state == LEVEL_3_CUTSCENE)
			{
				if (m_cutscene.dialogue_done(m_game_state))
				{
					m_game_state = LEVEL_3;
					m_level = m_game_state;
					m_map.set_current_map(LEVEL_3);
					m_char.set_position(m_map.get_spawn_pos());
					m_cutscene.increment_dialogue_counter(m_game_state);

					Mix_VolumeMusic(25);
				}
				else
					m_cutscene.increment_dialogue_counter(m_game_state);
			}
			else if (m_game_state == WIN_SCREEN)
			{
				if (m_current_game_won_state == MAIN_MENU)
				{
					m_game_state = START_SCREEN;
					m_cutscene.set_dialogue_counter(m_game_state, 1);
					m_current_game_state = 0;
					reset_game();

					Mix_PlayChannel(-1, m_sfx_click, 0);
				}
				else if (m_current_game_won_state == QUIT)
				{
					m_game_state = QUIT;

					Mix_PlayChannel(-1, m_sfx_click, 0);
				}
			}
			else if (m_game_state == LOSE_SCREEN)
			{
				if (m_current_game_over_state == MAIN_MENU)
				{
					m_current_game_over_state = RESTART;
					m_game_state = START_SCREEN;
					m_cutscene.set_dialogue_counter(m_game_state, 1);
					m_current_game_state = 0;
					reset_game();

					Mix_PlayChannel(-1, m_sfx_click, 0);
				}
				else if (m_current_game_over_state == RESTART)
				{
					m_current_game_over_state = RESTART;
					m_game_state = m_level;
					reset_game();

					Mix_PlayChannel(-1, m_sfx_click, 0);
				}
			}
			else if (m_current_game_state == 0)
			{
				m_game_state = STORY_SCREEN;
				m_cutscene.set_dialogue_counter(m_game_state, 1);

				Mix_PlayChannel(-1, m_sfx_click, 0);
			}
			else if (m_game_state == CONTROL_SCREEN)
			{
				m_game_state = START_SCREEN;
				m_cutscene.set_dialogue_counter(m_game_state, 1);

				Mix_PlayChannel(-1, m_sfx_click, 0);
			}
			else if (m_game_state == LEVEL_SCREEN)
			{
				switch (m_current_level_state)
				{
				case 0:
					m_game_state = LEVEL_TUTORIAL;
					m_map.set_current_map(LEVEL_TUTORIAL);
					m_char.set_position(m_map.get_spawn_pos());
					m_cutscene.set_dialogue_counter(m_game_state, 28);
					m_current_level_state = 0;
					break;
				case 1:
					m_game_state = LEVEL_1_CUTSCENE;
					m_cutscene.set_dialogue_counter(m_game_state, 50);
					m_current_level_state = 0;
					break;
				case 2:
					m_game_state = LEVEL_2_CUTSCENE;
					m_cutscene.set_dialogue_counter(m_game_state, 70);
					m_current_level_state = 0;
					break;
				case 3:
					m_game_state = LEVEL_3_CUTSCENE;
					m_cutscene.set_dialogue_counter(m_game_state, 81);
					m_current_level_state = 0;
					break;
				case 4:
					m_game_state = LEVEL_4;
					m_level = m_game_state;
					m_map.set_current_map(LEVEL_4);
					m_char.set_position(m_map.get_spawn_pos());
					m_current_level_state = 0;
					break;
				case 5:
					m_game_state = LEVEL_5;
					m_level = m_game_state;
					m_map.set_current_map(LEVEL_5);
					m_char.set_position(m_map.get_spawn_pos());
					m_current_level_state = 0;
					break;
				}

				Mix_PlayChannel(-1, m_sfx_click, 0);
			}
			else
			{
				m_game_state = m_current_game_state;
				m_cutscene.set_dialogue_counter(m_game_state, 1);

				Mix_PlayChannel(-1, m_sfx_click, 0);
			}
		}
	}

	// ESC: return to start screen or pause screen
	if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE)
	{
		if (m_game_state == PAUSE_SCREEN || m_game_state == LEVEL_1 || m_game_state == LEVEL_2 || m_game_state == LEVEL_3 || m_game_state == LEVEL_4 || m_game_state == LEVEL_5)
		{
			if (!m_paused)
			{
				m_paused = true;
				m_level = m_game_state;
				m_game_state = PAUSE_SCREEN;
				pause_time = glfwGetTime();
				Mix_PlayChannel(-1, m_sfx_pause, 0);
			}
			else
			{
				m_paused = false;
				m_game_state = m_level;
				glfwSetTime(pause_time);
				Mix_PlayChannel(-1, m_sfx_resume, 0);
			}
		}
		else
		{
			m_current_game_state = 0;
			m_game_state = START_SCREEN;
			reset_game();
		}
	}

	// pause screen
	if (action == GLFW_PRESS && m_paused && m_game_state == PAUSE_SCREEN)
	{
		if (action == GLFW_PRESS && key == GLFW_KEY_DOWN)
			if (m_current_pause_state < 3)
				m_current_pause_state++;

		if (action == GLFW_PRESS && key == GLFW_KEY_UP)
			if (m_current_pause_state > 0)
				m_current_pause_state--;

		if (action == GLFW_PRESS && key == GLFW_KEY_ENTER)
		{
			if (m_current_pause_state == RESUME)
			{
				m_current_pause_state = 0;
				m_paused = !m_paused;
				glfwSetTime(pause_time);
				m_game_state = m_level;

				Mix_PlayChannel(-1, m_sfx_resume, 0);
			}
			else if (m_current_pause_state == RESTART)
			{
				m_current_pause_state = 0;
				m_paused = !m_paused;
				m_game_state = m_level;
				reset_game();

				Mix_PlayChannel(-1, m_sfx_click, 0);
			}
			else if (m_current_pause_state == MAIN_MENU)
			{
				m_current_pause_state = 0;
				m_paused = !m_paused;
				m_game_state = START_SCREEN;
				m_current_game_state = 0;
				reset_game();

				Mix_PlayChannel(-1, m_sfx_click, 0);
			}
			else if (m_current_pause_state == QUIT)
			{
				m_current_pause_state = 0;
				m_paused = !m_paused;
				m_game_state = QUIT;

				Mix_PlayChannel(-1, m_sfx_click, 0);
			}
		}
	}
	// movement, set movement
	if (action == GLFW_PRESS && !m_paused && (m_game_state == LEVEL_1 || m_game_state == LEVEL_2 || m_game_state == LEVEL_3 || m_game_state == LEVEL_4 || m_game_state == LEVEL_5 || m_game_state == LEVEL_TUTORIAL) && !m_char.is_dashing())
	{
		if ((key == GLFW_KEY_W && m_control == 0) || (key == GLFW_KEY_UP && m_control == 1))
		{
			m_char.set_direction('U', true);
			m_char.change_direction(0);
		}
		else if ((key == GLFW_KEY_S && m_control == 0) || (key == GLFW_KEY_DOWN && m_control == 1))
		{
			m_char.set_direction('D', true);
			m_char.change_direction(1);
		}
		else if ((key == GLFW_KEY_A && m_control == 0) || (key == GLFW_KEY_LEFT && m_control == 1))
		{
			m_char.set_direction('L', true);
			m_char.change_direction(2);
		}
		else if ((key == GLFW_KEY_D && m_control == 0) || (key == GLFW_KEY_RIGHT && m_control == 1))
		{
			m_char.set_direction('R', true);
			m_char.change_direction(3);
		}
	}

	// color, set color, consequences
	if (action == GLFW_PRESS && !m_paused && m_cooldown >= MAX_COOLDOWN && !m_char.is_dashing() &&
		(m_game_state == LEVEL_1 || m_game_state == LEVEL_2 || m_game_state == LEVEL_3 || m_game_state == LEVEL_4 || m_game_state == LEVEL_5 || m_game_state == LEVEL_TUTORIAL))
	{
		// red
		if (((key == GLFW_KEY_UP && m_control == 0) || (key == GLFW_KEY_W && m_control == 1)) && m_char.get_color() != 1)
		{
			m_char.set_dash(true);
			m_char.set_color(1);
		}
		// green
		else if (((key == GLFW_KEY_DOWN && m_control == 0) || (key == GLFW_KEY_S && m_control == 1)))
		{
			m_cooldown = 0;
			m_char.set_color(2);
			if (!m_alert_mode)
				Mix_PlayChannel(-1, m_sfx_alert, 0);
			m_alert_mode = true;
			m_alert_mode_cooldown = 0;
		}
		// blue
		else if (((key == GLFW_KEY_LEFT && m_control == 0) || (key == GLFW_KEY_A && m_control == 1)))
		{
			m_cooldown = 0;
			m_char.set_color(3);
		}
		// yellow
		else if (((key == GLFW_KEY_RIGHT && m_control == 0) || (key == GLFW_KEY_D && m_control == 1)))
		{
			m_char.set_color(4);
			m_map.set_flash(1);
			// Alert mode removed from yellow
			// m_alert_mode = true;
			// m_alert_mode_cooldown = 0;
			m_cooldown = 0;
		}
	}

	// remove movement
	if (action == GLFW_RELEASE && !m_paused && (m_game_state == LEVEL_1 || m_game_state == LEVEL_2 || m_game_state == LEVEL_3 || m_game_state == LEVEL_4 || m_game_state == LEVEL_5 || m_game_state == LEVEL_TUTORIAL))
	{
		if ((key == GLFW_KEY_D && m_control == 0) || (key == GLFW_KEY_RIGHT && m_control == 1))
			m_char.set_direction('R', false);
		else if ((key == GLFW_KEY_A && m_control == 0) || (key == GLFW_KEY_LEFT && m_control == 1))
			m_char.set_direction('L', false);
		else if ((key == GLFW_KEY_W && m_control == 0) || (key == GLFW_KEY_UP && m_control == 1))
			m_char.set_direction('U', false);
		else if ((key == GLFW_KEY_S && m_control == 0) || (key == GLFW_KEY_DOWN && m_control == 1))
			m_char.set_direction('D', false);
	}

	// game mode
	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_1)
			m_control = 0;
		else if (key == GLFW_KEY_2)
			m_control = 1;
	}

	// skip dialogues
	if (action == GLFW_PRESS && key == GLFW_KEY_SPACE)
	{
		if (m_game_state == STORY_SCREEN)
		{
			m_game_state = LEVEL_TUTORIAL;
			m_map.set_current_map(LEVEL_TUTORIAL);
			m_char.set_position(m_map.get_spawn_pos());
			m_cutscene.set_dialogue_counter(m_game_state, 28);
		}
		else if (m_game_state == LEVEL_1_CUTSCENE)
		{
			m_game_state = LEVEL_1;
			m_level = m_game_state;
			m_map.set_current_map(LEVEL_1);
			m_char.set_position(m_map.get_spawn_pos());
			m_cutscene.set_dialogue_counter(m_game_state, 50);

			Mix_VolumeMusic(25);
		}
		else if (m_game_state == LEVEL_2_CUTSCENE)
		{
			m_game_state = LEVEL_2;
			m_level = m_game_state;
			m_map.set_current_map(LEVEL_2);
			m_char.set_position(m_map.get_spawn_pos());
			m_cutscene.set_dialogue_counter(m_game_state, 70);
		}
		else if (m_game_state == LEVEL_3_CUTSCENE)
		{
			m_game_state = LEVEL_3;
			m_level = m_game_state;
			m_map.set_current_map(LEVEL_3);
			m_char.set_position(m_map.get_spawn_pos());
			m_cutscene.set_dialogue_counter(m_game_state, 81);
		}
		Mix_VolumeMusic(25);
	}

	// reset
	if (action == GLFW_RELEASE && key == GLFW_KEY_R)
	{
		//reset_game();
	}

	// game current speed
	if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_COMMA)
		m_current_speed -= 0.1f;
	if (action == GLFW_RELEASE && (mod & GLFW_MOD_SHIFT) && key == GLFW_KEY_PERIOD)
		m_current_speed += 0.1f;

	m_current_speed = fmax(0.f, m_current_speed);
}

void World::on_mouse_move(GLFWwindow *window, double xpos, double ypos)
{
	// TODO -- reference indicator position to get positions instead of hardcoding
	if (m_game_state == LEVEL_1 || m_game_state == LEVEL_2 || m_game_state == LEVEL_3 || m_game_state == LEVEL_4 || m_game_state == LEVEL_5)
	{
		// red tooltip
		if (xpos >= 1026 && xpos <= 1074 && ypos >= 59 && ypos <= 106)
		{
			m_hud.set_tooltip('R', true);
			m_hud.set_tooltip('G', false);
			m_hud.set_tooltip('B', false);
			m_hud.set_tooltip('Y', false);
		}
		// green tooltip
		else if (xpos >= 1026 && xpos <= 1074 && ypos >= 178 && ypos <= 226)
		{
			m_hud.set_tooltip('R', false);
			m_hud.set_tooltip('G', true);
			m_hud.set_tooltip('B', false);
			m_hud.set_tooltip('Y', false);
		}
		// blue tooltip
		else if (xpos >= 968 && xpos <= 1015 && ypos >= 116 && ypos <= 165)
		{
			m_hud.set_tooltip('R', false);
			m_hud.set_tooltip('G', false);
			m_hud.set_tooltip('B', true);
			m_hud.set_tooltip('Y', false);
		}
		// yellow tooltip
		else if (xpos >= 1089 && xpos <= 1138 && ypos >= 116 && ypos <= 165)
		{
			m_hud.set_tooltip('R', false);
			m_hud.set_tooltip('G', false);
			m_hud.set_tooltip('B', false);
			m_hud.set_tooltip('Y', true);
		}
		else
		{
			m_hud.set_tooltip('R', false);
			m_hud.set_tooltip('G', false);
			m_hud.set_tooltip('B', false);
			m_hud.set_tooltip('Y', false);
		}
	}
	else if (m_game_state == LEVEL_TUTORIAL)
	{
		// red tooltip
		if (xpos >= 1053 && xpos <= 1094 && ypos >= 52 && ypos <= 91)
		{
			m_hud.set_tooltip('R', true);
			m_hud.set_tooltip('G', false);
			m_hud.set_tooltip('B', false);
			m_hud.set_tooltip('Y', false);
		}
		// green tooltip
		else if (xpos >= 1053 && xpos <= 1094 && ypos >= 153 && ypos <= 191)
		{
			m_hud.set_tooltip('R', false);
			m_hud.set_tooltip('G', true);
			m_hud.set_tooltip('B', false);
			m_hud.set_tooltip('Y', false);
		}
		// blue tooltip
		else if (xpos >= 1004 && xpos <= 1044 && ypos >= 101 && ypos <= 140)
		{
			m_hud.set_tooltip('R', false);
			m_hud.set_tooltip('G', false);
			m_hud.set_tooltip('B', true);
			m_hud.set_tooltip('Y', false);
		}
		// yellow tooltip
		else if (xpos >= 1108 && xpos <= 1146 && ypos >= 101 && ypos <= 140)
		{
			m_hud.set_tooltip('R', false);
			m_hud.set_tooltip('G', false);
			m_hud.set_tooltip('B', false);
			m_hud.set_tooltip('Y', true);
		}
		else
		{
			m_hud.set_tooltip('R', false);
			m_hud.set_tooltip('G', false);
			m_hud.set_tooltip('B', false);
			m_hud.set_tooltip('Y', false);
		}
	}
}

bool World::is_char_detectable()
{
	return m_alert_mode || !(!m_char.is_moving() && (m_map.get_tile_type(m_char.get_position()) == m_char.get_color() + 1));
}

void World::advance_to_cutscene()
{
	switch (m_game_state)
	{
	case LEVEL_1:
		m_cutscene.set_dialogue_counter(LEVEL_2_CUTSCENE, 70);
		m_game_state = LEVEL_2_CUTSCENE;
		break;
	case LEVEL_2:
		m_cutscene.set_dialogue_counter(LEVEL_3_CUTSCENE, 81);
		m_game_state = LEVEL_3_CUTSCENE;
		break;
	case LEVEL_3:
		m_game_state = LEVEL_4;
		m_level = m_game_state;
		m_map.set_current_map(LEVEL_4);
		m_char.set_position(m_map.get_spawn_pos());
		break;
	case LEVEL_4:
		m_game_state = LEVEL_5;
		m_level = m_game_state;
		m_map.set_current_map(LEVEL_5);
		m_char.set_position(m_map.get_spawn_pos());
		break;
	case LEVEL_5:
		m_game_state = WIN_SCREEN;
		break;
	default:
		break;
	}
	Mix_VolumeMusic(60);
	Mix_FadeInMusic(m_background_music, -1, 5000);
}

void World::reset_game()
{
	m_char.destroy();
	m_char.init(m_map.get_spawn_pos(), m_map);
	m_char.reset_stealth();
	m_spotters.clear();
	m_wanderers.clear();
	m_shooters.clear();
	m_map.reset_char_dead_time();
	m_current_speed = 1.f;
	m_overlay.destroy();
	m_alert_mode = false;
	m_overlay.init(m_alert_mode, MAX_COOLDOWN);
	glfwSetTime(0);

	// reset direction for every spotter
	for (auto &spotter : m_spotters)
	{
		spotter.reset_direction();
	}
}
