#pragma once

#include <rasterizer/matrix.hpp>
#include <rasterizer/settings.hpp>
#include <rasterizer/mesh.hpp>
#include <rasterizer/light.hpp>
#include <rasterizer/texture.hpp>
#include <rasterizer/sampler.hpp>

#include <optional>

namespace rasterizer
{

	struct texture_and_sampler
	{
		struct texture<color4ub> const * texture;
		struct sampler sampler;
	};

	struct draw_command
	{
		struct mesh mesh;
		enum cull_mode cull_mode = cull_mode::none;
		depth_settings depth = {};

		matrix4x4f model = matrix4x4f::identity();
		matrix4x4f view = matrix4x4f::identity();
		matrix4x4f projection = matrix4x4f::identity();

		std::optional<light_settings> lights = {};

		std::optional<texture_and_sampler> albedo = {};
	};

}
