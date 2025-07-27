#ifndef CAMERA_H
#define CAMERA_H

#include "graphics_math.h"
#include <cmath>

struct Camera {
	float x = 0;
	float y = 0;
	float z = 0;

	// rotation[0] is around the x-axis, rotation[1] is around the y-axis, and rotation[2] is around the z-axis.
	float rotation [3] {0, 0, 0};

	float z_near;
	float z_far;
	float field_of_view;

	Camera(float z_near, float z_far, float fov) : z_near(z_near), z_far(z_far) {
		set_fov_deg(fov);
	}

	void set_rotation_deg(float x, float y, float z) {
		rotation[0] = M_PI / 180.0 * x;
		rotation[1] = M_PI / 180.0 * y;
		rotation[2] = M_PI / 180.0 * z;
	}

	void set_fov_deg(float fov) {
		field_of_view = M_PI / 180.0 * fov;
	}

	gmath::matrix get_look_direction() {
		return gmath::matrix({{0, 0, 1}}) * (gmath::matrix({
			{ 1, 0, 0},
			{ 0, cos(rotation[0]), -sin(rotation[0])},
			{ 0, sin(rotation[0]), cos(rotation[0])},
		}) * gmath::matrix({
			{ cos(rotation[1]), 0, sin(rotation[1])},
			{ 0, 1, 0},
			{ -sin(rotation[1]), 0, cos(rotation[1])},
		}) * gmath::matrix({
			{ cos(rotation[2]), -sin(rotation[2]), 0},
			{ sin(rotation[2]), cos(rotation[2]), 0},
			{ 0, 0, 1},
		}));
	}

	gmath::matrix get_forward() {
		const float x = rotation[0];
		const float y = rotation[1];
		const float z = rotation[2];
		return gmath::matrix {
			{
				(sin(x) * sin(y) * cos(z)) + (cos(x) * cos(y)),
				(sin(x) * sin(y) * sin(z)) - (cos(x) * cos(z)),
				sin(x) * cos(y)
			}
		}.normalized();
	}

	gmath::matrix get_up() {
		const float x = rotation[0];
		const float y = rotation[1];
		const float z = rotation[2];
		return gmath::matrix {
			{
				-sin(x) * cos(y) * cos(z),
				-sin(x) * cos(y) * sin(z),
				cos(x) * sin(y),
			}
		}.normalized();
	}

	gmath::matrix get_right() {
		const float x = rotation[0];
		const float y = rotation[1];
		const float z = rotation[2];
		return gmath::matrix {
			{
				(cos(y) * cos(z)) - (sin(y) * sin(x) * sin(z)),
				(cos(y) * sin(z)) + (sin(y) * sin(x) * cos(z)),
				-sin(y) * cos(x),
			}
		}.normalized();
	}

	gmath::matrix get_view_matrix() {
		return gmath::matrix{
			{ 1, 0, 0, 0 },
			{ 0, 1, 0, 0 },
			{ 0, 0, 1, 0 },
			{ -x, -y, -z, 1 },
		} * (gmath::matrix({
			{ 1, 0, 0, 0},
			{ 0, cos(rotation[0]), -sin(rotation[0]), 0},
			{ 0, sin(rotation[0]), cos(rotation[0]), 0},
			{0, 0, 0, 1}
		}) * gmath::matrix({
			{ cos(rotation[1]), 0, sin(rotation[1]), 0},
			{ 0, 1, 0, 0},
			{ -sin(rotation[1]), 0, cos(rotation[1]), 0},
			{0, 0, 0, 1}
		}) * gmath::matrix({
			{ cos(rotation[2]), -sin(rotation[2]), 0, 0},
			{ sin(rotation[2]), cos(rotation[2]), 0, 0},
			{ 0, 0, 1, 0},
			{0, 0, 0, 1}
		})).transposed();
	}

	gmath::matrix get_projection_matrix() {
		return gmath::matrix({
			{ 1 / tan(field_of_view / 2.0f), 0,  0,  0  },
			{ 0, 1 / tan(field_of_view / 2.0f),  0,  0  },
			{ 0, 0, 1 / (z_far - z_near), 1  },
			{ 0, 0,  z_near / (z_far - z_near),  0  },
		});
	}
	gmath::matrix get_vp_matrix() {
		return get_view_matrix() * get_projection_matrix();
	}
};

#endif
