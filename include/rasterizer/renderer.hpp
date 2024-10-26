#pragma once

#include <rasterizer/types.hpp>
#include <rasterizer/image_view.hpp>

namespace rasterizer
{

	void clear(image_view const & color_buffer, vector4f const & color);

}
