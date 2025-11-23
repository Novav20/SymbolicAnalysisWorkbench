#include "SymbolicCore/DHMatrixCalculator.h"
#include <stdexcept>
#include <sstream>

namespace SymbolicCore {

// DHParameters implementation
DHParameters::DHParameters(const SymbolicScalar& theta_val,
                          const SymbolicScalar& d_val,
                          const SymbolicScalar& a_val,
                          const SymbolicScalar& alpha_val,
                          JointType type_val)
    : theta(theta_val), d(d_val), a(a_val), alpha(alpha_val), type(type_val) {}

DHParameters DHParameters::withConstants(double theta_val, double d_val, double a_val, double alpha_val, JointType type_val) {
    return DHParameters(
        SymbolicScalar::constant(theta_val),
        SymbolicScalar::constant(d_val),
        SymbolicScalar::constant(a_val),
        SymbolicScalar::constant(alpha_val),
        type_val
    );
}

DHParameters DHParameters::withVariables(const std::string& theta_name, const std::string& d_name,
                                         const std::string& a_name, const std::string& alpha_name, JointType type_val) {
    return DHParameters(
        SymbolicScalar::variable(theta_name),
        SymbolicScalar::variable(d_name),
        SymbolicScalar::variable(a_name),
        SymbolicScalar::variable(alpha_name),
        type_val
    );
}

DHParameters DHParameters::mixed(const std::string& theta_name, double d_val, double a_val, double alpha_val, JointType type_val) {
    return DHParameters(
        SymbolicScalar::variable(theta_name),
        SymbolicScalar::constant(d_val),
        SymbolicScalar::constant(a_val),
        SymbolicScalar::constant(alpha_val),
        type_val
    );
}

// DHMatrixCalculator implementation
DHMatrixCalculator::DHMatrixCalculator() : num_joints_(0) {}

DHMatrixCalculator::DHMatrixCalculator(const std::vector<DHParameters>& params)
    : dh_parameters_(params), num_joints_(params.size()) {}

void DHMatrixCalculator::addDHParameters(const DHParameters& params) {
    dh_parameters_.push_back(params);
    num_joints_ = dh_parameters_.size();
}

void DHMatrixCalculator::setDHParameters(const std::vector<DHParameters>& params) {
    dh_parameters_ = params;
    num_joints_ = params.size();
}

SymbolicMatrix DHMatrixCalculator::computeTransformMatrix(size_t joint_index) const {
    if (joint_index >= num_joints_) {
        throw std::out_of_range("Joint index out of range");
    }
    return computeTransformMatrix(dh_parameters_[joint_index]);
}

SymbolicMatrix DHMatrixCalculator::computeTransformMatrix(const DHParameters& params) const {
    return SymbolicMatrix::dhTransformMatrix(params.theta, params.d, params.a, params.alpha);
}

std::vector<SymbolicMatrix> DHMatrixCalculator::computeAllTransformMatrices() const {
    std::vector<SymbolicMatrix> result;
    result.reserve(num_joints_);
    for (size_t i = 0; i < num_joints_; ++i) {
        result.push_back(computeTransformMatrix(i));
    }
    return result;
}

SymbolicMatrix DHMatrixCalculator::computeCumulativeTransform(size_t end_joint) const {
    if (end_joint > num_joints_) {
        throw std::out_of_range("End joint index out of range");
    }
    
    if (end_joint == 0) {
        return SymbolicMatrix::identity(4);
    }
    
    SymbolicMatrix result = computeTransformMatrix(0);
    for (size_t i = 1; i < end_joint; ++i) {
        result = result * computeTransformMatrix(i);
    }
    return result;
}

SymbolicMatrix DHMatrixCalculator::computeEndEffectorPosition() const {
    return computeCumulativeTransform(num_joints_);
}

std::vector<SymbolicMatrix> DHMatrixCalculator::computeAllLinkPositions() const {
    std::vector<SymbolicMatrix> positions;
    positions.reserve(num_joints_ + 1);
    
    positions.push_back(SymbolicMatrix::identity(4)); // Base
    
    for (size_t i = 1; i <= num_joints_; ++i) {
        positions.push_back(computeCumulativeTransform(i));
    }
    
    return positions;
}

SymbolicMatrix DHMatrixCalculator::extractPositionEquations(const SymbolicMatrix& end_effector_transform) const {
    if (end_effector_transform.rows() < 3 || end_effector_transform.cols() < 4) {
        throw std::invalid_argument("Transform matrix must be at least 3x4");
    }
    
    SymbolicMatrix position(3, 1);
    position.set(0, 0, end_effector_transform.get(0, 3));
    position.set(1, 0, end_effector_transform.get(1, 3));
    position.set(2, 0, end_effector_transform.get(2, 3));
    
    return position;
}

SymbolicMatrix DHMatrixCalculator::extractOrientationEquations(const SymbolicMatrix& end_effector_transform) const {
    if (end_effector_transform.rows() < 3 || end_effector_transform.cols() < 3) {
        throw std::invalid_argument("Transform matrix must be at least 3x3");
    }
    
    SymbolicMatrix rotation(3, 3);
    for (size_t i = 0; i < 3; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            rotation.set(i, j, end_effector_transform.get(i, j));
        }
    }
    
