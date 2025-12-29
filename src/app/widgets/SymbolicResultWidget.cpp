#include "SymbolicResultWidget.h"
#include "SymbolicCore/SymbolicMatrix.h"
#include "SymbolicCore/SymbolicScalar.h"

#include <QtWidgets/QApplication>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>
#include <QtGui/QClipboard>
#include <QtCore/QMimeData>
#include <QtCore/QRegularExpression>
#include <QFile>
#include <QTextStream>
#include <set>

SymbolicResultWidget::SymbolicResultWidget(QWidget *parent)
    : QWidget(parent)
    , main_layout_(nullptr)
    , main_splitter_(nullptr)
    , tab_widget_(nullptr)
    , result_display_tab_(nullptr)
    , result_display_layout_(nullptr)
    , matrix_info_group_(nullptr)
    , matrix_info_layout_(nullptr)
    , matrix_size_label_(nullptr)
    , matrix_type_label_(nullptr)
    , matrix_properties_label_(nullptr)
    , result_display_(nullptr)
    , latex_tab_(nullptr)
    , latex_layout_(nullptr)
    , latex_render_widget_(nullptr)
    , latex_display_(nullptr)
    , copy_latex_btn_(nullptr)
    , numerical_tab_(nullptr)
    , numerical_layout_(nullptr)
    , evaluation_params_group_(nullptr)
    , evaluation_params_layout_(nullptr)
    , variable_values_label_(nullptr)
    , variable_values_edit_(nullptr)
    , evaluate_btn_(nullptr)
    , numerical_result_(nullptr)
    , export_tab_(nullptr)
    , export_layout_(nullptr)
    , export_format_group_(nullptr)
    , export_format_layout_(nullptr)
    , export_latex_btn_(nullptr)
    , export_python_btn_(nullptr)
    , export_mathematica_btn_(nullptr)
    , export_matlab_btn_(nullptr)
    , export_preview_(nullptr)
    , simplify_btn_(nullptr)
    , copy_to_clipboard_btn_(nullptr)
    , clear_btn_(nullptr)
    , is_dirty_(false)
{
    setupUI();
    connectSignals();
}

SymbolicResultWidget::~SymbolicResultWidget() = default;

void SymbolicResultWidget::setupUI()
{
    main_layout_ = new QVBoxLayout(this);
    
    main_splitter_ = new QSplitter(Qt::Vertical, this);
    tab_widget_ = new QTabWidget(this);
    
    setupResultDisplayTab();
    setupLatexTab();
    setupNumericalTab();
    setupExportTab();
    
    tab_widget_->addTab(result_display_tab_, "Mathematical");
    tab_widget_->addTab(latex_tab_, "LaTeX");
    tab_widget_->addTab(numerical_tab_, "Numerical");
    tab_widget_->addTab(export_tab_, "Export");
    
    main_splitter_->addWidget(tab_widget_);
    
    control_buttons_layout_ = new QHBoxLayout();
    simplify_btn_ = new QPushButton("Simplify", this);
    copy_to_clipboard_btn_ = new QPushButton("Copy to Clipboard", this);
    clear_btn_ = new QPushButton("Clear", this);
    
    control_buttons_layout_->addWidget(simplify_btn_);
    control_buttons_layout_->addWidget(copy_to_clipboard_btn_);
    control_buttons_layout_->addWidget(clear_btn_);
    control_buttons_layout_->addStretch();
    
    main_layout_->addWidget(main_splitter_);
    main_layout_->addLayout(control_buttons_layout_);
}

void SymbolicResultWidget::setupResultDisplayTab()
{
    result_display_tab_ = new QWidget();
    result_display_layout_ = new QVBoxLayout(result_display_tab_);
    
    matrix_info_group_ = new QGroupBox("Matrix Information");
    matrix_info_layout_ = new QVBoxLayout(matrix_info_group_);
    matrix_size_label_ = new QLabel("Size: N/A");
    matrix_type_label_ = new QLabel("Type: N/A");
    matrix_properties_label_ = new QLabel("Properties: None");
    
    matrix_info_layout_->addWidget(matrix_size_label_);
    matrix_info_layout_->addWidget(matrix_type_label_);
    matrix_info_layout_->addWidget(matrix_properties_label_);
    
    result_display_ = new QTextEdit();
    result_display_->setReadOnly(true);
    result_display_->setFont(QFont("Courier New", 10));
    
    result_display_layout_->addWidget(matrix_info_group_);
    result_display_layout_->addWidget(result_display_);
}

