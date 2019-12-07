// internal
#include "common.hpp"
#include "world.hpp"

#define GL3W_IMPLEMENTATION
#include <gl3w.h>

// stdlib
#include <chrono>
#include <iostream>

using Clock = std::chrono::high_resolution_clock;

// global 
World world;

const float TICK = 40.f;

// entry point
int main(int argc, char* argv[])
{
	// initializing world (after renderer.init().. sorry)
	if (!world.init())
	{
		// time to read the error message
		std::cout << "Press any key to exit" << std::endl;
		std::cin.get();
		return EXIT_FAILURE;
	}

	auto t = Clock::now();

	float avg_ms = 0.f;
	float dt = 0.f;

	// variable timestep loop
	while (!world.is_over())
	{
		// processes system messages, if this wasn't present the window would become unresponsive
		glfwPollEvents();

		// calculating elapsed times in milliseconds from the previous iteration
		auto now = Clock::now();
		float ms = (float)(std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count() / 1000;

		avg_ms = (avg_ms + ms) / 2;
		//fprintf(stderr, "cur:%f, avg:%f\n", ms, avg_ms);

		if (ms < TICK)
		{
			#if __APPLE__
				usleep(TICK - ms);
			#else
   			Sleep(TICK - ms);
			#endif
		}

		t = now;

		world.update(ms - dt);
		world.draw();
		
		dt = 0.f;
	}

	world.destroy();

	return EXIT_SUCCESS;
}