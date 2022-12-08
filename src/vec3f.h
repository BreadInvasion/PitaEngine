#pragma once

#include <stdlib.h>

#include "math.h"

/**
 * @file
 * @author Mark Haddleton
 *
 * vec3f contains the basic structure and helper functions for a 3D vector.
 */

/**
 * \brief Stores three unique float values, referred to as x, y, and z.
 */
typedef struct vec3f_t
{
	union
	{
		struct { float x, y, z; };
		float a[3];
	};
} vec3f_t;

/**
 * \brief Returns the x unit vector, (1,0,0).
 * 
 * \return (1,0,0)
 */
__forceinline vec3f_t vec3f_x()
{
	return (vec3f_t) { .x = 1.0f };
}

/**
 * \brief Returns the y unit vector, (0,1,0).
 *
 * \return (0,1,0)
 */
__forceinline vec3f_t vec3f_y()
{
	return (vec3f_t) { .y = 1.0f };
}

/**
 * \brief Returns the z unit vector, (0,0,1).
 *
 * \return (0,0,1)
 */
__forceinline vec3f_t vec3f_z()
{
	return (vec3f_t) { .z = 1.0f };
}

/**
 * \brief Returns the one vector, (1,1,1).
 * 
 * It is important to note that this is NOT a unit vector.
 * 
 * \return (1,1,1)
 */
__forceinline vec3f_t vec3f_one()
{
	return (vec3f_t) { .x = 1.0f,.y = 1.0f, .z = 1.0f };
}

/**
 * \brief Returns the zero vector, (0,0,0).
 *
 * \return (0,0,0)
 */
__forceinline vec3f_t vec3f_zero()
{
	return (vec3f_t) { .x = 0.0f, .y = 0.0f, .z = 0.0f };
}

/**
 * \brief Returns a unit vector representing the "forward" direction, (-1,0,0).
 *
 * \return (-1,0,0)
 */
__forceinline vec3f_t vec3f_forward()
{
	return (vec3f_t) { .x = -1.0f };
}

/**
 * \brief Returns a unit vector representing the "right" direction, (0,1,0).
 * 
 * \sa vec3f_y()
 *
 * \return (0,1,0)
 */
__forceinline vec3f_t vec3f_right()
{
	return (vec3f_t) { .y = 1.0f };
}

/**
 * \brief Returns a unit vector representing the "up" direction, (0,0,1).
 * 
 * \sa vec3f_z()
 * 
 * \return (0,0,1)
 */
__forceinline vec3f_t vec3f_up()
{
	return (vec3f_t) { .z = 1.0f };
}

/** 
 * \brief Returns the additive inverse of the provided vector.
 * 
 * This is, of course, simply the vector created by finding the additive inverses of each axis and reassembling them in the same order.
 * 
 * \param v The vector to be negated.
 * \return The additive inverse of v.
 */
__forceinline vec3f_t vec3f_negate(vec3f_t v)
{
	return (vec3f_t)
	{
		.x = -v.x,
		.y = -v.y,
		.z = -v.z,
	};
}

/**
 * \brief Returns the sum of two vectors.
 * 
 * Vector addition is incredibly simple - just disassemble into components, 
 * add the matching components together, then reassemble the sums in the same order.
 * 
 * Note: Addition is commutative. The a and b parameters may be freely swapped, with no effect on the output.
 * 
 * \param a The first vector to be added.
 * \param b The second vector to be added.
 * \return The sum of a and b.
 */
__forceinline vec3f_t vec3f_add(vec3f_t a, vec3f_t b)
{
	return (vec3f_t)
	{
		.x = a.x + b.x,
		.y = a.y + b.y,
		.z = a.z + b.z
	};
}

/**
 * \brief Returns the difference between two vectors.
 * 
 * Vector addition is incredibly simple - just disassemble into components, 
 * subtract the matching components, then reassemble the differences in the same order.
 * 
 * Note: Subtraction is NOT commutative. In this implementation, subtraction is conducted as r = (a - b).
 * For all inputs a != b, r will change depending on the order of inputs.
 * 
 * \param a The first vector, from which b will be subtracted.
 * \param b The second vector, to be subtracted from a.
 * \return The difference between a and b.
 */