void SymbolicResultWidget::setupLatexTab()
{
    latex_tab_ = new QWidget();
    latex_layout_ = new QVBoxLayout(latex_tab_);
    
    latex_display_ = new QTextEdit();
    latex_display_->setReadOnly(true);
    latex_display_->setFont(QFont("Courier New", 10));
    latex_display_->setMaximumHeight(100); // Reduce height to make room for render
    
    latex_render_widget_ = new LatexOutputWidget();
    latex_render_widget_->setMinimumHeight(200);
    
    copy_latex_btn_ = new QPushButton("Copy LaTeX");
    
    latex_layout_->addWidget(latex_render_widget_);
    latex_layout_->addWidget(latex_display_);
    latex_layout_->addWidget(copy_latex_btn_);
}

void SymbolicResultWidget::setupNumericalTab()
{
    numerical_tab_ = new QWidget();
    numerical_layout_ = new QVBoxLayout(numerical_tab_);
    
    evaluation_params_group_ = new QGroupBox("Variable Values");
    evaluation_params_layout_ = new QVBoxLayout(evaluation_params_group_);
    variable_values_label_ = new QLabel("Enter variable values (format: var=value, one per line):");
    variable_values_edit_ = new QTextEdit();
    variable_values_edit_->setMaximumHeight(100);
    evaluate_btn_ = new QPushButton("Evaluate Numerically");
    
    evaluation_params_layout_->addWidget(variable_values_label_);
    evaluation_params_layout_->addWidget(variable_values_edit_);
    evaluation_params_layout_->addWidget(evaluate_btn_);
    
    numerical_result_ = new QTextEdit();
    numerical_result_->setReadOnly(true);
    numerical_result_->setFont(QFont("Courier New", 10));
    
    numerical_layout_->addWidget(evaluation_params_group_);
    numerical_layout_->addWidget(numerical_result_);
}

void SymbolicResultWidget::setupExportTab()
{
    export_tab_ = new QWidget();
    export_layout_ = new QVBoxLayout(export_tab_);
    
    export_format_group_ = new QGroupBox("Export Formats");
    export_format_layout_ = new QVBoxLayout(export_format_group_);
    export_latex_btn_ = new QPushButton("Export to LaTeX");
    export_python_btn_ = new QPushButton("Export to Python");
    export_mathematica_btn_ = new QPushButton("Export to Mathematica");
    export_matlab_btn_ = new QPushButton("Export to MATLAB");
    
    export_format_layout_->addWidget(export_latex_btn_);
    export_format_layout_->addWidget(export_python_btn_);
    export_format_layout_->addWidget(export_mathematica_btn_);
    export_format_layout_->addWidget(export_matlab_btn_);
    
    export_preview_ = new QTextEdit();
    export_preview_->setReadOnly(true);
    export_preview_->setFont(QFont("Courier New", 10));
    
    export_layout_->addWidget(export_format_group_);
    export_layout_->addWidget(export_preview_);
}

void SymbolicResultWidget::connectSignals()
{
    connect(tab_widget_, &QTabWidget::currentChanged, this, &SymbolicResultWidget::onTabChanged);
    
    connect(simplify_btn_, &QPushButton::clicked, this, &SymbolicResultWidget::onSimplifyExpression);
    connect(copy_to_clipboard_btn_, &QPushButton::clicked, this, &SymbolicResultWidget::onCopyToClipboard);
    connect(clear_btn_, &QPushButton::clicked, this, &SymbolicResultWidget::clear);
    
    connect(copy_latex_btn_, &QPushButton::clicked, this, &SymbolicResultWidget::onCopyToClipboard);
    
    connect(evaluate_btn_, &QPushButton::clicked, this, &SymbolicResultWidget::onEvaluateNumerically);
    
    connect(export_latex_btn_, &QPushButton::clicked, this, &SymbolicResultWidget::onExportLatex);
    connect(export_python_btn_, &QPushButton::clicked, this, &SymbolicResultWidget::onExportPython);
    connect(export_mathematica_btn_, &QPushButton::clicked, this, &SymbolicResultWidget::onExportMathematica);
    connect(export_matlab_btn_, &QPushButton::clicked, this, &SymbolicResultWidget::onExportMatlab);
}

