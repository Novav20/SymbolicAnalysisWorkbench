#include "SymbolicCore/SymbolicScalar.h"
#include "exprtk.hpp"
#include <regex>
#include <algorithm>
#include <cctype>

namespace SymbolicCore {

// Constructors
SymbolicScalar::SymbolicScalar() : expression_("0") {}

SymbolicScalar::SymbolicScalar(const std::string& expr) : expression_(expr) {}

SymbolicScalar::SymbolicScalar(double constant_value) {
    std::ostringstream oss;
    oss << constant_value;
    expression_ = oss.str();
}

// Evaluation using ExprTk
double SymbolicScalar::evaluate(const std::map<std::string, double>& variable_values) const {
    typedef exprtk::symbol_table<double> symbol_table_t;
    typedef exprtk::expression<double> expression_t;
    typedef exprtk::parser<double> parser_t;
    
    symbol_table_t symbol_table;
    expression_t expression;
    parser_t parser;
    
    // Add variables to symbol table
    std::map<std::string, double> vars = variable_values;
    for (auto& pair : vars) {
        symbol_table.add_variable(pair.first, pair.second);
    }
    
    expression.register_symbol_table(symbol_table);
    
    // Parse and evaluate
    if (!parser.compile(expression_, expression)) {
        // If parsing fails, try to evaluate as a constant
        try {
            return std::stod(expression_);
        } catch (...) {
            return 0.0; // Return 0 if can't parse
        }
    }
    
    return expression.value();
}

// Derivative (symbolic differentiation - basic implementation)
std::shared_ptr<SymbolicScalar> SymbolicScalar::derivative(const std::string& variable) const {
    // This is a simplified symbolic derivative
    // For a production system, you'd implement proper differentiation rules
    
    std::string result;
    
    // Check if expression is just the variable
    if (expression_ == variable) {
        result = "1";
    }
    // Check if expression is a constant (no variables)
    else if (getVariables().find(variable) == getVariables().end()) {
        result = "0";
   }
    // Check for linear terms like "2*theta1" or "theta1*2"
    else if (expression_.find(variable) != std::string::npos) {
        // Simplified: For now, return derivative hint
        result = "d(" + expression_ + ")/d(" + variable + ")";
    }
    else {
        result = "0";
    }
    
    return std::make_shared<SymbolicScalar>(result);
}

// Simplification (basic)
SymbolicScalar SymbolicScalar::simplify() const {
    // Basic simplifications
    std::string simplified = expression_;
    
    // Remove double negatives
    std::regex double_neg("--");
    simplified = std::regex_replace(simplified, double_neg, "+");
    
    // Simplify + followed by +
    std::regex double_plus("\\+\\+");
    simplified = std::regex_replace(simplified, double_plus, "+");
    
    // Try to evaluate if it's a constant expression
    if (getVariables().empty()) {
        try {
            double val = evaluate();
            return SymbolicScalar(val);
        } catch (...) {
            // Can't evaluate, return as-is
        }
    }
    
    return SymbolicScalar(simplified);
}

// Get all variables in the expression
std::set<std::string> SymbolicScalar::getVariables() const {
    std::set<std::string> variables;
    
    // Use regex to find variable names (alphanumeric sequences starting with letter)
    std::regex var_regex("[a-zA-Z_][a-zA-Z0-9_]*");
    std::sregex_iterator iter(expression_.begin(), expression_.end(), var_regex);
    std::sregex_iterator end;
    
    // Known function names to exclude
    std::set<std::string> functions = {"sin", "cos", "tan", "exp", "log", "sqrt", "abs", "pow"};
    
    while (iter != end) {
        std::string var = iter->str();
        if (functions.find(var) == functions.end()) {
            variables.insert(var);
        }
        ++iter;
    }
    
    return variables;
}

// String representation
std::string SymbolicScalar::toString() const {
    return expression_;
}

std::string SymbolicScalar::toLatex() const {
    std::string latex = expression_;
    
    // 1. Replace Greek letters and standard DH parameters with subscripts
    // theta1 -> \theta_1, alpha2 -> \alpha_2, d3 -> d_3, a4 -> a_4
    // We use a regex that looks for the name followed by digits
    
    // theta -> \theta
    std::regex theta_regex("theta([0-9]+)");
    latex = std::regex_replace(latex, theta_regex, "\\\\theta_{$1}");
    
    // alpha -> \alpha
    std::regex alpha_regex("alpha([0-9]+)");
    latex = std::regex_replace(latex, alpha_regex, "\\\\alpha_{$1}");
    
    // d -> d (just subscript)
    std::regex d_regex("d([0-9]+)");
    latex = std::regex_replace(latex, d_regex, "d_{$1}");
    
    // a -> a (just subscript)
    std::regex a_regex("a([0-9]+)");
    latex = std::regex_replace(latex, a_regex, "a_{$1}");
    
    // 2. Convert functions
    std::regex sin_regex("sin\\(([^)]*)\\)");
    latex = std::regex_replace(latex, sin_regex, "\\\\sin($1)");
    
    std::regex cos_regex("cos\\(([^)]*)\\)");
    latex = std::regex_replace(latex, cos_regex, "\\\\cos($1)");
    
    std::regex tan_regex("tan\\(([^)]*)\\)");
    latex = std::regex_replace(latex, tan_regex, "\\\\tan($1)");
    
    std::regex sqrt_regex("sqrt\\(([^)]*)\\)");
    latex = std::regex_replace(latex, sqrt_regex, "\\\\sqrt{$1}");
    
    // 3. Operators
    // Replace * with \cdot
    std::regex mult_regex("\\*");
    latex = std::regex_replace(latex, mult_regex, " \\\\cdot ");
    
    // 4. Cleanup
    // Remove redundant parentheses around single terms if possible (simple heuristic)
    // For now, we just return the string.
    
    return latex; // Note: We don't wrap in $...$ here, the widget will do that or the matrix will
}

// Static factory methods
SymbolicScalar SymbolicScalar::variable(const std::string& name) {
    return SymbolicScalar(name);
}

SymbolicScalar SymbolicScalar::constant(double value) {
    return SymbolicScalar(value);
}

SymbolicScalar SymbolicScalar::parse(const std::string& expression) {
    return SymbolicScalar(expression);
}

// Trigonometric functions
SymbolicScalar SymbolicScalar::sin(const SymbolicScalar& arg) {
    return SymbolicScalar("sin(" + arg.expression_ + ")");
}

SymbolicScalar SymbolicScalar::cos(const SymbolicScalar& arg) {
    return SymbolicScalar("cos(" + arg.expression_ + ")");
}

SymbolicScalar SymbolicScalar::tan(const SymbolicScalar& arg) {
    return SymbolicScalar("tan(" + arg.expression_ + ")");
}

// Mathematical functions
SymbolicScalar SymbolicScalar::exp(const SymbolicScalar& arg) {
    return SymbolicScalar("exp(" + arg.expression_ + ")");
}

SymbolicScalar SymbolicScalar::log(const SymbolicScalar& arg) {
    return SymbolicScalar("log(" + arg.expression_ + ")");
}

SymbolicScalar SymbolicScalar::sqrt(const SymbolicScalar& arg) {
    return SymbolicScalar("sqrt(" + arg.expression_ + ")");
}

SymbolicScalar SymbolicScalar::pow(const SymbolicScalar& base, const SymbolicScalar& exponent) {
    return SymbolicScalar("pow(" + base.expression_ + "," + exponent.expression_ + ")");
}

// Helper methods
std::string SymbolicScalar::wrapInParens(const std::string& expr) {
    if (needsParens(expr)) {
        return "(" + expr + ")";
    }
    return expr;
}

bool SymbolicScalar::needsParens(const std::string& expr) {
    // Check if expression contains operators that would need parentheses
    return expr.find('+') != std::string::npos || 
           expr.find('-') != std::string::npos;
}

// Operator overloads
SymbolicScalar SymbolicScalar::operator+(const SymbolicScalar& other) const {
    return SymbolicScalar(expression_ + "+" + other.expression_);
}

SymbolicScalar SymbolicScalar::operator-(const SymbolicScalar& other) const {
    return SymbolicScalar(expression_ + "-" + wrapInParens(other.expression_));
}

SymbolicScalar SymbolicScalar::operator*(const SymbolicScalar& other) const {
    std::string left = wrapInParens(expression_);
    std::string right = wrapInParens(other.expression_);
    return SymbolicScalar(left + "*" + right);
}

SymbolicScalar SymbolicScalar::operator/(const SymbolicScalar& other) const {
    std::string left = wrapInParens(expression_);
    std::string right = wrapInParens(other.expression_);
    return SymbolicScalar(left + "/" + right);
}

SymbolicScalar SymbolicScalar::operator-() const {
    return SymbolicScalar("-" + wrapInParens(expression_));
}

// Compound assignment  
SymbolicScalar& SymbolicScalar::operator+=(const SymbolicScalar& other) {
    *this = *this + other;
    return *this;
}

SymbolicScalar& SymbolicScalar::operator-=(const SymbolicScalar& other) {
    *this = *this - other;
    return *this;
}

SymbolicScalar& SymbolicScalar::operator*=(const SymbolicScalar& other) {
    *this = *this * other;
    return *this;
}

SymbolicScalar& SymbolicScalar::operator/=(const SymbolicScalar& other) {
    *this = *this / other;
    return *this;
}

// Comparison
bool SymbolicScalar::operator==(const SymbolicScalar& other) const {
    return expression_ == other.expression_;
}

bool SymbolicScalar::operator!=(const SymbolicScalar& other) const {
    return !(*this == other);
}

} // namespace SymbolicCore
