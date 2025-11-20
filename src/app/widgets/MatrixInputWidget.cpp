#include "MatrixInputWidget.h"
#include "SymbolicCore/SymbolicMatrix.h"
#include "SymbolicCore/SymbolicScalar.h"
#include "SymbolicCore/MatrixOperations.h"

#include <QtWidgets/QMessageBox>
#include <QtWidgets/QHeaderView>
#include <QtCore/QRegularExpression>
#include <QRegularExpressionValidator>

MatrixInputWidget::MatrixInputWidget(QWidget *parent)
    : QWidget(parent)
    , main_layout_(nullptr)
    , tab_widget_(nullptr)
    , matrix_input_tab_(nullptr)
    , matrix_input_layout_(nullptr)
    , size_group_(nullptr)
    , size_layout_(nullptr)
    , rows_label_(nullptr)
    , rows_spin_(nullptr)
    , cols_label_(nullptr)
    , cols_spin_(nullptr)
    , matrix_scroll_(nullptr)
    , matrix_table_(nullptr)
    , matrix_buttons_layout_(nullptr)
    , clear_matrix_btn_(nullptr)
    , compute_matrix_btn_(nullptr)
    , operations_tab_(nullptr)
    , operations_layout_(nullptr)
    , operation_group_(nullptr)
    , operation_layout_(nullptr)
    , operation_combo_(nullptr)
    , operation_description_(nullptr)
    , parameters_group_(nullptr)
    , parameters_layout_(nullptr)
    , parameters_label_(nullptr)
    , parameters_edit_(nullptr)
    , variables_tab_(nullptr)
    , variables_layout_(nullptr)
    , variable_list_group_(nullptr)
    , variable_list_layout_(nullptr)
    , variable_table_(nullptr)
    , variable_buttons_layout_(nullptr)
    , add_variable_btn_(nullptr)
    , remove_variable_btn_(nullptr)
    , presets_tab_(nullptr)
    , presets_layout_(nullptr)
    , preset_combo_(nullptr)
    , preset_description_(nullptr)
    , apply_preset_btn_(nullptr)
{
    setupUI();
    connectSignals();
}

MatrixInputWidget::~MatrixInputWidget() = default;

void MatrixInputWidget::setupUI()
{
    main_layout_ = new QVBoxLayout(this);
    tab_widget_ = new QTabWidget(this);
    
    setupMatrixInputTab();
    setupOperationsTab();
    setupVariablesTab();
    setupPresetsTab();
    
    tab_widget_->addTab(matrix_input_tab_, "Matrix Input");
    tab_widget_->addTab(operations_tab_, "Operations");
    tab_widget_->addTab(variables_tab_, "Variables");
    tab_widget_->addTab(presets_tab_, "Presets");
    
    main_layout_->addWidget(tab_widget_);
}

void MatrixInputWidget::setupMatrixInputTab()
{
    matrix_input_tab_ = new QWidget();
    matrix_input_layout_ = new QVBoxLayout(matrix_input_tab_);
    
    // Size controls
    size_group_ = new QGroupBox("Matrix Size");
    size_layout_ = new QHBoxLayout(size_group_);
    rows_label_ = new QLabel("Rows:");
    rows_spin_ = new QSpinBox();
    rows_spin_->setRange(1, 10);
    rows_spin_->setValue(3);
    cols_label_ = new QLabel("Columns:");
    cols_spin_ = new QSpinBox();
    cols_spin_->setRange(1, 10);
    cols_spin_->setValue(3);
    
    size_layout_->addWidget(rows_label_);
    size_layout_->addWidget(rows_spin_);
    size_layout_->addWidget(cols_label_);
    size_layout_->addWidget(cols_spin_);
    size_layout_->addStretch();
    
    // Matrix table
    matrix_scroll_ = new QScrollArea();
    matrix_table_ = new QTableWidget(3, 3);
    matrix_table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    matrix_table_->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            auto item = new QTableWidgetItem("0");
            matrix_table_->setItem(i, j, item);
        }
    }
    
    matrix_scroll_->setWidget(matrix_table_);
    matrix_scroll_->setWidgetResizable(true);
    
    // Control buttons
    matrix_buttons_layout_ = new QHBoxLayout();
    clear_matrix_btn_ = new QPushButton("Clear");
    compute_matrix_btn_ = new QPushButton("Compute");
    matrix_buttons_layout_->addWidget(clear_matrix_btn_);
    matrix_buttons_layout_->addWidget(compute_matrix_btn_);
    matrix_buttons_layout_->addStretch();
    
    matrix_input_layout_->addWidget(size_group_);
    matrix_input_layout_->addWidget(matrix_scroll_);
    matrix_input_layout_->addLayout(matrix_buttons_layout_);
}

