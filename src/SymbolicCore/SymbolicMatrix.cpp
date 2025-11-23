#include "SymbolicCore/SymbolicMatrix.h"
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <symengine/add.h>
#include <symengine/mul.h>
#include <symengine/pow.h>
#include <symengine/matrix.h>

namespace SymbolicCore {

// Constructors
SymbolicMatrix::SymbolicMatrix() 
    : matrix_(0, 0) {}

SymbolicMatrix::SymbolicMatrix(size_t rows, size_t cols)
    : matrix_(rows, cols) {
    for (unsigned i = 0; i < rows; ++i) {
        for (unsigned j = 0; j < cols; ++j) {
            matrix_.set(i, j, SymEngine::real_double(0.0));
        }
    }
}

SymbolicMatrix::SymbolicMatrix(size_t rows, size_t cols, const SymbolicScalar& init_value)
    : matrix_(rows, cols) {
    for (unsigned i = 0; i < rows; ++i) {
        for (unsigned j = 0; j < cols; ++j) {
            matrix_.set(i, j, init_value.getExpression());
        }
    }
}

SymbolicMatrix::SymbolicMatrix(const std::vector<std::vector<SymbolicScalar>>& elements)
    : matrix_(elements.size(), elements.empty() ? 0 : elements[0].size()) {
    for (size_t i = 0; i < elements.size(); ++i) {
        for (size_t j = 0; j < elements[i].size(); ++j) {
            matrix_.set(i, j, elements[i][j].getExpression());
        }
    }
}

SymbolicMatrix::SymbolicMatrix(const SymEngine::DenseMatrix& matrix)
    : matrix_(matrix) {}

// Element access
SymbolicScalar SymbolicMatrix::get(size_t row, size_t col) const {
    if (row >= matrix_.nrows() || col >= matrix_.ncols()) {
        throw std::out_of_range("Matrix index out of bounds");
    }
    return SymbolicScalar(matrix_.get(row, col));
}

void SymbolicMatrix::set(size_t row, size_t col, const SymbolicScalar& value) {
    if (row >= matrix_.nrows() || col >= matrix_.ncols()) {
        throw std::out_of_range("Matrix index out of bounds");
    }
    matrix_.set(row, col, value.getExpression());
}

// Matrix operations
SymbolicMatrix SymbolicMatrix::operator+(const SymbolicMatrix& other) const {
    SymEngine::DenseMatrix result(matrix_.nrows(), matrix_.ncols());
    matrix_.add_matrix(other.matrix_, result);
    return SymbolicMatrix(result);
}

SymbolicMatrix SymbolicMatrix::operator-(const SymbolicMatrix& other) const {
    SymEngine::DenseMatrix result(matrix_.nrows(), matrix_.ncols());
    // SymEngine doesn't have sub_dense_dense directly exposed in some versions, 
    // but we can do A + (-1)*B
    SymEngine::DenseMatrix neg_other(other.matrix_.nrows(), other.matrix_.ncols());
    other.matrix_.mul_scalar(SymEngine::integer(-1), neg_other);
    matrix_.add_matrix(neg_other, result);
    return SymbolicMatrix(result);
}

SymbolicMatrix SymbolicMatrix::operator*(const SymbolicMatrix& other) const {
    SymEngine::DenseMatrix result(matrix_.nrows(), other.matrix_.ncols());
    matrix_.mul_matrix(other.matrix_, result);
    return SymbolicMatrix(result);
}

SymbolicMatrix SymbolicMatrix::operator*(const SymbolicScalar& scalar) const {
    SymEngine::DenseMatrix result(matrix_.nrows(), matrix_.ncols());
    matrix_.mul_scalar(scalar.getExpression(), result);
    return SymbolicMatrix(result);
}

// Matrix functions
SymbolicMatrix SymbolicMatrix::transpose() const {
    SymEngine::DenseMatrix result(matrix_.ncols(), matrix_.nrows());
    matrix_.transpose(result);
    return SymbolicMatrix(result);
}

SymbolicScalar SymbolicMatrix::determinant() const {
    return SymbolicScalar(matrix_.det());
}

SymbolicMatrix SymbolicMatrix::inverse() const {
    SymEngine::DenseMatrix result(matrix_.nrows(), matrix_.ncols());
    matrix_.inv(result);
    return SymbolicMatrix(result);
}

bool SymbolicMatrix::isSymmetric() const {
    if (matrix_.nrows() != matrix_.ncols()) return false;
    
    for (unsigned i = 0; i < matrix_.nrows(); ++i) {
        for (unsigned j = i + 1; j < matrix_.ncols(); ++j) {
            if (!SymEngine::eq(*matrix_.get(i, j), *matrix_.get(j, i))) {
                return false;
            }
        }
    }
    return true;
}

// Evaluation
Eigen::MatrixXd SymbolicMatrix::evaluateToEigen(const std::map<std::string, double>& variable_values) const {
    Eigen::MatrixXd result(matrix_.nrows(), matrix_.ncols());
    
    SymEngine::map_basic_basic subs_map;
    for (const auto& pair : variable_values) {
        subs_map[SymEngine::symbol(pair.first)] = SymEngine::real_double(pair.second);
    }
    
    for (unsigned i = 0; i < matrix_.nrows(); ++i) {
        for (unsigned j = 0; j < matrix_.ncols(); ++j) {
            auto elem = matrix_.get(i, j);
            auto evaluated = elem->subs(subs_map);
            result(i, j) = SymEngine::eval_double(*evaluated);
        }
    }
    return result;
}

std::vector<std::vector<double>> SymbolicMatrix::evaluate(const std::map<std::string, double>& variable_values) const {
    std::vector<std::vector<double>> result(matrix_.nrows(), std::vector<double>(matrix_.ncols()));
    
    SymEngine::map_basic_basic subs_map;
    for (const auto& pair : variable_values) {
        subs_map[SymEngine::symbol(pair.first)] = SymEngine::real_double(pair.second);
    }
    
    for (unsigned i = 0; i < matrix_.nrows(); ++i) {
        for (unsigned j = 0; j < matrix_.ncols(); ++j) {
            auto elem = matrix_.get(i, j);
            auto evaluated = elem->subs(subs_map);
            result[i][j] = SymEngine::eval_double(*evaluated);
        }
    }
    return result;
}

// Simplification
SymbolicMatrix SymbolicMatrix::simplify() const {
    SymEngine::DenseMatrix result(matrix_.nrows(), matrix_.ncols());
    for (unsigned i = 0; i < matrix_.nrows(); ++i) {
        for (unsigned j = 0; j < matrix_.ncols(); ++j) {
            result.set(i, j, SymEngine::expand(matrix_.get(i, j)));
        }
    }
    return SymbolicMatrix(result);
}

// String representation
std::string SymbolicMatrix::toString() const {
    return matrix_.__str__();
}

std::string SymbolicMatrix::toLatex() const {
    std::ostringstream oss;
    oss << "\\begin{bmatrix}" << std::endl;
    for (unsigned i = 0; i < matrix_.nrows(); ++i) {
        for (unsigned j = 0;  j < matrix_.ncols(); ++j) {
            oss << SymEngine::latex(*matrix_.get(i, j));
            if (j < matrix_.ncols() - 1) oss << " & ";
        }
        if (i < matrix_.nrows() - 1) oss << " \\\\";
        oss << std::endl;
    }
    oss << "\\end{bmatrix}";
    return oss.str();
}

// Static factory methods
SymbolicMatrix SymbolicMatrix::identity(size_t size) {
    SymEngine::DenseMatrix mat(size, size);
    SymEngine::eye(mat);
    return SymbolicMatrix(mat);
}

SymbolicMatrix SymbolicMatrix::zeros(size_t rows, size_t cols) {
    SymEngine::DenseMatrix mat(rows, cols);
    SymEngine::zeros(mat);
    return SymbolicMatrix(mat);
}

SymbolicMatrix SymbolicMatrix::ones(size_t rows, size_t cols) {
    SymEngine::DenseMatrix mat(rows, cols);
    // SymEngine::ones_dense(mat); // If available, otherwise loop
    for (unsigned i = 0; i < rows; ++i) {
        for (unsigned j = 0; j < cols; ++j) {
            mat.set(i, j, SymEngine::integer(1));
        }
    }
    return SymbolicMatrix(mat);
}

// DH Transform matrix
SymbolicMatrix SymbolicMatrix::dhTransformMatrix(
    const SymbolicScalar& theta,
    const SymbolicScalar& d,
    const SymbolicScalar& a,
    const SymbolicScalar& alpha
) {
    // Construct DH matrix using SymEngine expressions directly
    // [ cos(theta), -sin(theta)*cos(alpha),  sin(theta)*sin(alpha), a*cos(theta) ]
    // [ sin(theta),  cos(theta)*cos(alpha), -cos(theta)*sin(alpha), a*sin(theta) ]
    // [          0,             sin(alpha),             cos(alpha),            d ]
    // [          0,                      0,                      0,            1 ]
    
    SymEngine::DenseMatrix T(4, 4);
    
    auto cos_theta = SymEngine::cos(theta.getExpression());
    auto sin_theta = SymEngine::sin(theta.getExpression());
    auto cos_alpha = SymEngine::cos(alpha.getExpression());
    auto sin_alpha = SymEngine::sin(alpha.getExpression());
    
    // Row 0
    T.set(0, 0, cos_theta);
    T.set(0, 1, SymEngine::mul(SymEngine::neg(sin_theta), cos_alpha));
    T.set(0, 2, SymEngine::mul(sin_theta, sin_alpha));
    T.set(0, 3, SymEngine::mul(a.getExpression(), cos_theta));
    
    // Row 1
    T.set(1, 0, sin_theta);
    T.set(1, 1, SymEngine::mul(cos_theta, cos_alpha));
    T.set(1, 2, SymEngine::mul(SymEngine::neg(cos_theta), sin_alpha));
    T.set(1, 3, SymEngine::mul(a.getExpression(), sin_theta));
    
    // Row 2
    T.set(2, 0, SymEngine::integer(0));
    T.set(2, 1, sin_alpha);
    T.set(2, 2, cos_alpha);
    T.set(2, 3, d.getExpression());
    
    // Row 3
    T.set(3, 0, SymEngine::integer(0));
    T.set(3, 1, SymEngine::integer(0));
    T.set(3, 2, SymEngine::integer(0));
    T.set(3, 3, SymEngine::integer(1));
    
    return SymbolicMatrix(T);
}

} // namespace SymbolicCore
