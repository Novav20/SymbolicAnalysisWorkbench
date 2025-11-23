#include "SymbolicCore/SymbolicScalar.h"
#include <symengine/parser.h>
#include <symengine/eval_double.h>
#include <symengine/derivative.h>
#include <symengine/simplify.h>
#include <symengine/printers.h>
#include <symengine/symbol.h>
#include <symengine/real_double.h>
#include <symengine/add.h>
#include <symengine/mul.h>
#include <symengine/pow.h>
#include <symengine/functions.h>

namespace SymbolicCore {

// Constructors
SymbolicScalar::SymbolicScalar() 
    : expression_(SymEngine::real_double(0.0)) {}

SymbolicScalar::SymbolicScalar(const std::string& expr) {
    try {
        expression_ = SymEngine::parse(expr);
    } catch (...) {
        // Fallback or error handling
        expression_ = SymEngine::real_double(0.0);
    }
}

SymbolicScalar::SymbolicScalar(double constant_value) 
    : expression_(SymEngine::real_double(constant_value)) {}

SymbolicScalar::SymbolicScalar(const SymEngine::RCP<const SymEngine::Basic>& expr)
    : expression_(expr) {}

// Evaluation
double SymbolicScalar::evaluate(const std::map<std::string, double>& variable_values) const {
    SymEngine::map_basic_basic subs_map;
    for (const auto& pair : variable_values) {
        subs_map[SymEngine::symbol(pair.first)] = SymEngine::real_double(pair.second);
    }
    
    auto evaluated = expression_->subs(subs_map);
    return SymEngine::eval_double(*evaluated);
}

// Derivative
std::shared_ptr<SymbolicScalar> SymbolicScalar::derivative(const std::string& variable) const {
    auto x = SymEngine::symbol(variable);
    auto diff = expression_->diff(x);
    return std::make_shared<SymbolicScalar>(diff);
}

// Simplification
SymbolicScalar SymbolicScalar::simplify() const {
    // SymEngine automatically simplifies many things on construction
    // But we can explicitly call expand() or other simplification methods
    return SymbolicScalar(SymEngine::expand(expression_));
}

// Get variables
std::set<std::string> SymbolicScalar::getVariables() const {
    std::set<std::string> variables;
    auto syms = SymEngine::free_symbols(*expression_);
    for (const auto& sym : syms) {
        variables.insert(sym->__str__());
    }
    return variables;
}

// String representation
std::string SymbolicScalar::toString() const {
    return expression_->__str__();
}

std::string SymbolicScalar::toLatex() const {
    return SymEngine::latex(*expression_);
}

// Static factory methods
SymbolicScalar SymbolicScalar::variable(const std::string& name) {
    return SymbolicScalar(SymEngine::symbol(name));
}

SymbolicScalar SymbolicScalar::constant(double value) {
    return SymbolicScalar(value);
}

SymbolicScalar SymbolicScalar::parse(const std::string& expression) {
    return SymbolicScalar(expression);
}

// Trigonometric functions
SymbolicScalar SymbolicScalar::sin(const SymbolicScalar& arg) {
    return SymbolicScalar(SymEngine::sin(arg.expression_));
}

SymbolicScalar SymbolicScalar::cos(const SymbolicScalar& arg) {
    return SymbolicScalar(SymEngine::cos(arg.expression_));
}

SymbolicScalar SymbolicScalar::tan(const SymbolicScalar& arg) {
    return SymbolicScalar(SymEngine::tan(arg.expression_));
}

// Mathematical functions
SymbolicScalar SymbolicScalar::exp(const SymbolicScalar& arg) {
    return SymbolicScalar(SymEngine::exp(arg.expression_));
}

SymbolicScalar SymbolicScalar::log(const SymbolicScalar& arg) {
    return SymbolicScalar(SymEngine::log(arg.expression_));
}

SymbolicScalar SymbolicScalar::sqrt(const SymbolicScalar& arg) {
    return SymbolicScalar(SymEngine::sqrt(arg.expression_));
}

SymbolicScalar SymbolicScalar::pow(const SymbolicScalar& base, const SymbolicScalar& exponent) {
    return SymbolicScalar(SymEngine::pow(base.expression_, exponent.expression_));
}

// Operator overloads
SymbolicScalar SymbolicScalar::operator+(const SymbolicScalar& other) const {
    return SymbolicScalar(SymEngine::add(expression_, other.expression_));
}

SymbolicScalar SymbolicScalar::operator-(const SymbolicScalar& other) const {
    return SymbolicScalar(SymEngine::sub(expression_, other.expression_));
}

SymbolicScalar SymbolicScalar::operator*(const SymbolicScalar& other) const {
    return SymbolicScalar(SymEngine::mul(expression_, other.expression_));
}

SymbolicScalar SymbolicScalar::operator/(const SymbolicScalar& other) const {
    return SymbolicScalar(SymEngine::div(expression_, other.expression_));
}

SymbolicScalar SymbolicScalar::operator-() const {
    return SymbolicScalar(SymEngine::neg(expression_));
}

// Compound assignment
SymbolicScalar& SymbolicScalar::operator+=(const SymbolicScalar& other) {
    expression_ = SymEngine::add(expression_, other.expression_);
    return *this;
}

SymbolicScalar& SymbolicScalar::operator-=(const SymbolicScalar& other) {
    expression_ = SymEngine::sub(expression_, other.expression_);
    return *this;
}

SymbolicScalar& SymbolicScalar::operator*=(const SymbolicScalar& other) {
    expression_ = SymEngine::mul(expression_, other.expression_);
    return *this;
}

SymbolicScalar& SymbolicScalar::operator/=(const SymbolicScalar& other) {
    expression_ = SymEngine::div(expression_, other.expression_);
    return *this;
}

// Comparison
bool SymbolicScalar::operator==(const SymbolicScalar& other) const {
    return SymEngine::eq(*expression_, *other.expression_);
}

bool SymbolicScalar::operator!=(const SymbolicScalar& other) const {
    return !(*this == other);
}

} // namespace SymbolicCore
