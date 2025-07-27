#ifndef GRAPHICS_MATH_H
#define GRAPHICS_MATH_H

#include <cstring>
#include <initializer_list>
#include <stdexcept>
#include <vector>

namespace gmath {
	struct matrix {
		matrix(std::initializer_list<std::initializer_list<float>> _data);

		matrix(size_t rows, size_t cols, float val = 0);

		matrix(const matrix& rhs);

		~matrix();

		const matrix operator=(const matrix& rhs);

		float& operator[](int r, int c) const;

		const matrix get_row(int r) const;

		const matrix get_col(int c) const;

		matrix operator+(const matrix& rhs) const;

		matrix operator-() const;

		matrix operator-(const matrix& rhs) const;

		matrix operator*(const matrix& rhs) const;

		matrix operator*(const float scalar) const;

		const float dot(const matrix& rhs) const;

		const matrix cross(const matrix& rhs) const;

		const matrix normalized() const;

		const matrix submatrix(int rows, int cols, int r_start = 0, int c_start = 0) const;

		matrix transposed() const;

		matrix inverted() const;

		void operator+=(const matrix& rhs);

		void operator-=(const matrix& rhs);

		void operator*=(const matrix& rhs);

		void operator*=(const float scalar);

		void print() const;

		float* data;
		size_t rows;
		size_t cols;
	};

	matrix identity(size_t size);
};
#endif