void SymbolicResultWidget::displayMatrix(const SymbolicCore::SymbolicMatrix& matrix)
{
    current_matrix_ = std::make_shared<SymbolicCore::SymbolicMatrix>(matrix);
    is_dirty_ = true;
    updateAllDisplays();
    emit matrixUpdated(matrix);
}

void SymbolicResultWidget::clear()
{
    current_matrix_.reset();
    result_display_->clear();
    latex_display_->clear();
    numerical_result_->clear();
    export_preview_->clear();
    matrix_size_label_->setText("Size: N/A");
    matrix_type_label_->setText("Type: N/A");
    matrix_properties_label_->setText("Properties: None");
}

void SymbolicResultWidget::setVariableValues(const std::map<std::string, double>& values)
{
    QString text;
    for (const auto& [var, val] : values) {
        text += QString::fromStdString(var) + "=" + QString::number(val) + "\n";
    }
    variable_values_edit_->setText(text.trimmed());
}

SymbolicCore::SymbolicMatrix SymbolicResultWidget::getCurrentMatrix() const
{
    if (current_matrix_) {
        return *current_matrix_;
    }
    return SymbolicCore::SymbolicMatrix(0, 0);
}

void SymbolicResultWidget::onExportLatex()
{
    if (!current_matrix_) return;
    
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    "Export to LaTeX",
                                                    "symbolic_matrix.tex",
                                                    "LaTeX Files (*.tex);;All Files (*)");
    
    if (!fileName.isEmpty()) {
        QString latex_content = formatMatrixAsLatex(*current_matrix_);
        
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << latex_content;
            file.close();
            
            emit exportRequested("LaTeX", fileName);
        } else {
            emit errorOccurred("Failed to save LaTeX file");
        }
    }
}

void SymbolicResultWidget::onExportPython()
{
    if (!current_matrix_) return;
    
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    "Export to Python",
                                                    "symbolic_matrix.py",
                                                    "Python Files (*.py);;All Files (*)");
    
    if (!fileName.isEmpty()) {
        QString python_content = formatMatrixAsPython(*current_matrix_);
        
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << python_content;
            file.close();
            
            emit exportRequested("Python", fileName);
        } else {
            emit errorOccurred("Failed to save Python file");
        }
    }
}

void SymbolicResultWidget::onExportMathematica()
{
    if (!current_matrix_) return;
    
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    "Export to Mathematica",
                                                    "symbolic_matrix.m",
                                                    "Mathematica Files (*.m);;All Files (*)");
    
    if (!fileName.isEmpty()) {
        QString mathematica_content = formatMatrixAsMathematica(*current_matrix_);
        
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << mathematica_content;
            file.close();
            
            emit exportRequested("Mathematica", fileName);
        } else {
            emit errorOccurred("Failed to save Mathematica file");
        }
    }
}

void SymbolicResultWidget::onExportMatlab()
{
    if (!current_matrix_) return;
    
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    "Export to MATLAB",
                                                    "symbolic_matrix.m",
                                                    "MATLAB Files (*.m);;All Files (*)");
    
    if (!fileName.isEmpty()) {
        QString matlab_content = formatMatrixAsMatlab(*current_matrix_);
        
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << matlab_content;
            file.close();
            
            emit exportRequested("MATLAB", fileName);
        } else {
            emit errorOccurred("Failed to save MATLAB file");
        }
    }
}

void SymbolicResultWidget::onEvaluateNumerically()
{
    if (!current_matrix_) return;
    
    try {
        auto numeric_result = evaluateMatrix(*current_matrix_);
        
        QString result_text = "Numerical Evaluation Result:\n\n";
        for (const auto& row : numeric_result) {
            for (double val : row) {
                result_text += QString("  %1").arg(val, 10, 'f', 6);
            }
            result_text += "\n";
        }
        
        numerical_result_->setText(result_text);
        
    } catch (const std::exception& e) {
        emit errorOccurred(QString("Numerical evaluation failed: %1").arg(e.what()));
    }
}

void SymbolicResultWidget::onSimplifyExpression()
{
    if (!current_matrix_) return;
    
    try {
        SymbolicCore::SymbolicMatrix simplified = *current_matrix_;
        simplified.simplify();
        displayMatrix(simplified);
        
    } catch (const std::exception& e) {
        emit errorOccurred(QString("Simplification failed: %1").arg(e.what()));
    }
}

