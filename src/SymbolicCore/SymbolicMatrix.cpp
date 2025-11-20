#include "SymbolicCore/SymbolicMatrix.h"
#include <stdexcept>
#include <sstream>
#include <iomanip>

namespace SymbolicCore {

// Constructors
SymbolicMatrix::SymbolicMatrix() : rows_(0), cols_(0) {}

SymbolicMatrix::SymbolicMatrix(size_t rows, size_t cols)
    : rows_(rows), cols_(cols), elements_(rows, std::vector<SymbolicScalar>(cols, SymbolicScalar(0.0))) {}

SymbolicMatrix::SymbolicMatrix(size_t rows, size_t cols, const SymbolicScalar& init_value)
    : rows_(rows), cols_(cols), elements_(rows, std::vector<SymbolicScalar>(cols, init_value)) {}

SymbolicMatrix::SymbolicMatrix(const std::vector<std::vector<SymbolicScalar>>& elements)
    : rows_(elements.size()), cols_(elements.empty() ? 0 : elements[0].size()), elements_(elements) {
    // Validate that all rows have the same number of columns
    for (const auto& row : elements_) {
        if (row.size() != cols_) {
            throw std::invalid_argument("All rows must have the same number of columns");
        }
    }
}

// Element access
SymbolicScalar& SymbolicMatrix::operator()(size_t row, size_t col) {
    validateDimensions(row, col);
    return elements_[row][col];
}

const SymbolicScalar& SymbolicMatrix::operator()(size_t row, size_t col) const {
    validateDimensions(row, col);
    return elements_[row][col];
}

SymbolicScalar SymbolicMatrix::get(size_t row, size_t col) const {
    validateDimensions(row, col);
    return elements_[row][col];
}

void SymbolicMatrix::set(size_t row, size_t col, const SymbolicScalar& value) {
    validateDimensions(row, col);
    elements_[row][col] = value;
}

// Matrix operations
SymbolicMatrix SymbolicMatrix::operator+(const SymbolicMatrix& other) const {
    if (rows_ != other.rows_ || cols_ != other.cols_) {
        throw std::invalid_argument("Matrix dimensions must match for addition");
    }
    
    SymbolicMatrix result(rows_, cols_);
    for (size_t i = 0; i < rows_; ++i) {
        for (size_t j = 0; j < cols_; ++j) {
            result(i, j) = elements_[i][j] + other.elements_[i][j];
        }
    }
    return result;
}

SymbolicMatrix SymbolicMatrix::operator-(const SymbolicMatrix& other) const {
    if (rows_ != other.rows_ || cols_ != other.cols_) {
        throw std::invalid_argument("Matrix dimensions must match for subtraction");
    }
    
    SymbolicMatrix result(rows_, cols_);
    for (size_t i = 0; i < rows_; ++i) {
        for (size_t j = 0; j < cols_; ++j) {
            result(i, j) = elements_[i][j] - other.elements_[i][j];
        }
    }
    return result;
}

SymbolicMatrix SymbolicMatrix::operator*(const SymbolicMatrix& other) const {
    if (cols_ != other.rows_) {
        throw std::invalid_argument("Inner matrix dimensions must match for multiplication");
    }
    
    SymbolicMatrix result(rows_, other.cols_);
    for (size_t i = 0; i < rows_; ++i) {
        for (size_t j = 0; j < other.cols_; ++j) {
            SymbolicScalar sum(0.0);
            for (size_t k = 0; k < cols_; ++k) {
                sum = sum + (elements_[i][k] * other.elements_[k][j]);
            }
            result(i, j) = sum;
        }
    }
    return result;
}

SymbolicMatrix SymbolicMatrix::operator*(const SymbolicScalar& scalar) const {
    SymbolicMatrix result(rows_, cols_);
    for (size_t i = 0; i < rows_; ++i) {
        for (size_t j = 0; j < cols_; ++j) {
            result(i, j) = elements_[i][j] * scalar;
        }
    }
    return result;
}

// Matrix functions
SymbolicMatrix SymbolicMatrix::transpose() const {
    SymbolicMatrix result(cols_, rows_);
    for (size_t i = 0; i < rows_; ++i) {
        for (size_t j = 0; j < cols_; ++j) {
            result(j, i) = elements_[i][j];
        }
    }
    return result;
}

SymbolicScalar SymbolicMatrix::determinant() const {
    if (rows_ != cols_) {
        throw std::invalid_argument("Determinant only defined for square matrices");
    }
    return computeDeterminant(*this);
}

SymbolicMatrix SymbolicMatrix::inverse() const {
    if (rows_ != cols_) {
        throw std::invalid_argument("Inverse only defined for square matrices");
    }
    
    SymbolicScalar det = determinant();
    SymbolicMatrix result(rows_, cols_);
    
    // Compute adjugate matrix (transpose of cofactor matrix)
    for (size_t i = 0; i < rows_; ++i) {
        for (size_t j = 0; j < cols_; ++j) {
            SymbolicMatrix minor = computeMinor(i, j);
            SymbolicScalar cofactor = minor.determinant();
            
            // Apply sign based on position
            if ((i + j) % 2 == 1) {
                cofactor = -cofactor;
            }
            
            // Transpose indices for adjugate
            result(j, i) = cofactor / det;
        }
    }
    
    return result;
}

bool SymbolicMatrix::isSymmetric() const {
    if (rows_ != cols_) return false;
    
    for (size_t i = 0; i < rows_; ++i) {
        for (size_t j = i + 1; j < cols_; ++j) {
            if (elements_[i][j].toString() != elements_[j][i].toString()) {
                return false;
            }
        }
    }
    return true;
}

// Evaluation
Eigen::MatrixXd SymbolicMatrix::evaluateToEigen(const std::map<std::string, double>& variable_values) const {
    Eigen::MatrixXd result(rows_, cols_);
    for (size_t i = 0; i < rows_; ++i) {
        for (size_t j = 0; j < cols_; ++j) {
            result(i, j) = elements_[i][j].evaluate(variable_values);
        }
    }
    return result;
}

std::vector<std::vector<double>> SymbolicMatrix::evaluate(const std::map<std::string, double>& variable_values) const {
    std::vector<std::vector<double>> result(rows_, std::vector<double>(cols_));
    for (size_t i = 0; i < rows_; ++i) {
        for (size_t j = 0; j < cols_; ++j) {
            result[i][j] = elements_[i][j].evaluate(variable_values);
        }
    }
    return result;
}

// Simplification
SymbolicMatrix SymbolicMatrix::simplify() const {
    SymbolicMatrix result(rows_, cols_);
    for (size_t i = 0; i < rows_; ++i) {
        for (size_t j = 0; j < cols_; ++j) {
            result(i, j) = elements_[i][j].simplify();
        }
    }
    return result;
}

// String representation
std::string SymbolicMatrix::toString() const {
    std::ostringstream oss;
    oss << "[" << std::endl;
    for (size_t i = 0; i < rows_; ++i) {
        oss << "  [";
        for (size_t j = 0; j < cols_; ++j) {
            oss << elements_[i][j].toString();
            if (j < cols_ - 1) oss << ", ";
        }
        oss << "]";
        if (i < rows_ - 1) oss << ",";
        oss << std::endl;
    }
    oss << "]";
    return oss.str();
}

std::string SymbolicMatrix::toLatex() const {
    std::ostringstream oss;
    oss << "\\begin{bmatrix}" << std::endl;
    for (size_t i = 0; i < rows_; ++i) {
        for (size_t j = 0;  j < cols_; ++j) {
            oss << elements_[i][j].toLatex();
            if (j < cols_ - 1) oss << " & ";
        }
        if (i < rows_ - 1) oss << " \\\\";
        oss << std::endl;
    }
    oss << "\\end{bmatrix}";
    return oss.str();
}

// Static factory methods
SymbolicMatrix SymbolicMatrix::identity(size_t size) {
    SymbolicMatrix result(size, size, SymbolicScalar(0.0));
    for (size_t i = 0; i < size; ++i) {
        result(i, i) = SymbolicScalar(1.0);
    }
    return result;
}

SymbolicMatrix SymbolicMatrix::zeros(size_t rows, size_t cols) {
    return SymbolicMatrix(rows, cols, SymbolicScalar(0.0));
}

SymbolicMatrix SymbolicMatrix::ones(size_t rows, size_t cols) {
    return SymbolicMatrix(rows, cols, SymbolicScalar(1.0));
}

// DH Transform matrix
SymbolicMatrix SymbolicMatrix::dhTransformMatrix(
    const SymbolicScalar& theta,
    const SymbolicScalar& d,
    const SymbolicScalar& a,
    const SymbolicScalar& alpha
) {
    SymbolicMatrix T(4, 4);
    
    SymbolicScalar cos_theta = SymbolicScalar::cos(theta);
    SymbolicScalar sin_theta = SymbolicScalar::sin(theta);
    SymbolicScalar cos_alpha = SymbolicScalar::cos(alpha);
    SymbolicScalar sin_alpha = SymbolicScalar::sin(alpha);
    
    // Row 0
    T(0, 0) = cos_theta;
    T(0, 1) = -sin_theta * cos_alpha;
    T(0, 2) = sin_theta * sin_alpha;
    T(0, 3) = a * cos_theta;
    
    // Row 1
    T(1, 0) = sin_theta;
    T(1, 1) = cos_theta * cos_alpha;
    T(1, 2) = -cos_theta * sin_alpha;
    T(1, 3) = a * sin_theta;
    
    // Row 2
    T(2, 0) = SymbolicScalar(0.0);
    T(2, 1) = sin_alpha;
    T(2, 2) = cos_alpha;
    T(2, 3) = d;
    
    // Row 3
    T(3, 0) = SymbolicScalar(0.0);
    T(3, 1) = SymbolicScalar(0.0);
    T(3, 2) = SymbolicScalar(0.0);
    T(3, 3) = SymbolicScalar(1.0);
    
    return T;
}

// Helper methods
void SymbolicMatrix::validateDimensions(size_t row, size_t col) const {
    if (row >= rows_ || col >= cols_) {
        throw std::out_of_range("Matrix index out of bounds");
    }
}

SymbolicScalar SymbolicMatrix::computeDeterminant(const SymbolicMatrix& mat) const {
    if (mat.rows_ == 1) {
        return mat.elements_[0][0];
    }
    
    if (mat.rows_ == 2) {
        return mat.elements_[0][0] * mat.elements_[1][1] - 
               mat.elements_[0][1] * mat.elements_[1][0];
    }
    
    // Laplace expansion along first row
    SymbolicScalar det(0.0);
    for (size_t j = 0; j < mat.cols_; ++j) {
        SymbolicMatrix minor = mat.computeMinor(0, j);
        SymbolicScalar cofactor = mat.elements_[0][j] * computeDeterminant(minor);
        
        if (j % 2 == 0) {
            det = det + cofactor;
        } else {
            det = det - cofactor;
        }
    }
    
    return det;
}

SymbolicMatrix SymbolicMatrix::computeMinor(size_t exclude_row, size_t exclude_col) const {
    SymbolicMatrix minor(rows_ - 1, cols_ - 1);
    
    size_t minor_row = 0;
    for (size_t i = 0; i < rows_; ++i) {
        if (i == exclude_row) continue;
        
        size_t minor_col = 0;
        for (size_t j = 0; j < cols_; ++j) {
            if (j == exclude_col) continue;
            
            minor(minor_row, minor_col) = elements_[i][j];
            ++minor_col;
        }
        ++minor_row;
    }
    
    return minor;
}

} // namespace SymbolicCore
