#include <SDL2/SDL.h>
#include <chrono>
#include <iostream>
#include <vector>
#include <unordered_set>

#include <rasterizer/renderer.hpp>
#include <rasterizer/cube.hpp>
#include <rasterizer/image.hpp>
#include <rasterizer/texture.hpp>

using namespace rasterizer;

int main()
{
	SDL_Init(SDL_INIT_VIDEO);

	int width = 800;
	int height = 600;

	SDL_Window * window = SDL_CreateWindow("Tiny rasterizer", 0*SDL_WINDOWPOS_UNDEFINED, 0*SDL_WINDOWPOS_UNDEFINED, width, height, 0*SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN);

	SDL_Surface * draw_surface = nullptr;

	int mouse_x = 0;
	int mouse_y = 0;

	std::filesystem::path project_root = PROJECT_ROOT;

	using namespace rasterizer;

	image<std::uint32_t> depth_buffer;

	texture<color4ub> brick_texture;
	brick_texture.mipmaps.push_back(load_image(project_root / "assets" / "brick_1024.jpg"));
	generate_mipmaps(brick_texture);

	float cube_angle = 0.f;
	float cube_distance = 5.f;

	std::unordered_set<SDL_Keycode> keydown;

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
		case SDL_KEYDOWN:
			keydown.insert(event.key.keysym.sym);
			break;
		case SDL_KEYUP:
			keydown.erase(event.key.keysym.sym);
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

		if (keydown.contains(SDLK_LEFT))
			cube_angle -= 2.f * dt;
		if (keydown.contains(SDLK_RIGHT))
			cube_angle += 2.f * dt;

		if (keydown.contains(SDLK_UP))
			cube_distance += 4.f * dt;
		if (keydown.contains(SDLK_DOWN))
			cube_distance -= 4.f * dt;

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

		matrix4x4f model = matrix4x4f::rotateZX(cube_angle);

		matrix4x4f view = matrix4x4f::translate({0.f, 0.f, -cube_distance}) * matrix4x4f::rotateYZ(M_PIf / 12.f);

		matrix4x4f projection = matrix4x4f::perspective(0.1f, 100.f, M_PIf / 3.f, width * 1.f / height);

		directional_light sun
		{
			normalized(vector3f{1.f, 1.f, 1.f}),
			{1.f, 1.f, 1.f},
		};

		vector4f color{1.f, 1.f, 1.f, 1.f};

		draw(framebuffer, viewport,
			draw_command {
				.mesh = {
					.positions = cube.positions,
					.normals = cube.normals,
					.colors = {&color, 0},
					.texcoords = cube.texcoords,
					.indices = cube.indices,
					.count = cube.count,
				},
				.cull_mode = cull_mode::cw,
				.depth = {
					.write = true,
					.mode = depth_test_mode::less,
				},
				.model = model,
				.view = view,
				.projection = projection,
				.lights = light_settings {
					.ambient_light = {0.2f, 0.2f, 0.2f},
					.directional_lights = {&sun, 1},
				},
				.albedo = texture_and_sampler {
					.texture = &brick_texture,
					.sampler = {
						.mag_filter = filtering::linear,
						.min_filter = filtering::linear,
					},
				}
			}
		);

		SDL_Rect rect{.x = 0, .y = 0, .w = width, .h = height};
		SDL_BlitSurface(draw_surface, &rect, SDL_GetWindowSurface(window), &rect);

		SDL_UpdateWindowSurface(window);
	}
}