void MatrixInputWidget::setupOperationsTab()
{
    operations_tab_ = new QWidget();
    operations_layout_ = new QVBoxLayout(operations_tab_);
    
    operation_group_ = new QGroupBox("Select Operation");
    operation_layout_ = new QVBoxLayout(operation_group_);
    operation_combo_ = new QComboBox();
    operation_combo_->addItems({
        "Transpose",
        "Inverse",
        "Determinant",
        "Trace"
    });
    operation_description_ = new QLabel("Select an operation to perform on the matrix.");
    operation_description_->setWordWrap(true);
    
    operation_layout_->addWidget(operation_combo_);
    operation_layout_->addWidget(operation_description_);
    
    parameters_group_ = new QGroupBox("Parameters");
    parameters_layout_ = new QVBoxLayout(parameters_group_);
    parameters_label_ = new QLabel("Operation-specific parameters:");
    parameters_edit_ = new QTextEdit();
    parameters_edit_->setMaximumHeight(100);
    
    parameters_layout_->addWidget(parameters_label_);
    parameters_layout_->addWidget(parameters_edit_);
    
    operations_layout_->addWidget(operation_group_);
    operations_layout_->addWidget(parameters_group_);
    operations_layout_->addStretch();
}

void MatrixInputWidget::setupVariablesTab()
{
    variables_tab_ = new QWidget();
    variables_layout_ = new QVBoxLayout(variables_tab_);
    
    variable_list_group_ = new QGroupBox("Variable List");
    variable_list_layout_ = new QVBoxLayout(variable_list_group_);
    variable_table_ = new QTableWidget(0, 3);
    variable_table_->setHorizontalHeaderLabels({"Name", "Value", "Type"});
    variable_table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    
    variable_list_layout_->addWidget(variable_table_);
    
    variable_buttons_layout_ = new QHBoxLayout();
    add_variable_btn_ = new QPushButton("Add Variable");
    remove_variable_btn_ = new QPushButton("Remove Variable");
    variable_buttons_layout_->addWidget(add_variable_btn_);
    variable_buttons_layout_->addWidget(remove_variable_btn_);
    variable_buttons_layout_->addStretch();
    
    variables_layout_->addWidget(variable_list_group_);
    variables_layout_->addLayout(variable_buttons_layout_);
}

void MatrixInputWidget::setupPresetsTab()
{
    presets_tab_ = new QWidget();
    presets_layout_ = new QVBoxLayout(presets_tab_);
    
    preset_combo_ = new QComboBox();
    preset_combo_->addItems({
        "Identity Matrix",
        "Zero Matrix",
        "Rotation Matrix Z"
    });
    preset_description_ = new QLabel("Select a preset matrix.");
    preset_description_->setWordWrap(true);
    apply_preset_btn_ = new QPushButton("Apply Preset");
    
    presets_layout_->addWidget(preset_combo_);
    presets_layout_->addWidget(preset_description_);
    presets_layout_->addWidget(apply_preset_btn_);
    presets_layout_->addStretch();
}

