#pragma once

// 3D transform object.
// Translation, scale, and rotation.

/**
 * @file
 * @author Mark Haddleton
 *
 * Transforms manage the position, orientation, and relative size of objects in 3D space - 
 * this file contains a structure and associated methods to handle and alter these values both independently and in relation to eachother.
 */

#include "mat4f.h"
#include "quatf.h"
#include "vec3f.h"

// Transform object.
/**
 * \brief transform_t stores spatial positioning data.
 * 
 * Specifically, linear position, scale, and rotation are stored as vectors and a quaternion.
 */
typedef struct transform_t
{
	vec3f_t translation; /**< translation is a 3D vector representing the x, y, and z position of the object in space. */
	vec3f_t scale; /**< scale is a 3D vector representing the linear scaling of the object's dimensions along the x, y, and z axes. */
	quatf_t rotation; /**< rotation is a quaternion that defines the rotation of the object in 3D space. A quaternion is used specifically to prevent cases in which multiple sets of values could result in the same effective rotation. See quatf_t for more info. */
} transform_t;

// Make a transform with no rotation, unit scale, and zero position.
/**
 * \brief Creates a transform_t instance with 0 rotation, (1,1,1) scale, and (0,0,0) position.
 * 
 * \param transform A pointer that will be assigned the identity transform.
 */
void transform_identity(transform_t* transform);

// Convert a transform to a matrix representation.
/** 
 * \brief Converts a provided transform_t to matrix form.
 * 
 * \param transform The transform_t to be converted.
 * \param output A pointer that will be assigned the matrix output.
 */
void transform_to_matrix(const transform_t* transform, mat4f_t* output);

// Combine two transforms -- result and t -- and store the output in result.
/**
 * \brief Combines two input transforms.
 * 
 * This is not as simple as adding the two together - while the scales and rotations can simply use vector and quaternion multiplication respectively, 
 * the translation is a little bit more complicated. First, one of the translations has to be multiplied by the other's scale and rotation, 
 * reflecting the difference in relative positioning. Once they are in the same relative scope, <i>then</i> they can be added together.
 * 
 * \param result The first transform provided, which will also be replaced with the output.
 * \param t The second transform provided.
 */
void transform_multiply(transform_t* result, const transform_t* t);

// Compute a transform's inverse in translation, scale, and rotation.
/**
 * \brief Inverts the provided transform.
 * 
 * Scale is simply a multiplicative inverse, Rotation uses the function quatf_conjugate(), and the translation is the additive inverse, adjusted for rotation, then multiplied by the new scale.
 * 
 * \param transform The transform to be inverted, which will be reassigned the new values.
 */
void transform_invert(transform_t* transform);

// Transform a vector by a transform object. Return the resulting vector.
/**
 * \brief Transforms the provided vector using the provided transform.
 * 
 * The function sequentially translates, rotates, and scales the vector using each aspect of the transform.
 * 
 * \param transform The transform whose contents will modify the provided vector.
 * \param v The vector to be modified.
 * \return The resulting vector.
 */
vec3f_t transform_transform_vec3(const transform_t* transform, vec3f_t v);
