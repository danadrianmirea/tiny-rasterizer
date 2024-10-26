#pragma once

#include <rasterizer/types.hpp>
#include <rasterizer/attribute.hpp>

namespace rasterizer
{

	struct mesh
	{
		attribute<vector3f> positions = {};
		attribute<vector4f> colors = {};
		std::uint32_t vertex_count = 0;
	};

}
