# Fast Point Overlap Test

The following is a minimal, optimized point overlap testing library
that uses a simple spatial grid data structure to accelerate point
tests. It's optimized for 3D points only and uses a fixed size grid that
offers a balanced performance to size ratio for a variety of data sets.

# Usage
```c
#include "fpot.h"
// points are passed as const float*, and expects each point has three
// components: x, y, z.
fpot *obj = NULL;

// creates the fast point overlap test object
// note: number_of_points is the number of tuples (x, y, z) and not
// the number of scalars making up the points.
if (!fpot_create(&obj, points, number_of_points)) {
	printf("could not create fpot (&obj is invalid or out of memory)\n");
}

// check if a point overlaps
if (fpot_overlaps(obj, point)) {
	printf("point: (%f, %f, %f) overlaps!\n", point[0], point[1], point[2]);
}

// be sure to destroy it after:
fpot_destroy(obj);
```
