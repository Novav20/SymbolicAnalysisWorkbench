#pragma once

#include "SymbolicMatrix.h"
#include "SymbolicScalar.h"
#include <vector>
#include <memory>
#include <string>

namespace SymbolicCore {

/**
 * @struct DHParameters
 * @brief Symbolic representation of Denavit-Hartenberg parameters
 */
struct DHParameters {
    SymbolicScalar theta;  // Joint angle
    SymbolicScalar d;      // Link offset
    SymbolicScalar a;      // Link length
    SymbolicScalar alpha;  // Link twist

    DHParameters(const SymbolicScalar& theta_val,
                 const SymbolicScalar& d_val,
                 const SymbolicScalar& a_val,
                 const SymbolicScalar& alpha_val);

    // Convenience constructors
    static DHParameters withConstants(double theta_val, double d_val, double a_val, double alpha_val);
    static DHParameters withVariables(const std::string& theta_name, const std::string& d_name, 
                                     const std::string& a_name, const std::string& alpha_name);
    static DHParameters mixed(const std::string& theta_name, double d_val, double a_val, double alpha_val);
};

/**
 * @class DHMatrixCalculator
 * @brief Specialized calculator for Denavit-Hartenberg transformation matrices
 * 
 * This class provides symbolic computation capabilities specifically tailored
 * for DH parameter calculations in robotics kinematics.
 */
class DHMatrixCalculator {
private:
    std::vector<DHParameters> dh_parameters_;
    size_t num_joints_;

public:
    // Constructors
    DHMatrixCalculator();
    explicit DHMatrixCalculator(const std::vector<DHParameters>& params);

    // DH parameter management
    void addDHParameters(const DHParameters& params);
    void setDHParameters(const std::vector<DHParameters>& params);
    const std::vector<DHParameters>& getDHParameters() const { return dh_parameters_; }
    size_t getNumJoints() const { return num_joints_; }

    // Transformation matrix calculations
    SymbolicMatrix computeTransformMatrix(size_t joint_index) const;
    SymbolicMatrix computeTransformMatrix(const DHParameters& params) const;
    std::vector<SymbolicMatrix> computeAllTransformMatrices() const;
    SymbolicMatrix computeCumulativeTransform(size_t end_joint) const;

    // Forward kinematics
    SymbolicMatrix computeEndEffectorPosition() const;
    std::vector<SymbolicMatrix> computeAllLinkPositions() const;

    // Inverse kinematics helpers
    SymbolicMatrix extractPositionEquations(const SymbolicMatrix& end_effector_transform) const;
    SymbolicMatrix extractOrientationEquations(const SymbolicMatrix& end_effector_transform) const;

    // Jacobian calculations
    SymbolicMatrix computeGeometricJacobian() const;
    SymbolicMatrix computeAnalyticalJacobian() const;

    // Matrix manipulation utilities
    SymbolicMatrix postMultiplyByInverse(const SymbolicMatrix& matrix, size_t joint_index) const;
    SymbolicMatrix preMultiplyByInverse(const SymbolicMatrix& matrix, size_t joint_index) const;

    // Symbolic differentiation
    SymbolicMatrix differentiateTransform(size_t joint_index, const std::string& variable) const;
    SymbolicMatrix differentiateEndEffectorPosition(const std::string& joint_variable) const;

    // Utility methods
    void simplifyAll();
    std::string generateLatexReport() const;

    // Static utility methods
    static SymbolicMatrix createStandardDHTransform(
        const SymbolicScalar& theta,
        const SymbolicScalar& d,
        const SymbolicScalar& a,
        const SymbolicScalar& alpha
    );

    static SymbolicMatrix createModifiedDHTransform(
        const SymbolicScalar& theta,
        const SymbolicScalar& d,
        const SymbolicScalar& a,
        const SymbolicScalar& alpha
    );

    // Validation methods
    bool isValidDHConfiguration() const;
    std::vector<std::string> getValidationErrors() const;
};

} // namespace SymbolicCore