__forceinline vec3f_t vec3f_sub(vec3f_t a, vec3f_t b)
{
	return (vec3f_t)
	{
		.x = a.x - b.x,
		.y = a.y - b.y,
		.z = a.z - b.z
	};
}

/**
 * \brief Returns the multiplicative product of two vectors.
 * 
 * It is important to clarify that this is the <i>multiplicative</i> product of a and b, 
 * as the dot product and the cross product are both distinct products that could be found relating to three-dimensional vectors. 
 * This particular product is very simple: Just disassemble into components, 
 * multiply the matching components, then reassemble the products in the same order.
 * 
 * Note: Multiplication is commutative. Swapping the order of a and b will not change the result.
 * 
 * \param a The first vector to be multiplied.
 * \param b The second vector to be multiplied.
 * \return The multiplicative product of a and b.
 */
__forceinline vec3f_t vec3f_mul(vec3f_t a, vec3f_t b)
{
	return (vec3f_t)
	{
		.x = a.x * b.x,
		.y = a.y * b.y,
		.z = a.z * b.z
	};
}

/**
 * \brief Returns the minimum values along all axes of a and b.
 * 
 * The resultant vector contains all of the smallest values when comparing each matching component - 
 * notably, what is found are not the closest values to 0 of that axis, but rather the <i>most negative</i> values.
 * 
 * \param a The first vector to be compared.
 * \param b The second vector to be compared.
 * \return The minimum component vector of a and b.
 */
__forceinline vec3f_t vec3f_min(vec3f_t a, vec3f_t b)
{
	return (vec3f_t)
	{
		.x = __min(a.x, b.x),
		.y = __min(a.y, b.y),
		.z = __min(a.z, b.z),
	};
}

/**
 * \brief Returns the minimum values along all axes of a and b.
 *
 * The resultant vector contains all of the smallest values when comparing each matching component -
 * notably, what is found are not the closest values to 0 of that axis, but rather the <i>most negative</i> values.
 *
 * \return The minimum component vector of a and b.
 */
__forceinline vec3f_t vec3f_max(vec3f_t a, vec3f_t b)
{
	return (vec3f_t)
	{
		.x = __max(a.x, b.x),
		.y = __max(a.y, b.y),
		.z = __max(a.z, b.z),
	};
}

/**
 * \brief Scales vector v by constant factor f.
 * 
 * Multiply the individual components of v by the constant factor, then reassemble them in the same order.
 * 
 * \param v The vector to be scaled.
 * \param f The constant factor by which to scale v.
 * \return The scaled vector.
 */
__forceinline vec3f_t vec3f_scale(vec3f_t v, float f)
{
	return (vec3f_t)
	{
		.x = v.x * f,
		.y = v.y * f,
		.z = v.z * f,
	};
}

/**
 * \brief Returns the vector f% along the linear interpolation between a and b.
 * 
 * Since we're working linearly, the individual matching components can be interpolated by the same constant factor.
 * 
 * Note: While the intended behavior of this function expects 0 <= f <= 1, an f value outside of this range will not produce an error - 
 * rather, it will extend the imaginary line that exists between a and b in 3D space, extending the interpolation out beyond the end, 
 * where 1 unit is equal to the distance between a and b.
 * 
 * \param a The "beginning vector" in the interpolation. This is what will be returned by f = 0.
 * \param b The "end vector" in the interpolation. This is what will be returned by f = 1.
 * \return The interpolated vector.
 */
__forceinline vec3f_t vec3f_lerp(vec3f_t a, vec3f_t b, float f)
{
	return (vec3f_t)
	{
		.x = lerpf(a.x, b.x, f),
		.y = lerpf(a.y, b.y, f),
		.z = lerpf(a.z, b.z, f),
	};
}

/**
 * \brief Returns the square of the magnitude of vector v.
 * 
 * \sa vec3_dot(v,v), pow(vec3f_mag(v),2)
 * 
 * \param v The vector to find the square magnitude of.
 * \return The square magnitude of v.
 */
