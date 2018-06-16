#ifndef FPOT_H
#define FPOT_H

#if defined(__cplusplus)
extern "C" {
#endif

#include <stdbool.h>

/** @brief Opaque type representing a fast point overlap test object */
typedef struct fpot fpot;

/**
 * @brief Create a fast point overlap test object
 * @param data_ The opaque object is stored here
 * @param _points Points stored as [{x, y, z}, {x, y, z}, ...]
 * @param _n_points_ The number of points
 * @return true when successfully created, false otherwise
 */
bool fpot_create(fpot **data_, const float *const _points, size_t _n_points);

/**
 * @brief Destroy as fast point overlap test object
 * @param _data The opaque object to destroy
 * @note It's safe to pass NULL
 */
void fpot_destroy(fpot *_data);

/**
 * @brief Test if a point overlaps something
 * @param _fpot The opaque fast point overlap test object
 * @param _point The point specified as {x, y, z}
 * @param true when point overlaps, false otherwise
 * @note It's safe to pass NULL
 */
bool fpot_overlaps(const fpot *const _fpot, const float *const _point);

#if defined(__cplusplus)
}
#endif

#endif
