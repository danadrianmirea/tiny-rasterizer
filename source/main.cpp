#include <SDL2/SDL.h>
#include <chrono>
#include <iostream>
#include <vector>

#include <rasterizer/renderer.hpp>
#include <rasterizer/cube.hpp>
#include <rasterizer/image.hpp>

using namespace rasterizer;

struct terrain_vertex
{
	vector3f position;
	vector3f normal;
	vector4f color;
};

float terrain_height_at(float x, float z)
{
	return 5.f * (std::sin(0.1f * x) * std::cos(0.25f * z) + std::sin(0.04f * x) * std::cos(0.07f * z));
}

std::pair<std::vector<terrain_vertex>, std::vector<std::uint32_t>> generate_terrain(int size)
{
	std::vector<terrain_vertex> vertices;
	std::vector<std::uint32_t> indices;

	for (int z = 0; z <= size; ++z)
	{
		for (int x = 0; x <= size; ++x)
		{
			vector3f position{x - size * 0.5f, 0.f, z - size * 0.5f};

			position.y = terrain_height_at(position.x, position.z);

			vertices.push_back(terrain_vertex {
				.position = position,
				.normal = {0.f, 0.f, 0.f},
				.color = {0.5f, 0.5f, 0.5f, 1.f},
			});
		}
	}

	auto index = [size](int x, int z){ return z * (size + 1) + x; };

	for (int z = 0; z < size; ++z)
	{
		for (int x = 0; x < size; ++x)
		{
			indices.push_back(index(x, z));
			indices.push_back(index(x, z + 1));
			indices.push_back(index(x + 1, z));

			indices.push_back(index(x + 1, z));
			indices.push_back(index(x, z + 1));
			indices.push_back(index(x + 1, z + 1));
		}
	}

	for (int i = 0; i < indices.size(); i += 3)
	{
		auto & v0 = vertices[indices[i + 0]];
		auto & v1 = vertices[indices[i + 1]];
		auto & v2 = vertices[indices[i + 2]];

		auto n = cross(v1.position - v0.position, v2.position - v0.position);

		v0.normal = v0.normal + n;
		v1.normal = v1.normal + n;
		v2.normal = v2.normal + n;
	}

	for (auto & v : vertices)
		v.normal = normalized(v.normal);

	return {std::move(vertices), std::move(indices)};
}

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

	float lights_angle = 0.f;
	float view_angle = 0.f;

	auto [ terrain_vertices, terrain_indices ] = generate_terrain(64);

	mesh terrain_mesh
	{
		.positions = {terrain_vertices.data(), sizeof(terrain_vertex)},
		.normals = {(char const *)(terrain_vertices.data()) + 12, sizeof(terrain_vertex)},
		.colors = {(char const *)(terrain_vertices.data()) + 24, sizeof(terrain_vertex)},
		.indices = terrain_indices.data(),
		.count = terrain_indices.size(),
	};

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

		lights_angle += dt;
		view_angle += 0.1f * dt;

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

		matrix4x4f model = matrix4x4f::identity();

		matrix4x4f view = matrix4x4f::translate({0.f, 0.f, -50.f}) * matrix4x4f::rotateYZ(M_PIf / 4.f) * matrix4x4f::rotateZX(view_angle);

		matrix4x4f projection = matrix4x4f::perspective(0.1f, 100.f, M_PIf / 3.f, width * 1.f / height);

		directional_light sun
		{
			normalized(vector3f{2.f, 3.f, -1.f}),
			{1.f, 1.f, 1.f},
		};

		point_light point_lights[3]
		{
			{
				.position = {20.f * std::cos(lights_angle + 0.f * M_PIf / 3.f), 0.f, 20.f * std::sin(lights_angle + 0.f * M_PIf / 3.f)},
				.intensity = {1.f, 1.f, 0.f},
				.attenuation = {1.f, 0.f, 0.01f},
			},
			{
				.position = {20.f * std::cos(lights_angle + 2.f * M_PIf / 3.f), 0.f, 20.f * std::sin(lights_angle + 2.f * M_PIf / 3.f)},
				.intensity = {0.f, 1.f, 1.f},
				.attenuation = {1.f, 0.f, 0.01f},
			},
			{
				.position = {20.f * std::cos(lights_angle + 4.f * M_PIf / 3.f), 0.f, 20.f * std::sin(lights_angle + 4.f * M_PIf / 3.f)},
				.intensity = {1.f, 0.f, 1.f},
				.attenuation = {1.f, 0.f, 0.01f},
			},
		};

		for (auto & light : point_lights)
			light.position.y = 1.f + terrain_height_at(light.position.x, light.position.z);

		draw(framebuffer, viewport,
			draw_command {
				.mesh = terrain_mesh,
				.cull_mode = cull_mode::none,
				.depth = {
					.mode = depth_test_mode::less,
				},
				.model = model,
				.view = view,
				.projection = projection,

				.lights = light_settings {
					.ambient_light = {0.2f, 0.2f, 0.2f},
					.directional_lights = {&sun, 1},
					.point_lights = point_lights,
				},
			}
		);

		for (auto const & light : point_lights)
		{
			vector4f color = {light.intensity.x, light.intensity.y, light.intensity.z, 1.f};
			draw(framebuffer, viewport,
				draw_command {
					.mesh = {
						.positions = cube.positions,
						.normals = cube.normals,
						.colors = {&color, 0},
						.indices = cube.indices,
						.count = cube.count,
					},
					.model = matrix4x4f::translate(light.position) * matrix4x4f::scale(0.5f),
					.view = view,
					.projection = projection,
				}
			);
		}

		SDL_Rect rect{.x = 0, .y = 0, .w = width, .h = height};
		SDL_BlitSurface(draw_surface, &rect, SDL_GetWindowSurface(window), &rect);

		SDL_UpdateWindowSurface(window);
	}
}