void SymbolicResultWidget::onCopyToClipboard()
{
    QString content;
    
    switch (tab_widget_->currentIndex()) {
    case 0: // Mathematical
        content = result_display_->toPlainText();
        break;
    case 1: // LaTeX
        content = latex_display_->toPlainText();
        break;
    case 2: // Numerical
        content = numerical_result_->toPlainText();
        break;
    case 3: // Export
        content = export_preview_->toPlainText();
        break;
    default:
        return;
    }
    
    QApplication::clipboard()->setText(content);
}

void SymbolicResultWidget::onTabChanged(int index)
{
    if (is_dirty_ && current_matrix_) {
        updateAllDisplays();
        is_dirty_ = false;
    }
}

void SymbolicResultWidget::updateAllDisplays()
{
    if (!current_matrix_) return;
    
    updateMathematicalDisplay();
    updateLatexDisplay();
    updateNumericalDisplay();
    updateExportPreview();
}

void SymbolicResultWidget::updateMathematicalDisplay()
{
    if (!current_matrix_) return;
    
    // Update matrix information
    matrix_size_label_->setText(QString("Size: %1x%2")
                              .arg(current_matrix_->rows())
                              .arg(current_matrix_->cols()));
    
    QString type = "General Matrix";
    bool isSquare = (current_matrix_->rows() == current_matrix_->cols());
    if (isSquare) {
        type = "Square Matrix";
        if (current_matrix_->rows() == 2) type = "2x2 Matrix";
        else if (current_matrix_->rows() == 3) type = "3x3 Matrix";
        else if (current_matrix_->rows() == 4) type = "4x4 Matrix";
    }
    matrix_type_label_->setText("Type: " + type);
    
    // Update properties
    QString properties;
    if (isSquare) {
        try {
            auto det = current_matrix_->determinant();
            properties += QString("det = %1").arg(QString::fromStdString(det.toString()));
        } catch (...) {
            properties += "Unable to compute determinant";
        }
    }
    
    if (current_matrix_->isSymmetric()) {
        properties += (properties.isEmpty() ? "" : QString(", ")) + "Symmetric";
    }
    
    matrix_properties_label_->setText("Properties: " + (properties.isEmpty() ? "None" : properties));
    
    // Update matrix display
    result_display_->setText(formatMatrixAsText(*current_matrix_));
}

void SymbolicResultWidget::updateLatexDisplay()
{
    if (!current_matrix_) return;
    
    QString latex = formatMatrixAsLatex(*current_matrix_);
    latex_display_->setText(latex);
    // Let QString handle the conversion to wstring properly
    latex_render_widget_->setLatex(latex.toStdWString());
}

void SymbolicResultWidget::updateNumericalDisplay()
{
    if (!current_matrix_) return;
    
    numerical_result_->clear();
}

void SymbolicResultWidget::updateExportPreview()
{
    if (!current_matrix_) return;
    
    // Show Python format as default preview
    export_preview_->setText(formatMatrixAsPython(*current_matrix_));
}

QString SymbolicResultWidget::formatMatrixAsText(const SymbolicCore::SymbolicMatrix& matrix) const
{
    QString content;
    content += QString("Rows: %1\n").arg(matrix.rows());
    content += QString("Columns: %1\n\n").arg(matrix.cols());
    
    for (size_t i = 0; i < matrix.rows(); ++i) {
        for (size_t j = 0; j < matrix.cols(); ++j) {
            auto element = matrix.get(i, j);
            content += QString::fromStdString(element.toString());
            if (j < matrix.cols() - 1) {
                content += "  ";
            }
        }
        content += "\n";
    }
    
    return content;
}

QString SymbolicResultWidget::formatMatrixAsLatex(const SymbolicCore::SymbolicMatrix& matrix) const
{
    QString latex = "\\begin{bmatrix}\n";
    for (size_t i = 0; i < matrix.rows(); ++i) {
        for (size_t j = 0; j < matrix.cols(); ++j) {
            auto element = matrix.get(i, j);
            latex += QString::fromStdString(element.toLatex());
            if (j < matrix.cols() - 1) {
                latex += " & ";
            }
        }
        if (i < matrix.rows() - 1) {
            latex += " \\\\";
        }
        latex += "\n";
    }
    latex += "\\end{bmatrix}";
    return latex;
}

