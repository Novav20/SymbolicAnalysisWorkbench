#include <gtest/gtest.h>
#include "SymbolicCore/DHMatrixCalculator.h"
#include "SymbolicCore/SymbolicScalar.h"

using namespace SymbolicCore;

TEST(PrismaticJointTest, JacobianCalculation) {
    // SCARA-like robot: R-R-P-R
    // Joint 3 is Prismatic
    
    DHMatrixCalculator calculator;
    
    // Joint 1: Revolute (theta1, 0, a1, 0)
    calculator.addDHParameters(DHParameters::withVariables("q1", "0", "a1", "0", JointType::Revolute));
    
    // Joint 2: Revolute (theta2, 0, a2, 0)
    calculator.addDHParameters(DHParameters::withVariables("q2", "0", "a2", "0", JointType::Revolute));
    
    // Joint 3: Prismatic (0, d3, 0, 0)
    calculator.addDHParameters(DHParameters(
        SymbolicScalar::constant(0), // theta fixed
        SymbolicScalar::variable("d3"), // d variable
        SymbolicScalar::constant(0), // a
        SymbolicScalar::constant(0), // alpha
        JointType::Prismatic
    ));
    
    // Joint 4: Revolute (theta4, 0, 0, 0)
    calculator.addDHParameters(DHParameters::withVariables("q4", "0", "0", "0", JointType::Revolute));
    
    SymbolicMatrix jacobian = calculator.computeGeometricJacobian();
    
    // Verify Jacobian size
    EXPECT_EQ(jacobian.rows(), 6);
    EXPECT_EQ(jacobian.cols(), 4);
    
    // Verify Column 3 (Index 2) - Prismatic
    // J_v = z_2, J_w = 0
    
    // Since all alphas are 0, all z axes are parallel to z_0 = (0, 0, 1).
    // So J_v should be (0, 0, 1) and J_w should be (0, 0, 0).
    
    // Evaluate with some values to check numerical correctness
    std::map<std::string, double> values = {
        {"q1", 0.5}, {"q2", 0.3}, {"d3", 0.2}, {"q4", 0.1},
        {"a1", 1.0}, {"a2", 1.0}
    };
    
    auto jacobian_val = jacobian.evaluate(values);
    
    // Check J_w (Angular velocity) - Rows 3, 4, 5
    EXPECT_NEAR(jacobian_val[3][2], 0.0, 1e-6);
    EXPECT_NEAR(jacobian_val[4][2], 0.0, 1e-6);
    EXPECT_NEAR(jacobian_val[5][2], 0.0, 1e-6);
    
    // Check J_v (Linear velocity) - Rows 0, 1, 2
    // Should be z_2 = (0, 0, 1)
    EXPECT_NEAR(jacobian_val[0][2], 0.0, 1e-6);
    EXPECT_NEAR(jacobian_val[1][2], 0.0, 1e-6);
    EXPECT_NEAR(jacobian_val[2][2], 1.0, 1e-6);
    
    // Verify Column 1 (Index 0) - Revolute
    // J_w = z_0 = (0, 0, 1)
    EXPECT_NEAR(jacobian_val[3][0], 0.0, 1e-6);
    EXPECT_NEAR(jacobian_val[4][0], 0.0, 1e-6);
    EXPECT_NEAR(jacobian_val[5][0], 1.0, 1e-6);
}
