#pragma once

#include <cstdint>

namespace rasterizer
{

	struct color4ub
	{
		std::uint8_t r, g, b, a;
	};

	struct vector3f
	{
		float x, y, z;
	};

	struct vector4f
	{
		float x, y, z, w;
	};

	struct matrix4x4f
	{
		float values[16];

		static matrix4x4f identity()
		{
			return matrix4x4f{
				1.f, 0.f, 0.f, 0.f,
				0.f, 1.f, 0.f, 0.f,
				0.f, 0.f, 1.f, 0.f,
				0.f, 0.f, 0.f, 1.f,
			};
		}
	};

	inline vector4f operator - (vector4f const & v0, vector4f const & v1)
	{
		return {v0.x - v1.x, v0.y - v1.y, v0.z - v1.z, v0.w - v1.w};
	}

	inline float det2D(vector4f const & v0, vector4f const & v1)
	{
		return v0.x * v1.y - v0.y * v1.x;
	}

	inline vector4f operator * (matrix4x4f const & m, vector4f const & v)
	{
		vector4f result{0.f, 0.f, 0.f, 0.f};

		result.x = m.values[ 0] * v.x + m.values[ 1] * v.y + m.values[ 2] * v.z + m.values[ 3] * v.w;
		result.y = m.values[ 4] * v.x + m.values[ 5] * v.y + m.values[ 6] * v.z + m.values[ 7] * v.w;
		result.z = m.values[ 8] * v.x + m.values[ 9] * v.y + m.values[10] * v.z + m.values[11] * v.w;
		result.w = m.values[12] * v.x + m.values[13] * v.y + m.values[14] * v.z + m.values[15] * v.w;

		return result;
	}

	inline vector4f as_vector(vector3f const & v)
	{
		return {v.x, v.y, v.z, 0.f};
	}

	inline vector4f as_point(vector3f const & v)
	{
		return {v.x, v.y, v.z, 1.f};
	}

	// Define custom min/max so that we don't have to include the full <algorithm> header

	template<typename T>
	T max(T a, T b)
	{
		return (a > b) ? a : b;
	}

	template<typename T>
	T min(T a, T b)
	{
		return (a > b) ? b : a;
	}

	inline color4ub to_color4ub(vector4f const & c)
	{
		color4ub result;

		result.r = max(0.f, min(255.f, c.x * 255.f));
		result.g = max(0.f, min(255.f, c.y * 255.f));
		result.b = max(0.f, min(255.f, c.z * 255.f));
		result.a = max(0.f, min(255.f, c.w * 255.f));

		return result;
	}

}