    return rotation;
}

SymbolicMatrix DHMatrixCalculator::computeGeometricJacobian() const {
    size_t n = num_joints_;
    SymbolicMatrix jacobian(6, n);
    
    // Compute cumulative transforms for all links (0 to N)
    // T[i] is transform from base to frame i
    std::vector<SymbolicMatrix> T;
    T.reserve(n + 1);
    T.push_back(SymbolicMatrix::identity(4)); // T0 (Base)
    
    for (size_t i = 1; i <= n; ++i) {
        T.push_back(computeCumulativeTransform(i));
    }
    
    // End effector position (p_e) is the position part of T[n]
    SymbolicMatrix p_e = extractPositionEquations(T[n]); // 3x1 vector
    
    for (size_t i = 0; i < n; ++i) {
        // For joint i+1 (which is index i in dh_parameters_), we need z_{i-1} and p_{i-1}
        // z_{i-1} is the z-axis of frame i-1 (3rd column of T[i])
        // p_{i-1} is the origin of frame i-1 (position part of T[i])
        
        SymbolicMatrix z_prev(3, 1);
        z_prev.set(0, 0, T[i].get(0, 2));
        z_prev.set(1, 0, T[i].get(1, 2));
        z_prev.set(2, 0, T[i].get(2, 2));
        
        SymbolicMatrix p_prev = extractPositionEquations(T[i]);
        
        if (dh_parameters_[i].type == JointType::Revolute) {
            // J_v = z_{i-1} x (p_e - p_{i-1})
            // J_w = z_{i-1}
            
            // Calculate p_e - p_{i-1}
            SymbolicMatrix diff = p_e - p_prev;
            
            // Calculate cross product: z_prev x diff
            SymbolicMatrix J_v(3, 1);
            J_v.set(0, 0, z_prev.get(1, 0) * diff.get(2, 0) - z_prev.get(2, 0) * diff.get(1, 0));
            J_v.set(1, 0, z_prev.get(2, 0) * diff.get(0, 0) - z_prev.get(0, 0) * diff.get(2, 0));
            J_v.set(2, 0, z_prev.get(0, 0) * diff.get(1, 0) - z_prev.get(1, 0) * diff.get(0, 0));
            
            // Fill Jacobian column i
            jacobian.set(0, i, J_v.get(0, 0));
            jacobian.set(1, i, J_v.get(1, 0));
            jacobian.set(2, i, J_v.get(2, 0));
            jacobian.set(3, i, z_prev.get(0, 0));
            jacobian.set(4, i, z_prev.get(1, 0));
            jacobian.set(5, i, z_prev.get(2, 0));
            
        } else { // Prismatic
            // J_v = z_{i-1}
            // J_w = 0
            
            jacobian.set(0, i, z_prev.get(0, 0));
            jacobian.set(1, i, z_prev.get(1, 0));
            jacobian.set(2, i, z_prev.get(2, 0));
            jacobian.set(3, i, SymbolicScalar(0.0));
            jacobian.set(4, i, SymbolicScalar(0.0));
            jacobian.set(5, i, SymbolicScalar(0.0));
        }
    }
    
    return jacobian;
}

SymbolicMatrix DHMatrixCalculator::computeAnalyticalJacobian() const {
    return computeGeometricJacobian();
}

SymbolicMatrix DHMatrixCalculator::postMultiplyByInverse(const SymbolicMatrix& matrix, size_t joint_index) const {
    SymbolicMatrix T_joint = computeTransformMatrix(joint_index);
    return matrix * T_joint.inverse();
}

SymbolicMatrix DHMatrixCalculator::preMultiplyByInverse(const SymbolicMatrix& matrix, size_t joint_index) const {
    SymbolicMatrix T_joint = computeTransformMatrix(joint_index);
    return T_joint.inverse() * matrix;
}

SymbolicMatrix DHMatrixCalculator::differentiateTransform(size_t joint_index, const std::string& variable) const {
    if (joint_index >= num_joints_) {
        throw std::out_of_range("Joint index out of range");
    }
    
    SymbolicMatrix T = computeTransformMatrix(joint_index);
    SymbolicMatrix dT(T.rows(), T.cols());
    
    for (size_t i = 0; i < T.rows(); ++i) {
        for (size_t j = 0; j < T.cols(); ++j) {
            auto deriv = T.get(i, j).derivative(variable);
            dT.set(i, j, *deriv);
        }
    }
    
    return dT;
}

