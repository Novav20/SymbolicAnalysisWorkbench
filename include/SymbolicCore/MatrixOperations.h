#pragma once

#include "SymbolicMatrix.h"
#include "SymbolicScalar.h"
#include <vector>
#include <memory>

namespace SymbolicCore {

/**
 * @class MatrixOperations
 * @brief Provides advanced symbolic matrix operations for robotics
 * 
 * This class contains specialized matrix operations commonly needed in
 * robotics analysis, particularly for kinematics and dynamics calculations.
 */
class MatrixOperations {
public:
    // Basic matrix decompositions
    static std::pair<SymbolicMatrix, SymbolicMatrix> luDecomposition(const SymbolicMatrix& matrix);
    static std::vector<SymbolicMatrix> qrDecomposition(const SymbolicMatrix& matrix);
    static std::vector<SymbolicMatrix> svdDecomposition(const SymbolicMatrix& matrix);

    // Matrix analysis
    static std::shared_ptr<SymbolicScalar> trace(const SymbolicMatrix& matrix);
    static SymbolicMatrix cofactorMatrix(const SymbolicMatrix& matrix);
    static SymbolicMatrix minorMatrix(const SymbolicMatrix& matrix, size_t row, size_t col);

    // Specialized operations for robotics
    static SymbolicMatrix jacobian(const std::vector<SymbolicMatrix>& transformation_matrices, 
                                  const std::vector<std::string>& joint_variables);
    static SymbolicMatrix geometricJacobian(const std::vector<SymbolicMatrix>& transformation_matrices,
                                           const std::vector<std::shared_ptr<SymbolicScalar>>& joint_variables);
    static SymbolicMatrix analyticalJacobian(const SymbolicMatrix& geometric_jacobian,
                                           const std::vector<std::shared_ptr<SymbolicScalar>>& joint_variables);

    // DH parameter specific operations
    static SymbolicMatrix extractRotation(const SymbolicMatrix& transform);
    static SymbolicMatrix extractTranslation(const SymbolicMatrix& transform);
    static std::shared_ptr<SymbolicScalar> extractEulerAngles(const SymbolicMatrix& rotation, const std::string& axis);

    // Matrix equation solving
    static SymbolicMatrix solveLinearSystem(const SymbolicMatrix& A, const SymbolicMatrix& b);
    static SymbolicMatrix leastSquares(const SymbolicMatrix& A, const SymbolicMatrix& b);

    // Optimization-related operations
    static std::shared_ptr<SymbolicScalar> matrixNorm(const SymbolicMatrix& matrix, const std::string& norm_type = "frobenius");
    static SymbolicMatrix pseudoInverse(const SymbolicMatrix& matrix);

    // Symbolic differentiation of matrices
    static SymbolicMatrix differentiate(const SymbolicMatrix& matrix, const std::string& variable);
    static SymbolicMatrix differentiate(const SymbolicMatrix& matrix, const std::vector<std::string>& variables);

    // Integration with numerical methods
    static std::vector<std::vector<double>> evaluateMatrixFunction(
        const SymbolicMatrix& matrix,
        const std::vector<std::map<std::string, double>>& variable_sets
    );

    // Error analysis
    static std::shared_ptr<SymbolicScalar> conditionNumber(const SymbolicMatrix& matrix);
    static SymbolicMatrix nearestOrthogonalMatrix(const SymbolicMatrix& matrix);

private:
    // Helper methods
    static SymbolicMatrix computeMinor(const SymbolicMatrix& matrix, size_t row_to_remove, size_t col_to_remove);
    static std::shared_ptr<SymbolicScalar> computeCofactor(const SymbolicMatrix& matrix, size_t row, size_t col);
    static bool isSymbolicallyInvertible(const SymbolicMatrix& matrix);
};

} // namespace SymbolicCore