__forceinline float vec3f_mag2(vec3f_t v)
{
	return (v.x * v.x) + (v.y * v.y) + (v.z * v.z);
}

/**
 * \brief Returns the magnitude of vector v.
 * 
 * Magnitude is defined as the square root of the sum of every component squared (quite the mouthful). 
 * In this case our work is simplified by helper function vec3f_mag2(v).
 * 
 * \param v The vector to find the magnitude of.
 * \return The magnitude of vector v.
 */
__forceinline float vec3f_mag(vec3f_t v)
{
	return sqrtf(vec3f_mag2(v));
}

/**
 * \brief Returns the square of the linear distance between vectors a and b.
 * 
 * This value is simply the square magnitude of (a-b) using vector math.
 * 
 * Note: Because the value is squared, order doesn't matter - a and b can be safely swapped.
 * 
 * \param a The first vector to be compared.
 * \param b The second vector to be compared.
 * \return The square of the linear distance between a and b.
 */
__forceinline float vec3f_dist2(vec3f_t a, vec3f_t b)
{
	vec3f_t diff = vec3f_sub(a, b);
	return vec3f_mag2(diff);
}

/** 
 * \brief Returns the linear distance between a and b.
 * 
 * This value is equal to the magnitude of (a-b) using vector math.
 * 
 * Note: As a result of the squaring that occurs in helper function vec3f_dist2(a,b), 
 * order doesn't matter - a and b can be safely swapped.
 * 
 * \param a The first vector to be compared.
 * \param b The second vector to be compared.
 * \return The linear distance between a and b.
 */
__forceinline float vec3f_dist(vec3f_t a, vec3f_t b)
{
	return sqrtf(vec3f_dist2(a, b));
}

/**
 * \brief Returns a normalized copy of vector v.
 * 
 * The "normal form" of a vector points in the same direction, but has a magnitude of 1. 
 * This can be achieved by scaling the vector by the inverse of its magnitude.
 * 
 * \param v The vector to be normalized.
 * \return The normal form of vector v.
 */
__forceinline vec3f_t vec3f_norm(vec3f_t v)
{
	float m = vec3f_mag(v);
	if (almost_equalf(m, 0.0f))
	{
		return v;
	}
	return vec3f_scale(v, 1.0f / m);
}

/**
 * \brief Returns the dot product of vector v.
 * 
 * Find the multiplicative product of the two vectors, then sum the result's components. This value is useful in vector calculus.
 * 
 * \param a The first vector.
 * \param b The second vector.
 * \return The dot product of a and b.
 */
__forceinline float vec3f_dot(vec3f_t a, vec3f_t b)
{
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

/**
 * \brief Returns the reflection of vector v about n.
 * 
 * There's some complicated vector math here - simply put, 
 * the dot product is used to scale r by a constant factor 
 * that will result in the output vector having the same magnitude as v, 
 * then the subtraction is performed to find the reflected vector.
 * 
 * \param v The vector to be reflected
 * \param n A vector representing the axis across which v will be reflected.
 * \return The reflection of v across n.
 */
__forceinline vec3f_t vec3f_reflect(vec3f_t v, vec3f_t n)
{
	float d = vec3f_dot(v, n);
	vec3f_t r = vec3f_scale(n, d * 2.0f);
	return vec3f_sub(v, r);
}

/** 
 * \brief Returns the cross product of a and b.
 * 
 * The cross product of two vectors is a vector that is orthogonal to the plane created by the two - that is, it is perpendicular to both a and b. 
 * This is relevant in a variety of operations relating to vectors and planes - 
 * it is important to note, though, that the direction of the vector created by this process <i>is</i> predictable (since its inverse is also orthogonal to the plane), using the right hand rule.
 * 
 * \param a The first vector in the cross product.
 * \param b The second vector in the cross product.
 * \return The cross product of a and b.
 */
__forceinline vec3f_t vec3f_cross(vec3f_t a, vec3f_t b)
{
	return (vec3f_t)
	{
		.x = a.y * b.z - a.z * b.y,
		.y = a.z * b.x - a.x * b.z,
		.z = a.x * b.y - a.y * b.x,
	};
}
