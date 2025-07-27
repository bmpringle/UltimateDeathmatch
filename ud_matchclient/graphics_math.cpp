#include "graphics_math.h"
#include <cmath>
#include <cstring>
#include <initializer_list>
#include <stdexcept>
#include <vector>
#include <iostream>
#include <numeric>

gmath::matrix::matrix(std::initializer_list<std::initializer_list<float>> _data) : rows(_data.size()) {
	if(_data.size() == 0) {
		data = new float[0];
		return;
	}
	data = new float[_data.size() * _data.begin()->size()];
	cols = _data.begin()->size();

	for(int i = 0; i < _data.size(); ++i) {
		auto it = std::next(_data.begin(), i);

		if(cols != it->size()) {
			throw std::runtime_error("A matrix can only be declared with a consistent number of columns.");
		}

		for(int j = 0; j < it->size(); ++j) {
			data[i * cols + j] = *(std::next(it->begin(), j));
		}
	}
}

gmath::matrix::matrix(size_t rows, size_t cols, float val) : data(new float[rows * cols]), rows(rows), cols(cols)  {
	for(int i = 0; i < rows; ++i) {
		for(int j = 0; j < cols; ++j) {
			data[i * cols + j] = val;
		}
	}
}

gmath::matrix::matrix(const gmath::matrix& rhs) : gmath::matrix(rhs.rows, rhs.cols, 0) {
	for(int i = 0; i < rows; ++i) {
		for(int j = 0; j < cols; ++j) {
			data[i * cols + j] = rhs[i, j];
		}
	}
}

gmath::matrix::~matrix() {
	delete[] data;
}

const gmath::matrix gmath::matrix::operator=(const gmath::matrix& rhs) {
	delete[] data;
	rows = rhs.rows;
	cols = rhs.cols;
	data = new float[rows * cols];

	for(int i = 0; i < rows; ++i) {
		for(int j = 0; j < cols; ++j) {
			data[i * cols + j] = rhs[i, j];
		}
	}
	return *this;
}

float& gmath::matrix::operator[](int r, int c) const {
	if(r >= rows || r < 0) {
		throw std::runtime_error("r is out of the matrix's range.");
	}
	if(c >= cols || c < 0) {
		throw std::runtime_error("c is out of the matrix's range.");
	}
	return data[r * cols + c];
}

const gmath::matrix gmath::matrix::get_row(int r) const {
	if(r >= rows || r < 0) {
		throw std::runtime_error("r is out of the matrix's range.");
	}
	gmath::matrix result = gmath::matrix(1, cols, 0);
	for(int i = 0; i < cols; ++i) {
		result[0, i] = data[r * cols + i];
	}
	return result;
}

const gmath::matrix gmath::matrix::get_col(int c) const {
	if(c >= cols || c < 0) {
		throw std::runtime_error("c is out of the matrix's range.");
	}
	gmath::matrix result = gmath::matrix(rows, 1, 0);
	for(int i = 0; i < rows; ++i) {
		result[i, 0] = data[i * cols + c];
	}
	return result;
}

gmath::matrix gmath::matrix::operator+(const gmath::matrix& rhs) const {
	if(rows != rhs.rows || cols != rhs.cols) {
		throw std::runtime_error("Matrix dimensions must match to do addition.");
	}
	gmath::matrix result = gmath::matrix(*this);
	for(int i = 0; i < rows; ++i) {
		for(int j = 0; j < cols; ++j) {
			result[i, j] += rhs[i, j];
		}
	}
	return result;
}

gmath::matrix gmath::matrix::operator-() const {
	gmath::matrix result = gmath::matrix(*this);
	for(int i = 0; i < rows; ++i) {
		for(int j = 0; j < cols; ++j) {
			result[i, j] = -result[i, j];
		}
	}
	return result;
}

