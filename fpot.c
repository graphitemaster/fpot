#include <math.h>
#include <string.h>
#include <stdlib.h>

#include "fpot.h"

/* Grid constants, these can be changed if you need a larger grid, though
 * these are suitable for nearly all amounts of vertex data. A smaller
 * grid will perform worse on large data sets, and a larger grid will
 * perform worse on small data sets. */
#define k_grid_shift 4
#define k_grid_max ((1 << k_grid_shift) - 1)
#define k_grid_size (1 << (3 * k_grid_shift))

/* Diliation offsets */
static const int k_dilate_offsets[3][27] = {
	{
		-1, -1, -1, -1, -1, -1, -1, -1, -1,
		 0,  0,  0,  0,  0,  0,  0,  0,  0,
		 1,  1,  1,  1,  1,  1,  1,  1,  1
	}, {
		-1, -1, -1,  0,  0,  0,  1,  1,  1,
		-1, -1, -1,  0,  0,  0,  1,  1,  1,
		-1, -1, -1,  0,  0,  0,  1,  1,  1
	}, {
		-1,  0,  1, -1,  0,  1, -1,  0,  1,
		-1,  0,  1, -1,  0,  1, -1,  0,  1,
		-1,  0,  1, -1,  0,  1, -1,  0,  1
	}
};

struct fpot {
	float _min[3];
	float _max[3];
	float _scale;
	char *_grid;
};

static inline int selmax(int _a1, int _a2) {
	return _a1 > _a2 ? _a1 : _a2;
}

static inline int selmin(int _a1, int _a2) {
	return _a1 < _a2 ? _a1 : _a2;
}

static inline int clamp(int _value, int _min, int _max) {
	return selmax(_min, selmin(_max, _value));
}

static inline bool valid(const fpot *const _fpot, const float *const _point) {
	return _point[0] >= _fpot->_min[0] && _point[1] >= _fpot->_min[1] && _point[2] >= _fpot->_min[2] &&
	       _point[0] <= _fpot->_max[0] && _point[1] <= _fpot->_max[1] && _point[2] <= _fpot->_max[2];
}

static inline int ind_cell(const int _x, const int _y, const int _z) {
	const int x = clamp(_x, 0, k_grid_max);
	const int y = clamp(_y, 0, k_grid_max);
	const int z = clamp(_z, 0, k_grid_max);
	return (x << (2 * k_grid_shift)) + (y << k_grid_shift) + z;
}

static inline int ind_point(const fpot *const _fpot, const float *const _point) {
	return ind_cell((int)(_fpot->_scale * (_point[0] - _fpot->_min[0])),
	                (int)(_fpot->_scale * (_point[1] - _fpot->_min[1])),
	                (int)(_fpot->_scale * (_point[2] - _fpot->_min[2])));
}

bool fpot_create(fpot **data_, const float *const _points, size_t _n_points) {
	if (!data_) {
		return false;
	}

	/* layout of memory: [fpot, grid, grid], first grid is data->_grid,
	 * the second grid, which is data->_grid + k_grid_size is used to
	 * dilate the data */
	fpot *data = malloc(sizeof *data + 2 * k_grid_size);
	if (!data) {
		return false;
	}

	data->_grid = (char *)data + 1;
	if (_n_points == 0) {
		memset(data->_min, 0, sizeof data->_min);
		memset(data->_max, 0, sizeof data->_max);
		data->_scale = 0.0f;
		return true;
	}

	/* Calculate bounding box of points */
	data->_min[0] = data->_max[0] = _points[0];
	data->_min[1] = data->_max[1] = _points[1];
	data->_min[2] = data->_max[2] = _points[2];
	for (size_t i = 1; i < _n_points; i++) {
		for (size_t j = 0; j < 3; j++) {
			if (_points[i * 3 + j] < data->_min[j]) {
				data->_min[j] = _points[i * 3 + j];
			} else if (_points[i * 3 + j] > data->_max[j]) {
				data->_max[j] = _points[i * 3 + j];
			}
		}
	}
	data->_scale = 1.0f / selmax(selmax(data->_max[0] - data->_min[0], data->_max[1] - data->_min[1]), data->_max[2] - data->_min[2]);
	data->_scale *= (float)(1 << k_grid_shift);

	/* dilate */
	char *grid = data->_grid + k_grid_size;
	for (size_t i = 0; i < _n_points; i++) {
		grid[ind_point(data, _points + 3 * i)] = 1;
	}

	for (size_t i = 0; i < k_grid_size; i++) {
		const int x = i >> (2 * k_grid_shift);
		const int y = (i >> k_grid_shift) & k_grid_max;
		const int z = i & k_grid_max;
		for (int j = 0; j < 27; j++) {
			if (grid[ind_cell(x + k_dilate_offsets[0][j],
			                  y + k_dilate_offsets[1][j],
			                  z + k_dilate_offsets[2][j])])
			{
				data->_grid[i] = 1;
				break;
			}
		}
	}

	*data_ = data;
	return true;
}

void fpot_destroy(fpot *_data) {
	free(_data);
}

bool fpot_overlaps(const fpot *const _fpot, const float *const _point) {
	return _fpot && valid(_fpot, _point) && _fpot->_grid[ind_point(_fpot, _point)];
}
