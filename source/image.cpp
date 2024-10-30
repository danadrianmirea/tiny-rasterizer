#include <rasterizer/image.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <rasterizer/stb_image.h>

namespace rasterizer
{

	image<color4ub> load_image(std::filesystem::path const & path)
	{
		image<color4ub> result;
		int channels;
		result.pixels.reset((color4ub *)stbi_load(path.c_str(), (int *)&result.width, (int *)&result.height, &channels, 4));
		return result;
	}

}