gmath::matrix gmath::matrix::operator-(const gmath::matrix& rhs) const {
	if(rows != rhs.rows || cols != rhs.cols) {
		throw std::runtime_error("Matrix dimensions must match to do subtraction.");
	}
	gmath::matrix result = gmath::matrix(*this);
	for(int i = 0; i < rows; ++i) {
		for(int j = 0; j < cols; ++j) {
			result[i, j] -= rhs[i, j];
		}
	}
	return result;
}

gmath::matrix gmath::matrix::operator*(const gmath::matrix& rhs) const {
	if(cols != rhs.rows) {
		throw std::runtime_error("Matrix multiplies can only be done when the left matrix's row count is equal to the right matrix's column count.");
	}
	gmath::matrix result = gmath::matrix(rows, rhs.cols, 0);

	for(int i = 0; i < result.rows; ++i) {
		for(int j = 0; j < result.cols; ++j) {
			result[i, j] = get_row(i).dot(rhs.get_col(j));
		}
	}
	return result;
}

gmath::matrix gmath::matrix::operator*(const float scalar) const {
	gmath::matrix result = *this;
	for(int i = 0; i < rows; ++i) {
		for(int j = 0; j < cols; ++j) {
			result[i, j] *= scalar;
		}
	}
	return result;
}

const float gmath::matrix::dot(const gmath::matrix& rhs) const {
	if(rows != 1 || rhs.cols != 1 || cols != rhs.rows) {
		throw std::runtime_error("Failed to meet dot product input requirements.");
	}
	float result = 0;
	for(int i = 0; i < cols; ++i) {
		result += data[i] * rhs[i, 0];
	}
	return result;
}

const gmath::matrix gmath::matrix::cross(const gmath::matrix& rhs) const {
	if((rows * cols != rhs.rows * rhs.cols) || (rows != 1 && cols != 1) || (rhs.rows != 1 && rhs.cols != 1) || (rows * cols != 3)) {
		throw std::runtime_error("Failed to meet cross product input requirements.");
	}
	return gmath::matrix({
		{
			data[1] * rhs.data[2] - data[2] * rhs.data[1],
			data[2] * rhs.data[0] - data[0] * rhs.data[2],
			data[0] * rhs.data[1] - data[1] * rhs.data[0]
		}
	});
}

gmath::matrix gmath::matrix::transposed() const {
	gmath::matrix result = gmath::matrix(cols, rows, 0);
	for(int i = 0; i < rows; ++i) {
		for(int j = 0; j < cols; ++j) {
			result[j, i] = data[i * cols + j];
		}
	}
	return result;
}

void gmath::matrix::operator+=(const gmath::matrix& rhs) {
	*this = (*this) + rhs;
}

void gmath::matrix::operator-=(const gmath::matrix& rhs) {
	*this = (*this) - rhs;
}

void gmath::matrix::operator*=(const gmath::matrix& rhs) {
	*this = (*this) * rhs;
}

void gmath::matrix::operator*=(const float scalar) {
	*this = (*this) * scalar;
}

void gmath::matrix::print() const {
	std::cout << rows << " x " << cols << std::endl;
	for(int r = 0; r < rows; ++r) {
		std::string row_str = " ";
		for(int c = 0; c < cols; ++c) {
			row_str += std::to_string(data[r * cols + c]) + " ";
		}
		std::cout << row_str << std::endl;
	}
}

gmath::matrix gmath::identity(size_t size) {
	gmath::matrix result = gmath::matrix(size, size, 0);
	for(int i = 0; i < size; ++i) {
		result[i, i] = 1;
	}
	return result;
}

void perform_elimination_step(gmath::matrix& original, gmath::matrix& inverse, int col, int r_src, int r_dst) {
	float src_row_multiplier = original[r_dst, col] / original[r_src, col];
	gmath::matrix new_row = original.get_row(r_dst) - (original.get_row(r_src) * src_row_multiplier);
	for(int i = 0; i < original.cols; ++i) {
		original[r_dst, i] = new_row[0, i];
	}

	new_row = inverse.get_row(r_dst) - (inverse.get_row(r_src) * src_row_multiplier);
	for(int i = 0; i < inverse.cols; ++i) {
		inverse[r_dst, i] = new_row[0, i];
	}
}

