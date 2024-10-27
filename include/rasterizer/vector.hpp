#pragma once

namespace rasterizer
{

	struct vector3f
	{
		float x, y, z;
	};

	struct vector4f
	{
		float x, y, z, w;
	};

	inline vector4f operator * (float s, vector4f const & v)
	{
		return {s * v.x, s * v.y, s * v.z, s * v.w};
	}

	inline vector4f operator - (vector4f const & v0, vector4f const & v1)
	{
		return {v0.x - v1.x, v0.y - v1.y, v0.z - v1.z, v0.w - v1.w};
	}

	inline vector4f operator + (vector4f const & v0, vector4f const & v1)
	{
		return {v0.x + v1.x, v0.y + v1.y, v0.z + v1.z, v0.w + v1.w};
	}

	inline float det2D(vector4f const & v0, vector4f const & v1)
	{
		return v0.x * v1.y - v0.y * v1.x;
	}

	inline float dot(vector4f const & v0, vector4f const & v1)
	{
		return v0.x * v1.x + v0.y * v1.y + v0.z * v1.z + v0.w * v1.w;
	}

	inline vector4f as_vector(vector3f const & v)
	{
		return {v.x, v.y, v.z, 0.f};
	}

	inline vector4f as_point(vector3f const & v)
	{
		return {v.x, v.y, v.z, 1.f};
	}

	inline vector4f perspective_divide(vector4f v)
	{
		v.w = 1.f / v.w;
		v.x *= v.w;
		v.y *= v.w;
		v.z *= v.w;
		return v;
	}

}
