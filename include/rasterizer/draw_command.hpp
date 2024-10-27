#pragma once

#include <rasterizer/matrix.hpp>
#include <rasterizer/settings.hpp>
#include <rasterizer/mesh.hpp>

namespace rasterizer
{

	struct draw_command
	{
		struct mesh mesh;
		enum cull_mode cull_mode = cull_mode::none;
		depth_settings depth = {};
		matrix4x4f transform = matrix4x4f::identity();
	};

}