void MatrixInputWidget::connectSignals()
{
    connect(rows_spin_, QOverload<int>::of(&QSpinBox::valueChanged), this, &MatrixInputWidget::onMatrixSizeChanged);
    connect(cols_spin_, QOverload<int>::of(&QSpinBox::valueChanged), this, &MatrixInputWidget::onMatrixSizeChanged);
    connect(operation_combo_, &QComboBox::currentTextChanged, this, &MatrixInputWidget::onOperationSelected);
    connect(compute_matrix_btn_, &QPushButton::clicked, this, &MatrixInputWidget::onComputeMatrix);
    connect(clear_matrix_btn_, &QPushButton::clicked, this, &MatrixInputWidget::onClearMatrix);
    connect(add_variable_btn_, &QPushButton::clicked, this, &MatrixInputWidget::onAddVariable);
    connect(remove_variable_btn_, &QPushButton::clicked, this, &MatrixInputWidget::onRemoveVariable);
    connect(apply_preset_btn_, &QPushButton::clicked, this, &MatrixInputWidget::onPresetMatrixSelected);
    connect(matrix_table_, &QTableWidget::cellChanged, this, &MatrixInputWidget::onCellChanged);
}

void MatrixInputWidget::onMatrixSizeChanged()
{
    int rows = rows_spin_->value();
    int cols = cols_spin_->value();
    
    matrix_table_->setRowCount(rows);
    matrix_table_->setColumnCount(cols);
    
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (!matrix_table_->item(i, j)) {
                auto item = new QTableWidgetItem("0");
                matrix_table_->setItem(i, j, item);
            }
        }
    }
}

void MatrixInputWidget::onOperationSelected()
{
    current_operation_ = operation_combo_->currentText();
    QString description;
    
    if (current_operation_ == "Transpose") {
        description = "Computes the transpose of the matrix (rows become columns).";
    } else if (current_operation_ == "Inverse") {
        description = "Computes the inverse of the matrix. Matrix must be square and non-singular.";
    } else if (current_operation_ == "Determinant") {
        description = "Computes the determinant of the matrix. Matrix must be square.";
    } else if (current_operation_ == "Trace") {
        description = "Computes the trace (sum of diagonal elements) of the matrix.";
    } else {
        description = "Select an operation to perform on the matrix.";
    }
    
    operation_description_->setText(description);
}

void MatrixInputWidget::onComputeMatrix()
{
    try {
        SymbolicCore::SymbolicMatrix matrix = createMatrixFromTable();
        SymbolicCore::SymbolicMatrix result = performOperation(matrix);
        
        updateResultDisplay(result);
        emit matrixComputed(result);
        emit operationCompleted(current_operation_, result);
        
    } catch (const std::exception& e) {
        emit errorOccurred(QString("Error computing matrix: %1").arg(e.what()));
    }
}

void MatrixInputWidget::onClearMatrix()
{
    matrix_table_->clearContents();
    for (int i = 0; i < matrix_table_->rowCount(); ++i) {
        for (int j = 0; j < matrix_table_->columnCount(); ++j) {
            auto item = new QTableWidgetItem("0");
            matrix_table_->setItem(i, j, item);
        }
    }
}

void MatrixInputWidget::onAddVariable()
{
    QString name = "var" + QString::number(variables_.size() + 1);
    auto scalar = SymbolicCore::SymbolicScalar::variable(name.toStdString());
    
    variables_.push_back({name, scalar});
    updateVariableList();
}

void MatrixInputWidget::onRemoveVariable()
{
    int currentRow = variable_table_->currentRow();
    if (currentRow >= 0 && currentRow < static_cast<int>(variables_.size())) {
        variables_.erase(variables_.begin() + currentRow);
        updateVariableList();
    }
}

void MatrixInputWidget::onPresetMatrixSelected()
{
    QString preset = preset_combo_->currentText();
    
    if (preset == "Identity Matrix") {
        int size = rows_spin_->value();
        for (int i = 0; i < size; ++i) {
            for (int j = 0; j < size; ++j) {
                QString value = (i == j) ? "1" : "0";
                matrix_table_->setItem(i, j, new QTableWidgetItem(value));
            }
        }
    } else if (preset == "Zero Matrix") {
        onClearMatrix();
    }
}

void MatrixInputWidget::onCellChanged(int row, int col)
{
    auto item = matrix_table_->item(row, col);
    if (item) {
        QString text = item->text().trimmed();
        QRegularExpression validPattern("^[a-zA-Z0-9_+\\-*/().,\\s]+$");
        if (!text.isEmpty() && !validPattern.match(text).hasMatch()) {
            item->setText("0");
        }
    }
}

