#pragma once

#include "SymbolicScalar.h"
#include <Eigen/Dense>
#include <vector>
#include <string>
#include <map>
#include <symengine/matrix.h>

namespace SymbolicCore {

/**
 * @class SymbolicMatrix
 * @brief Symbolic matrix using SymEngine backend.
 */
class SymbolicMatrix {
private:
    SymEngine::DenseMatrix matrix_;

public:
    // Constructors
    SymbolicMatrix();
    SymbolicMatrix(size_t rows, size_t cols);
    SymbolicMatrix(size_t rows, size_t cols, const SymbolicScalar& init_value);
    SymbolicMatrix(const std::vector<std::vector<SymbolicScalar>>& elements);
    SymbolicMatrix(const SymEngine::DenseMatrix& matrix);
    
    // Dimensions
    size_t rows() const { return matrix_.nrows(); }
    size_t cols() const { return matrix_.ncols(); }
    
    // Element access
    SymbolicScalar get(size_t row, size_t col) const;
    void set(size_t row, size_t col, const SymbolicScalar& value);
    
    // Matrix operations
    SymbolicMatrix operator+(const SymbolicMatrix& other) const;
    SymbolicMatrix operator-(const SymbolicMatrix& other) const;
    SymbolicMatrix operator*(const SymbolicMatrix& other) const;
    SymbolicMatrix operator*(const SymbolicScalar& scalar) const;
    
    // Matrix functions
    SymbolicMatrix transpose() const;
    SymbolicScalar determinant() const;
    SymbolicMatrix inverse() const;
    bool isSymmetric() const;
    
    // Evaluation
    Eigen::MatrixXd evaluateToEigen(const std::map<std::string, double>& variable_values = {}) const;
    std::vector<std::vector<double>> evaluate(const std::map<std::string, double>& variable_values = {}) const;
    
    // Simplification
    SymbolicMatrix simplify() const;
    
    // String representation
    std::string toString() const;
    std::string toLatex() const;
    
    // Static factory methods
    static SymbolicMatrix identity(size_t size);
    static SymbolicMatrix zeros(size_t rows, size_t cols);
    static SymbolicMatrix ones(size_t rows, size_t cols);
    
    // DH Transform matrix factory
    static SymbolicMatrix dhTransformMatrix(
        const SymbolicScalar& theta,
        const SymbolicScalar& d,
        const SymbolicScalar& a,
        const SymbolicScalar& alpha
    );
};

} // namespace SymbolicCore
