#include <SDL2/SDL.h>
#include <chrono>
#include <iostream>

#include <rasterizer/renderer.hpp>
#include <rasterizer/cube.hpp>
#include <rasterizer/image.hpp>

int main()
{
	SDL_Init(SDL_INIT_VIDEO);

	int width = 800;
	int height = 600;

	SDL_Window * window = SDL_CreateWindow("Tiny rasterizer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);

	SDL_Surface * draw_surface = nullptr;

	int mouse_x = 0;
	int mouse_y = 0;

	using namespace rasterizer;

	image<std::uint32_t> depth_buffer;

	float cube_angle = 0.f;
	float cube_scale = 1.f;

	using clock = std::chrono::high_resolution_clock;

	auto last_frame_start = clock::now();

	bool running = true;
	while (running)
	{
		for (SDL_Event event; SDL_PollEvent(&event);) switch (event.type)
		{
		case SDL_WINDOWEVENT:
			switch (event.window.event)
			{
			case SDL_WINDOWEVENT_RESIZED:
				if (draw_surface)
					SDL_FreeSurface(draw_surface);
				draw_surface = nullptr;
				width = event.window.data1;
				height = event.window.data2;
				depth_buffer = {};
				break;
			}
			break;
		case SDL_QUIT:
			running = false;
			break;
		case SDL_MOUSEMOTION:
			mouse_x = event.motion.x;
			mouse_y = event.motion.y;
			break;
		}

		if (!running)
			break;

		if (!draw_surface)
		{
			draw_surface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, SDL_PIXELFORMAT_RGBA32);
			SDL_SetSurfaceBlendMode(draw_surface, SDL_BLENDMODE_NONE);
		}

		if (!depth_buffer)
			depth_buffer = image<std::uint32_t>::allocate(width, height);

		auto now = clock::now();
		float dt = std::chrono::duration_cast<std::chrono::duration<float>>(now - last_frame_start).count();
		last_frame_start = now;

		std::cout << dt << std::endl;

		cube_angle += dt;

		using namespace rasterizer;

		framebuffer framebuffer
		{
			.color = {
				.pixels = (color4ub *)draw_surface->pixels,
				.width = (std::uint32_t)width,
				.height = (std::uint32_t)height,
			},
			.depth = depth_buffer.view(),
		};

		viewport viewport
		{
			.xmin = 0,
			.ymin = 0,
			.xmax = (std::int32_t)width,
			.ymax = (std::int32_t)height,
		};

		clear(framebuffer.color, {0.9f, 0.9f, 0.9f, 1.f});
		clear(framebuffer.depth, -1);

		matrix4x4f model = matrix4x4f::scale(cube_scale) * matrix4x4f::rotateZX(cube_angle) * matrix4x4f::rotateXY(cube_angle * 1.61f);

		matrix4x4f view = matrix4x4f::translate({0.f, 0.f, -5.f});

		matrix4x4f projection = matrix4x4f::perspective(3.0f, 10.f, M_PI / 3.f, width * 1.f / height);

		for (int i = -2; i <= 2; ++i)
			draw(framebuffer, viewport,
				draw_command{
					.mesh = cube,
					.cull_mode = cull_mode::none,
					.depth = {
						.mode = depth_test_mode::less,
					},
					.transform = projection * view * matrix4x4f::translate({i, 0.f, 0.f}) * model,
				}
			);

		SDL_Rect rect{.x = 0, .y = 0, .w = width, .h = height};
		SDL_BlitSurface(draw_surface, &rect, SDL_GetWindowSurface(window), &rect);

		SDL_UpdateWindowSurface(window);
	}
}
