#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <set>
#include <sstream>
#include <cmath>

namespace SymbolicCore {

/**
 * @class SymbolicScalar
 * @brief Symbolic scalar using string-based expression representation.
 * 
 * This implementation stores expressions as strings and provides
 * symbolic manipulation capabilities suitable for DH parameters.
 */
class SymbolicScalar {
private:
    std::string expression_;  // Expression as string (e.g., "sin(theta1) + 2*d")
    
public:
    // Constructors
    SymbolicScalar();
    explicit SymbolicScalar(const std::string& expr);
    explicit SymbolicScalar(double constant_value);
    
    // Accessors
    std::string getExpression() const { return expression_; }
    
    // Evaluation
    double evaluate(const std::map<std::string, double>& variable_values = {}) const;
    
    // Symbolic operations
    std::shared_ptr<SymbolicScalar> derivative(const std::string& variable) const;
    SymbolicScalar simplify() const;
    
    // Analysis
    std::set<std::string> getVariables() const;
    
    // String representation
    std::string toString() const;
    std::string toLatex() const;
    
    // Static factory methods for variables and functions
    static SymbolicScalar variable(const std::string& name);
    static SymbolicScalar constant(double value);
    static SymbolicScalar parse(const std::string& expression);
    
    // Trigonometric functions
    static SymbolicScalar sin(const SymbolicScalar& arg);
    static SymbolicScalar cos(const SymbolicScalar& arg);
    static SymbolicScalar tan(const SymbolicScalar& arg);
    
    // Mathematical functions
    static SymbolicScalar exp(const SymbolicScalar& arg);
    static SymbolicScalar log(const SymbolicScalar& arg);
    static SymbolicScalar sqrt(const SymbolicScalar& arg);
    static SymbolicScalar pow(const SymbolicScalar& base, const SymbolicScalar& exponent);
    
    // Operator overloads
    SymbolicScalar operator+(const SymbolicScalar& other) const;
    SymbolicScalar operator-(const SymbolicScalar& other) const;
    SymbolicScalar operator*(const SymbolicScalar& other) const;
    SymbolicScalar operator/(const SymbolicScalar& other) const;
    SymbolicScalar operator-() const; // Unary minus
    
    // Compound assignment operators
    SymbolicScalar& operator+=(const SymbolicScalar& other);
    SymbolicScalar& operator-=(const SymbolicScalar& other);
    SymbolicScalar& operator*=(const SymbolicScalar& other);
    SymbolicScalar& operator/=(const SymbolicScalar& other);
    
    // Comparison (for sorting/maps)
    bool operator==(const SymbolicScalar& other) const;
    bool operator!=(const SymbolicScalar& other) const;
    
private:
    // Helper methods
    static std::string wrapInParens(const std::string& expr);
    static bool needsParens(const std::string& expr);
};

} // namespace SymbolicCore
