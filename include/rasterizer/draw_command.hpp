#pragma once

#include <rasterizer/types.hpp>
#include <rasterizer/settings.hpp>
#include <rasterizer/mesh.hpp>

namespace rasterizer
{

	struct draw_command
	{
		struct mesh mesh;
		enum cull_mode cull_mode = cull_mode::none;
		matrix4x4f transform = matrix4x4f::identity();
	};

}