SymbolicMatrix DHMatrixCalculator::differentiateEndEffectorPosition(const std::string& joint_variable) const {
    SymbolicMatrix end_effector = computeEndEffectorPosition();
    SymbolicMatrix d_end_effector(end_effector.rows(), end_effector.cols());
    
    for (size_t i = 0; i < end_effector.rows(); ++i) {
        for (size_t j = 0; j < end_effector.cols(); ++j) {
            auto deriv = end_effector.get(i, j).derivative(joint_variable);
            d_end_effector.set(i, j, *deriv);
        }
    }
    
    return d_end_effector;
}

void DHMatrixCalculator::simplifyAll() {
    // In-place simplification of DH parameters
    for (auto& params : dh_parameters_) {
        params.theta = params.theta.simplify();
        params.d = params.d.simplify();
        params.a = params.a.simplify();
        params.alpha = params.alpha.simplify();
    }
}

std::string DHMatrixCalculator::generateLatexReport() const {
    std::ostringstream oss;
    oss << "\\section{DH Parameters}" << std::endl;
    oss << "\\begin{table}[h]" << std::endl;
    oss << "\\begin{tabular}{|c|c|c|c|c|}" << std::endl;
    oss << "\\hline" << std::endl;
    oss << "Joint & $\\theta$ & $d$ & $a$ & $\\alpha$ \\\\" << std::endl;
    oss << "\\hline" << std::endl;
    
    for (size_t i = 0; i < num_joints_; ++i) {
        oss << i + 1 << " & ";
        oss << dh_parameters_[i].theta.toString() << " & ";
        oss << dh_parameters_[i].d.toString() << " & ";
        oss << dh_parameters_[i].a.toString() << " & ";
        oss << dh_parameters_[i].alpha.toString() << " \\\\";
        oss << std::endl;
    }
    
    oss << "\\hline" << std::endl;
    oss << "\\end{tabular}" << std::endl;
    oss << "\\end{table}" << std::endl;
    
    return oss.str();
}

SymbolicMatrix DHMatrixCalculator::createStandardDHTransform(
    const SymbolicScalar& theta,
    const SymbolicScalar& d,
    const SymbolicScalar& a,
    const SymbolicScalar& alpha
) {
    return SymbolicMatrix::dhTransformMatrix(theta, d, a, alpha);
}

SymbolicMatrix DHMatrixCalculator::createModifiedDHTransform(
    const SymbolicScalar& theta,
    const SymbolicScalar& d,
    const SymbolicScalar& a,
    const SymbolicScalar& alpha
) {
    // Modified DH has different order
    SymbolicMatrix T(4, 4);
    
    SymbolicScalar cos_theta = SymbolicScalar::cos(theta);
    SymbolicScalar sin_theta = SymbolicScalar::sin(theta);
    SymbolicScalar cos_alpha = SymbolicScalar::cos(alpha);
    SymbolicScalar sin_alpha = SymbolicScalar::sin(alpha);
    
    // Modified DH convention (Craig's notation)
    T.set(0, 0, cos_theta);
    T.set(0, 1, -sin_theta);
    T.set(0, 2, SymbolicScalar(0.0));
    T.set(0, 3, a);
    
    T.set(1, 0, sin_theta * cos_alpha);
    T.set(1, 1, cos_theta * cos_alpha);
    T.set(1, 2, -sin_alpha);
    T.set(1, 3, -d * sin_alpha);
    
    T.set(2, 0, sin_theta * sin_alpha);
    T.set(2, 1, cos_theta * sin_alpha);
    T.set(2, 2, cos_alpha);
    T.set(2, 3, d * cos_alpha);
    
    T.set(3, 0, SymbolicScalar(0.0));
    T.set(3, 1, SymbolicScalar(0.0));
    T.set(3, 2, SymbolicScalar(0.0));
    T.set(3, 3, SymbolicScalar(1.0));
    
    return T;
}

bool DHMatrixCalculator::isValidDHConfiguration() const {
    // Basic validation - check that we have parameters
    return num_joints_ > 0 && dh_parameters_.size() == num_joints_;
}

std::vector<std::string> DHMatrixCalculator::getValidationErrors() const {
    std::vector<std::string> errors;
    
    if (num_joints_ == 0) {
        errors.push_back("No DH parameters configured");
    }
    
    if (dh_parameters_.size() != num_joints_) {
        errors.push_back("Mismatch between num_joints and dh_parameters size");
    }
    
    return errors;
}

} // namespace SymbolicCore
