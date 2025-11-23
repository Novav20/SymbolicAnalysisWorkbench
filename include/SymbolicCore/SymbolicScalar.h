#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <set>
#include <symengine/expression.h>
#include <symengine/basic.h>

namespace SymbolicCore {

/**
 * @class SymbolicScalar
 * @brief Symbolic scalar using SymEngine backend.
 */
class SymbolicScalar {
private:
    SymEngine::RCP<const SymEngine::Basic> expression_;
    
public:
    // Constructors
    SymbolicScalar();
    explicit SymbolicScalar(const std::string& expr); // Parses string
    explicit SymbolicScalar(double constant_value);
    explicit SymbolicScalar(const SymEngine::RCP<const SymEngine::Basic>& expr);
    
    // Accessors
    SymEngine::RCP<const SymEngine::Basic> getExpression() const { return expression_; }
    
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
    
    // Static factory methods
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
    
    // Comparison
    bool operator==(const SymbolicScalar& other) const;
    bool operator!=(const SymbolicScalar& other) const;
};

} // namespace SymbolicCore