QString SymbolicResultWidget::formatMatrixAsPython(const SymbolicCore::SymbolicMatrix& matrix) const
{
    QString result = "import sympy as sp\n\n";
    
    // Declare variables
    std::set<std::string> variables;
    for (size_t i = 0; i < matrix.rows(); ++i) {
        for (size_t j = 0; j < matrix.cols(); ++j) {
            auto vars = matrix.get(i, j).getVariables();
            variables.insert(vars.begin(), vars.end());
        }
    }
    
    for (const auto& var : variables) {
        result += QString("%1 = sp.symbols('%1')\n").arg(QString::fromStdString(var));
    }
    
    result += "\n";
    result += "# Create the matrix\n";
    result += "matrix = sp.Matrix([";
    
    for (size_t i = 0; i < matrix.rows(); ++i) {
        if (i > 0) result += ",\n                   ";
        result += "[";
        for (size_t j = 0; j < matrix.cols(); ++j) {
            if (j > 0) result += ", ";
            result += QString::fromStdString(matrix.get(i, j).toString());
        }
        result += "]";
    }
    
    result += "])\n\n";
    result += "# Display the matrix\n";
    result += "print(matrix)\n";
    
    return result;
}

QString SymbolicResultWidget::formatMatrixAsMathematica(const SymbolicCore::SymbolicMatrix& matrix) const
{
    QString result = "(* Mathematica symbolic matrix *)\n\n";
    
    // Declare symbols
    std::set<std::string> variables;
    for (size_t i = 0; i < matrix.rows(); ++i) {
        for (size_t j = 0; j < matrix.cols(); ++j) {
            auto vars = matrix.get(i, j).getVariables();
            variables.insert(vars.begin(), vars.end());
        }
    }
    
    for (const auto& var : variables) {
        result += QString("%1 = Symbol[\"%1\"];\n").arg(QString::fromStdString(var));
    }
    
    result += "\n";
    result += "matrix = {";
    
    for (size_t i = 0; i < matrix.rows(); ++i) {
        if (i > 0) result += ",\n          ";
        result += "{";
        for (size_t j = 0; j < matrix.cols(); ++j) {
            if (j > 0) result += ", ";
            result += QString::fromStdString(matrix.get(i, j).toString());
        }
        result += "}";
    }
    
    result += "};\n\n";
    result += "MatrixForm[matrix]\n";
    
    return result;
}

QString SymbolicResultWidget::formatMatrixAsMatlab(const SymbolicCore::SymbolicMatrix& matrix) const
{
    QString result = "% MATLAB symbolic matrix\n\n";
    result += "syms ";
    
    // Declare symbols
    std::set<std::string> variables;
    for (size_t i = 0; i < matrix.rows(); ++i) {
        for (size_t j = 0; j < matrix.cols(); ++j) {
            auto vars = matrix.get(i, j).getVariables();
            variables.insert(vars.begin(), vars.end());
        }
    }
    
    bool first = true;
    for (const auto& var : variables) {
        if (!first) result += " ";
        result += QString::fromStdString(var);
        first = false;
    }
    
    result += ";\n\n";
    result += "% Create the matrix\n";
    result += "matrix = [";
    
    for (size_t i = 0; i < matrix.rows(); ++i) {
        if (i > 0) result += ";\n          ";
        for (size_t j = 0; j < matrix.cols(); ++j) {
            if (j > 0) result += ", ";
            result += QString::fromStdString(matrix.get(i, j).toString());
        }
    }
    
    result += "];\n\n";
    result += "disp(matrix);\n";
    
    return result;
}

std::vector<std::vector<double>> SymbolicResultWidget::evaluateMatrix(const SymbolicCore::SymbolicMatrix& matrix) const
{
    // Parse variable values from text
    std::map<std::string, double> values;
    QString text = variable_values_edit_->toPlainText().trimmed();
    
    if (!text.isEmpty()) {
        QStringList lines = text.split('\n');
        for (const QString& line : lines) {
            QStringList parts = line.split('=');
            if (parts.size() == 2) {
                QString var_name = parts[0].trimmed();
                bool ok;
                double value = parts[1].trimmed().toDouble(&ok);
                if (ok) {
                    values[var_name.toStdString()] = value;
                }
            }
        }
    }
    
    return matrix.evaluate(values);
}
