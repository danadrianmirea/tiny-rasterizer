#include <rasterizer/renderer.hpp>

#include <algorithm>
#include <cmath>

namespace rasterizer
{

	void clear(image_view const & color_buffer, vector4f const & color)
	{
		auto ptr = color_buffer.pixels;
		auto size = color_buffer.width * color_buffer.height;
		std::fill(ptr, ptr + size, to_color4ub(color));
	}

	void draw(image_view const & color_buffer, viewport const & viewport, draw_command const & command)
	{
		for (std::uint32_t vertex_index = 0; vertex_index + 2 < command.mesh.count; vertex_index += 3)
		{
			std::uint32_t i0 = vertex_index + 0;
			std::uint32_t i1 = vertex_index + 1;
			std::uint32_t i2 = vertex_index + 2;

			if (command.mesh.indices)
			{
				i0 = command.mesh.indices[i0];
				i1 = command.mesh.indices[i1];
				i2 = command.mesh.indices[i2];
			}

			auto v0 = command.transform * as_point(command.mesh.positions[i0]);
			auto v1 = command.transform * as_point(command.mesh.positions[i1]);
			auto v2 = command.transform * as_point(command.mesh.positions[i2]);

			v0 = apply(viewport, perspective_divide(v0));
			v1 = apply(viewport, perspective_divide(v1));
			v2 = apply(viewport, perspective_divide(v2));

			auto c0 = command.mesh.colors[i0];
			auto c1 = command.mesh.colors[i1];
			auto c2 = command.mesh.colors[i2];

			float det012 = det2D(v1 - v0, v2 - v0);

			bool const ccw = det012 < 0.f;

			switch (command.cull_mode)
			{
			case cull_mode::none:
				if (ccw)
				{
					std::swap(v1, v2);
					std::swap(c1, c2);
					det012 = -det012;
				}
				break;
			case cull_mode::cw:
				if (!ccw)
					continue;
				std::swap(v1, v2);
				std::swap(c1, c2);
				det012 = -det012;
				break;
			case cull_mode::ccw:
				if (ccw)
					continue;
				break;
			}

			std::int32_t xmin = std::max<std::int32_t>(viewport.xmin, 0);
			std::int32_t xmax = std::min<std::int32_t>(viewport.xmax, color_buffer.width) - 1;
			std::int32_t ymin = std::max<std::int32_t>(viewport.ymin, 0);
			std::int32_t ymax = std::min<std::int32_t>(viewport.ymax, color_buffer.height) - 1;

			xmin = std::max<float>(xmin, std::min({std::floor(v0.x), std::floor(v1.x), std::floor(v2.x)}));
			xmax = std::min<float>(xmax, std::max({std::floor(v0.x), std::floor(v1.x), std::floor(v2.x)}));
			ymin = std::max<float>(ymin, std::min({std::floor(v0.y), std::floor(v1.y), std::floor(v2.y)}));
			ymax = std::min<float>(ymax, std::max({std::floor(v0.y), std::floor(v1.y), std::floor(v2.y)}));

			for (std::int32_t y = ymin; y <= ymax; ++y)
			{
				for (std::int32_t x = xmin; x <= xmax; ++x)
				{
					vector4f p{x + 0.5f, y + 0.5f, 0.f, 0.f};

					float det01p = det2D(v1 - v0, p - v0);
					float det12p = det2D(v2 - v1, p - v1);
					float det20p = det2D(v0 - v2, p - v2);

					if (det01p >= 0.f && det12p >= 0.f && det20p >= 0.f)
					{
						float l0 = det12p / det012;
						float l1 = det20p / det012;
						float l2 = det01p / det012;

						color_buffer.at(x, y) = to_color4ub(l0 * c0 + l1 * c1 + l2 * c2);
					}
				}
			}
		}
	}

}
