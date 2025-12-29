#pragma once

#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTableWidgetItem>
#include <QtWidgets/QTabWidget>
#include <vector>
#include <memory>

namespace SymbolicCore {
    class SymbolicMatrix;
    class SymbolicScalar;
}

/**
 * @class MatrixInputWidget
 * @brief Widget for inputting matrices and performing symbolic operations
 * 
 * This widget provides an interface for:
 * - Creating symbolic matrices with variables
 * - Performing matrix operations (add, multiply, transpose, inverse, etc.)
 * - Computing determinants and other properties
 * - Special robotics matrix operations
 */
class MatrixInputWidget : public QWidget {
    Q_OBJECT

private slots:
    void onMatrixSizeChanged();
    void onOperationSelected();
    void onComputeMatrix();
    void onClearMatrix();
    void onAddVariable();
    void onRemoveVariable();
    void onPresetMatrixSelected();
    void onCellChanged(int row, int col);
    void onVariableValueChanged();

private:
    void setupUI();
    void setupMatrixInputTab();
    void setupOperationsTab();
    void setupVariablesTab();
    void setupPresetsTab();
    void connectSignals();
    void updateMatrixTable();
    void updateVariableList();
    SymbolicCore::SymbolicMatrix createMatrixFromTable() const;
    SymbolicCore::SymbolicMatrix performOperation(const SymbolicCore::SymbolicMatrix& matrix) const;
    void updateResultDisplay(const SymbolicCore::SymbolicMatrix& result);

    // Main layout
    QVBoxLayout* main_layout_;
    QTabWidget* tab_widget_;
    
    // Matrix Input Tab
    QWidget* matrix_input_tab_;
    QVBoxLayout* matrix_input_layout_;
    
    // Matrix size controls
    QGroupBox* size_group_;
    QHBoxLayout* size_layout_;
    QLabel* rows_label_;
    QSpinBox* rows_spin_;
    QLabel* cols_label_;
    QSpinBox* cols_spin_;
    
    // Matrix table
    QScrollArea* matrix_scroll_;
    QTableWidget* matrix_table_;
    
    // Matrix control buttons
    QHBoxLayout* matrix_buttons_layout_;
    QPushButton* clear_matrix_btn_;
    QPushButton* compute_matrix_btn_;
    
    // Operations Tab
    QWidget* operations_tab_;
    QVBoxLayout* operations_layout_;
    
    // Operation selection
    QGroupBox* operation_group_;
    QVBoxLayout* operation_layout_;
    QComboBox* operation_combo_;
    QLabel* operation_description_;
    
    // Operation parameters
    QGroupBox* parameters_group_;
    QVBoxLayout* parameters_layout_;
    QLabel* parameters_label_;
    QTextEdit* parameters_edit_;
    
    // Variables Tab
    QWidget* variables_tab_;
    QVBoxLayout* variables_layout_;
    
    // Variable list
    QGroupBox* variable_list_group_;
    QVBoxLayout* variable_list_layout_;
    QTableWidget* variable_table_;
    
    // Variable controls
    QHBoxLayout* variable_buttons_layout_;
    QPushButton* add_variable_btn_;
    QPushButton* remove_variable_btn_;
    
    // Presets Tab
    QWidget* presets_tab_;
    QVBoxLayout* presets_layout_;
    QComboBox* preset_combo_;
    QLabel* preset_description_;
    QPushButton* apply_preset_btn_;
    
    // Current operation
    QString current_operation_;
    
    // Variable storage
    std::vector<std::pair<QString, SymbolicCore::SymbolicScalar>> variables_;

public:
    explicit MatrixInputWidget(QWidget *parent = nullptr);
    ~MatrixInputWidget();

    // Public interface
    void clear();
    SymbolicCore::SymbolicMatrix getCurrentMatrix() const;
    void setMatrix(const SymbolicCore::SymbolicMatrix& matrix);
    void addVariable(const QString& name, const SymbolicCore::SymbolicScalar& value);
    void removeVariable(const QString& name);

signals:
    void matrixComputed(const SymbolicCore::SymbolicMatrix& matrix);
    void operationCompleted(const QString& operation, const SymbolicCore::SymbolicMatrix& result);
    void errorOccurred(const QString& error);
};