bool equals_tolerance(float tolerance, float a, float b) {
	return abs(a - b) < tolerance;
}

gmath::matrix gmath::matrix::inverted() const {
	if(rows != cols) {
		std::cout << "Inverses can only be calculated for square matrices." << std::endl;
		return gmath::identity(rows);
		throw std::runtime_error("Inverses can only be calculated for square matrices.");
	}
	gmath::matrix inverse = gmath::identity(rows);
	gmath::matrix original = *this;

	int max_loops = 0;
	while(max_loops < 100) {
		int i1 = -1;
		int j_src = -1;
		int j_dst = -1;

		for(int i = 0; i < cols && i1 == -1; ++i) {
			// Find j_src
			for(int j = 0; j < rows && i1 == -1; ++j) {
				if(!equals_tolerance(0.001, original[j, i], 0)) {
					bool all_zeros = true;
					for(int k = 0; k < i; ++k) {
						if(!equals_tolerance(0.001, original[j, k], 0)) {
							all_zeros = false;
							break;
						}
					}
					if(all_zeros) {
						j_src = j;
						break;
					}
				}
			}
			if(j_src == -1) {
				continue;
			}
			// Find j_dst
			for(int j = 0; j < rows && i1 == -1; ++j) {
				if(!equals_tolerance(0.001, original[j, i], 0)) {
					if(j_src != j) {
						j_dst = j;
						i1 = i;
						break;
					}
				}
			}
			if(i1 == -1) {
				j_src = -1;
				j_dst = -1;
			}
		}

		if(i1 == -1) {
			break;
		}
		perform_elimination_step(original, inverse, i1, j_src, j_dst);

		bool completed_col = true;
		for(int j = 0; j < rows; ++j) {
			if(!equals_tolerance(0.001, original[j, i1], 0) && j != i1) {
				completed_col = false;
			}
		}

		if(completed_col) {
			if(equals_tolerance(0.0001, original[i1, i1], 0)) {
				std::cout << "The matrix is not invertable." << std::endl;
				return original;
				throw std::runtime_error("The matrix is not invertable.");
			}
			if(!equals_tolerance(0.001, original[i1, i1], 1)) {
				float divisor = original[i1, i1];
				for(int i = 0; i < original.cols; ++i) {
					original[i, i1] = original[i, i1] / divisor;
				}
				for(int i = 0; i < inverse.cols; ++i) {
					inverse[i, i1] = inverse[i, i1] / divisor;
				}
			}
		}
		++max_loops;
	}
	return inverse;
}

const gmath::matrix gmath::matrix::normalized() const {
	if(rows != 1 && cols != 1) {
		throw std::runtime_error("This is not a vector (row or column of 1) matrix and so cannot be normalized.");
	}
	gmath::matrix result = *this;
	double size = 0.0;
	for(int i = 0; i < rows * cols; ++i) {
		size += data[i] * data[i];
	}
	size = sqrt(size);
	for(int i = 0; i < rows * cols; ++i) {
		result.data[i] /= size;
	}
	return result;
}

const gmath::matrix gmath::matrix::submatrix(int rows, int cols, int r_start, int c_start) const {
	if(r_start + rows > this->rows || c_start + cols > this->cols) {
		throw std::runtime_error("There aren't enough rows/columns to get this submatrix");
	}
	gmath::matrix result = gmath::matrix(rows, cols, 0);
	for(int i = 0; i < rows; ++i) {
		for(int j = 0; j < cols; ++j) {
			result[i, j] = (*this)[r_start + i, c_start + j];
		}
	}
	return result;
}
