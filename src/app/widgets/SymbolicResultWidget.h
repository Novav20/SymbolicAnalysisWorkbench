#pragma once

#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QSplitter>
#include <memory>

namespace SymbolicCore {
    class SymbolicMatrix;
}

/**
 * @class SymbolicResultWidget
 * @brief Widget for displaying symbolic computation results
 * 
 * This widget provides multiple views of symbolic results:
 * - Mathematical notation display
 * - LaTeX representation
 * - Simplified form
 * - Numerical evaluation
 * - Export capabilities
 */
class SymbolicResultWidget : public QWidget {
    Q_OBJECT

private slots:
    void onExportLatex();
    void onExportPython();
    void onExportMathematica();
    void onExportMatlab();
    void onEvaluateNumerically();
    void onSimplifyExpression();
    void onCopyToClipboard();
    void onTabChanged(int index);

private:
    void setupUI();
    void setupResultDisplayTab();
    void setupLatexTab();
    void setupNumericalTab();
    void setupExportTab();
    void connectSignals();
    void updateAllDisplays();
    void updateMathematicalDisplay();
    void updateLatexDisplay();
    void updateNumericalDisplay();
    void updateExportPreview();
    QString formatMatrixAsText(const SymbolicCore::SymbolicMatrix& matrix) const;
    QString formatMatrixAsLatex(const SymbolicCore::SymbolicMatrix& matrix) const;
    QString formatMatrixAsPython(const SymbolicCore::SymbolicMatrix& matrix) const;
    QString formatMatrixAsMathematica(const SymbolicCore::SymbolicMatrix& matrix) const;
    QString formatMatrixAsMatlab(const SymbolicCore::SymbolicMatrix& matrix) const;
    std::vector<std::vector<double>> evaluateMatrix(const SymbolicCore::SymbolicMatrix& matrix) const;

    // Main layout
    QVBoxLayout* main_layout_;
    QSplitter* main_splitter_;
    
    // Tab widget for different display modes
    QTabWidget* tab_widget_;
    
    // Result display tab
    QWidget* result_display_tab_;
    QVBoxLayout* result_display_layout_;
    QGroupBox* matrix_info_group_;
    QVBoxLayout* matrix_info_layout_;
    QLabel* matrix_size_label_;
    QLabel* matrix_type_label_;
    QLabel* matrix_properties_label_;
    QTextEdit* result_display_;
    
    // LaTeX display tab
    QWidget* latex_tab_;
    QVBoxLayout* latex_layout_;
    QTextEdit* latex_display_;
    QPushButton* copy_latex_btn_;
    
    // Numerical evaluation tab
    QWidget* numerical_tab_;
    QVBoxLayout* numerical_layout_;
    QGroupBox* evaluation_params_group_;
    QVBoxLayout* evaluation_params_layout_;
    QLabel* variable_values_label_;
    QTextEdit* variable_values_edit_;
    QPushButton* evaluate_btn_;
    QTextEdit* numerical_result_;
    
    // Export tab
    QWidget* export_tab_;
    QVBoxLayout* export_layout_;
    QGroupBox* export_format_group_;
    QVBoxLayout* export_format_layout_;
    QPushButton* export_latex_btn_;
    QPushButton* export_python_btn_;
    QPushButton* export_mathematica_btn_;
    QPushButton* export_matlab_btn_;
    QTextEdit* export_preview_;
    
    // Control buttons
    QHBoxLayout* control_buttons_layout_;
    QPushButton* simplify_btn_;
    QPushButton* copy_to_clipboard_btn_;
    QPushButton* clear_btn_;
    
    // Current result
    std::shared_ptr<SymbolicCore::SymbolicMatrix> current_matrix_;
    bool is_dirty_;

public:
    explicit SymbolicResultWidget(QWidget *parent = nullptr);
    ~SymbolicResultWidget();

    // Public interface
    void displayMatrix(const SymbolicCore::SymbolicMatrix& matrix);
    void clear();
    void setVariableValues(const std::map<std::string, double>& values);
    SymbolicCore::SymbolicMatrix getCurrentMatrix() const;

signals:
    void matrixUpdated(const SymbolicCore::SymbolicMatrix& matrix);
    void exportRequested(const QString& format, const QString& content);
    void errorOccurred(const QString& error);
};
