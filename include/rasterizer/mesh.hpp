#pragma once

#include <rasterizer/vector.hpp>
#include <rasterizer/attribute.hpp>

namespace rasterizer
{

	struct mesh
	{
		attribute<vector3f> positions = {};
		attribute<vector3f> normals = {};
		attribute<vector4f> colors = {};
		std::uint32_t const * indices = nullptr;
		std::uint32_t count = 0;
	};

}