void MatrixInputWidget::onVariableValueChanged()
{
    updateVariableList();
}

void MatrixInputWidget::updateMatrixTable()
{
    // Implementation for updating matrix table based on current state
}

void MatrixInputWidget::updateVariableList()
{
    variable_table_->setRowCount(static_cast<int>(variables_.size()));
    
    for (size_t i = 0; i < variables_.size(); ++i) {
        const auto& var = variables_[i];
        
        auto nameItem = new QTableWidgetItem(var.first);
        auto valueItem = new QTableWidgetItem(QString::fromStdString(var.second.toString()));
        auto typeItem = new QTableWidgetItem("Variable");
        
        variable_table_->setItem(static_cast<int>(i), 0, nameItem);
        variable_table_->setItem(static_cast<int>(i), 1, valueItem);
        variable_table_->setItem(static_cast<int>(i), 2, typeItem);
    }
}

SymbolicCore::SymbolicMatrix MatrixInputWidget::createMatrixFromTable() const
{
    int rows = matrix_table_->rowCount();
    int cols = matrix_table_->columnCount();
    
    SymbolicCore::SymbolicMatrix matrix(rows, cols);
    
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            auto item = matrix_table_->item(i, j);
            QString text = item ? item->text().trimmed() : "0";
            
            if (text.isEmpty()) {
                text = "0";
            }
            
            try {
                auto scalar = SymbolicCore::SymbolicScalar::parse(text.toStdString());
                matrix(i, j) = scalar;
            } catch (...) {
                bool ok;
                double value = text.toDouble(&ok);
                if (ok) {
                    matrix(i, j) = SymbolicCore::SymbolicScalar::constant(value);
                } else {
                    matrix(i, j) = SymbolicCore::SymbolicScalar::variable(text.toStdString());
                }
            }
        }
    }
    
    return matrix;
}

SymbolicCore::SymbolicMatrix MatrixInputWidget::performOperation(const SymbolicCore::SymbolicMatrix& matrix) const
{
    if (current_operation_ == "Transpose") {
        return matrix.transpose();
    } else if (current_operation_ == "Inverse") {
        return matrix.inverse();
    } else if (current_operation_ == "Determinant") {
        auto det = matrix.determinant();
        return SymbolicCore::SymbolicMatrix(1, 1, det);
    } else if (current_operation_ == "Trace") {
        SymbolicCore::SymbolicScalar trace(0.0);
        for (size_t i = 0; i < std::min(matrix.rows(), matrix.cols()); ++i) {
            trace = trace + matrix.get(i, i);
        }
        return SymbolicCore::SymbolicMatrix(1, 1, trace);
    }
    
    return matrix;
}

void MatrixInputWidget::updateResultDisplay(const SymbolicCore::SymbolicMatrix& result)
{
    // This method can be used to update any internal result display if needed
}

void MatrixInputWidget::clear()
{
    onClearMatrix();
    variables_.clear();
    updateVariableList();
}

SymbolicCore::SymbolicMatrix MatrixInputWidget::getCurrentMatrix() const
{
    return createMatrixFromTable();
}

void MatrixInputWidget::setMatrix(const SymbolicCore::SymbolicMatrix& matrix)
{
    rows_spin_->setValue(static_cast<int>(matrix.rows()));
    cols_spin_->setValue(static_cast<int>(matrix.cols()));
    
    onMatrixSizeChanged();
    
    for (size_t i = 0; i < matrix.rows(); ++i) {
        for (size_t j = 0; j < matrix.cols(); ++j) {
            auto item = new QTableWidgetItem(QString::fromStdString(matrix.get(i, j).toString()));
            matrix_table_->setItem(static_cast<int>(i), static_cast<int>(j), item);
        }
    }
}

void MatrixInputWidget::addVariable(const QString& name, const SymbolicCore::SymbolicScalar& value)
{
    variables_.push_back({name, value});
    updateVariableList();
}

void MatrixInputWidget::removeVariable(const QString& name)
{
    variables_.erase(
        std::remove_if(variables_.begin(), variables_.end(),
                      [&name](const auto& var) { return var.first == name; }),
        variables_.end()
    );
    updateVariableList();
}
