#pragma once

#include <rasterizer/image_view.hpp>

#include <cstdint>
#include <memory>

namespace rasterizer
{

	template <typename Pixel>
	struct image
	{
		std::unique_ptr<Pixel[]> pixels;
		std::uint32_t width = 0;
		std::uint32_t height = 0;

		explicit operator bool() const
		{
			return pixels != nullptr;
		}

		image_view<Pixel> view()
		{
			return image_view<Pixel>
			{
				.pixels = pixels.get(),
				.width = width,
				.height = height,
			};
		}

		static image allocate(std::uint32_t width, std::uint32_t height)
		{
			return image
			{
				.pixels = std::unique_ptr<Pixel[]>(new Pixel[width * height]),
				.width = width,
				.height = height,
			};
		}
	};

}
