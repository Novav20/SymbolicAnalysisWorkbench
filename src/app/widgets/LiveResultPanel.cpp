#include "LiveResultPanel.h"
#include "SymbolicCore/SymbolicMatrix.h"
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QApplication>
#include <QtGui/QClipboard>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <sstream>

LiveResultPanel::LiveResultPanel(QWidget *parent)
    : QWidget(parent)
    , main_layout_(nullptr), status_layout_(nullptr), status_label_(nullptr)
    , preview_badge_(nullptr), render_group_(nullptr), render_layout_(nullptr)
    , latex_render_(nullptr), latex_code_group_(nullptr), latex_code_layout_(nullptr)
    , latex_code_display_(nullptr), latex_code_buttons_(nullptr), copy_latex_btn_(nullptr)
    , toggle_latex_btn_(nullptr), export_layout_(nullptr), export_label_(nullptr)
    , export_dropdown_btn_(nullptr), export_menu_(nullptr), current_matrix_(nullptr)
    , is_preview_(false), latex_code_visible_(false)
{
    setupUI();
    connectSignals();
}

LiveResultPanel::~LiveResultPanel() = default;

void LiveResultPanel::setupUI()
{
    main_layout_ = new QVBoxLayout(this);
    
    // Status bar
    status_layout_ = new QHBoxLayout();
    status_label_ = new QLabel("Ready");
    status_label_->setStyleSheet("QLabel { color: #666; font-size: 10pt; }");
    preview_badge_ = new QLabel("[PREVIEW]");
    preview_badge_->setStyleSheet("QLabel { color: #ff6600; font-weight: bold; font-size: 10pt; }");
    preview_badge_->setVisible(false);
    
    status_layout_->addWidget(status_label_);
    status_layout_->addWidget(preview_badge_);
    status_layout_->addStretch();
    
    // LaTeX render area
    render_group_ = new QGroupBox("LaTeX Output");
    render_layout_ = new QVBoxLayout(render_group_);
    latex_render_ = new LatexOutputWidget();
    latex_render_->setMinimumHeight(300);
    latex_render_->setStyleSheet("QWidget { background-color: white; border: 1px solid #ccc; }");
    render_layout_->addWidget(latex_render_);
    
    // LaTeX code section (initially hidden)
    latex_code_group_ = new QGroupBox("LaTeX Source Code");
    latex_code_layout_ = new QVBoxLayout(latex_code_group_);
    latex_code_display_ = new QTextEdit();
    latex_code_display_->setReadOnly(true);
    latex_code_display_->setMaximumHeight(150);
    latex_code_display_->setFont(QFont("Courier New", 9));
    
    latex_code_buttons_ = new QHBoxLayout();
    copy_latex_btn_ = new QPushButton("Copy");
    toggle_latex_btn_ = new QPushButton("Hide");
    latex_code_buttons_->addWidget(copy_latex_btn_);
    latex_code_buttons_->addWidget(toggle_latex_btn_);
    latex_code_buttons_->addStretch();
    
    latex_code_layout_->addWidget(latex_code_display_);
    latex_code_layout_->addLayout(latex_code_buttons_);
    latex_code_group_->setVisible(false);
    
    // Export controls
    export_layout_ = new QHBoxLayout();
    export_label_ = new QLabel("Export:");
    export_dropdown_btn_ = new QToolButton();
    export_dropdown_btn_->setText("Export As...");
    export_dropdown_btn_->setPopupMode(QToolButton::InstantPopup);
    
    export_menu_ = new QMenu(this);
    export_menu_->addAction("LaTeX (.tex)", this, &LiveResultPanel::onExportLatex);
    export_menu_->addAction("Python (.py)", this, &LiveResultPanel::onExportPython);
    export_menu_->addAction("MATLAB (.m)", this, &LiveResultPanel::onExportMatlab);
    export_menu_->addAction("Mathematica (.nb)", this, &LiveResultPanel::onExportMathematica);
    export_menu_->addAction("JSON (.json)", this, &LiveResultPanel::onExportJSON);
    
    export_dropdown_btn_->setMenu(export_menu_);
    
    QPushButton* show_latex_code_btn = new QPushButton("Show LaTeX Code");
    connect(show_latex_code_btn, &QPushButton::clicked, [this]() {
        latex_code_visible_ = !latex_code_visible_;
        latex_code_group_->setVisible(latex_code_visible_);
        toggle_latex_btn_->setText(latex_code_visible_ ? "Hide" : "Show");
    });
    
    export_layout_->addWidget(export_label_);
    export_layout_->addWidget(export_dropdown_btn_);
    export_layout_->addWidget(show_latex_code_btn);
    export_layout_->addStretch();
    
    // Add to main layout
    main_layout_->addLayout(status_layout_);
    main_layout_->addWidget(render_group_);
    main_layout_->addWidget(latex_code_group_);
    main_layout_->addLayout(export_layout_);
}

void LiveResultPanel::connectSignals()
{
    connect(copy_latex_btn_, &QPushButton::clicked, this, &LiveResultPanel::onCopyLatexCode);
    connect(toggle_latex_btn_, &QPushButton::clicked, this, &LiveResultPanel::onToggleLatexCode);
}

void LiveResultPanel::displayMatrix(const SymbolicCore::SymbolicMatrix& matrix)
{
    current_matrix_ = std::make_shared<SymbolicCore::SymbolicMatrix>(matrix);
    is_preview_ = false;
    preview_badge_->setVisible(false);
    updateLatexRender();
    updateLatexCodeDisplay();
    setStatus("Ready");
}

void LiveResultPanel::displayPreview(const SymbolicCore::SymbolicMatrix& matrix)
{
    current_matrix_ = std::make_shared<SymbolicCore::SymbolicMatrix>(matrix);
    is_preview_ = true;
    preview_badge_->setVisible(true);
    updateLatexRender();
    updateLatexCodeDisplay();
    setStatus("Preview");
}

void LiveResultPanel::clear()
{
    current_matrix_.reset();
    latex_render_->clear();
    latex_code_display_->clear();
    is_preview_ = false;
    preview_badge_->setVisible(false);
    setStatus("Ready");
}

void LiveResultPanel::setStatus(const QString& status)
{
    status_label_->setText(status);
}

SymbolicCore::SymbolicMatrix LiveResultPanel::getCurrentMatrix() const
{
    if (current_matrix_) {
        return *current_matrix_;
    }
    return SymbolicCore::SymbolicMatrix(0, 0);
}

void LiveResultPanel::updateLatexRender()
{
    if (!current_matrix_) return;
    
    try {
        QString latex = formatMatrixAsLatex(*current_matrix_);
        std::wstring wlatex = latex.toStdWString();
        latex_render_->setLatex(wlatex);
    } catch (const std::exception& e) {
        latex_render_->clear();
        emit errorOccurred(QString("LaTeX render error: %1").arg(e.what()));
    }
}

void LiveResultPanel::updateLatexCodeDisplay()
{
    if (!current_matrix_) return;
    latex_code_display_->setPlainText(formatMatrixAsLatex(*current_matrix_));
}

QString LiveResultPanel::formatMatrixAsLatex(const SymbolicCore::SymbolicMatrix& matrix) const
{
    return QString::fromStdString(matrix.toLatex());
}

QString LiveResultPanel::formatMatrixAsPython(const SymbolicCore::SymbolicMatrix& matrix) const
{
    std::ostringstream oss;
    oss << "import sympy as sp\n\n";
    oss << "# Matrix: " << matrix.rows() << "x" << matrix.cols() << "\n";
    oss << "matrix = sp.Matrix([\n";
    
    for (size_t i = 0; i < matrix.rows(); ++i) {
        oss << "    [";
        for (size_t j = 0; j < matrix.cols(); ++j) {
            oss << "sp.sympify('" << matrix.get(i, j).toString() << "')";
            if (j < matrix.cols() - 1) oss << ", ";
        }
        oss << "]";
        if (i < matrix.rows() - 1) oss << ",";
        oss << "\n";
    }
    oss << "])\n";
    
    return QString::fromStdString(oss.str());
}

QString LiveResultPanel::formatMatrixAsMatlab(const SymbolicCore::SymbolicMatrix& matrix) const
{
    std::ostringstream oss;
    oss << "% Symbolic Matrix: " << matrix.rows() << "x" << matrix.cols() << "\n";
    oss << "syms ";
    
    // Collect all variables
    std::set<std::string> vars;
    for (size_t i = 0; i < matrix.rows(); ++i) {
        for (size_t j = 0; j < matrix.cols(); ++j) {
            auto element_vars = matrix.get(i, j).getVariables();
            vars.insert(element_vars.begin(), element_vars.end());
        }
    }
    
    for (const auto& var : vars) {
        oss << var << " ";
    }
    oss << "\n\n";
    
    oss << "M = [\n";
    for (size_t i = 0; i < matrix.rows(); ++i) {
        oss << "    ";
        for (size_t j = 0; j < matrix.cols(); ++j) {
            oss << matrix.get(i, j).toString();
            if (j < matrix.cols() - 1) oss << ", ";
        }
        if (i < matrix.rows() - 1) oss << ";";
        oss << "\n";
    }
    oss << "];\n";
    
    return QString::fromStdString(oss.str());
}

QString LiveResultPanel::formatMatrixAsMathematica(const SymbolicCore::SymbolicMatrix& matrix) const
{
    std::ostringstream oss;
    oss << "(* Symbolic Matrix: " << matrix.rows() << "x" << matrix.cols() << " *)\n";
    oss << "matrix = {\n";
    
    for (size_t i = 0; i < matrix.rows(); ++i) {
        oss << "  {";
        for (size_t j = 0; j < matrix.cols(); ++j) {
            std::string elem = matrix.get(i, j).toString();
            // Simple conversion (SymEngine syntax is close to Mathematica)
            oss << elem;
            if (j < matrix.cols() - 1) oss << ", ";
        }
        oss << "}";
        if (i < matrix.rows() - 1) oss << ",";
        oss << "\n";
    }
    oss << "}\n";
    
    return QString::fromStdString(oss.str());
}

QString LiveResultPanel::formatMatrixAsJSON(const SymbolicCore::SymbolicMatrix& matrix) const
{
    QJsonObject root;
    root["rows"] = static_cast<int>(matrix.rows());
    root["cols"] = static_cast<int>(matrix.cols());
    
    QJsonArray elements;
    for (size_t i = 0; i < matrix.rows(); ++i) {
        QJsonArray row;
        for (size_t j = 0; j < matrix.cols(); ++j) {
            row.append(QString::fromStdString(matrix.get(i, j).toString()));
        }
        elements.append(row);
    }
    root["elements"] = elements;
    
    QJsonDocument doc(root);
    return QString::fromUtf8(doc.toJson(QJsonDocument::Indented));
}

void LiveResultPanel::saveToFile(const QString& content, const QString& filter, const QString& defaultSuffix)
{
    QString filename = QFileDialog::getSaveFileName(this, "Export Matrix", "", filter);
    if (filename.isEmpty()) return;
    
    if (!filename.endsWith(defaultSuffix)) {
        filename += defaultSuffix;
    }
    
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Export Error", "Could not open file for writing.");
        return;
    }
    
    QTextStream out(&file);
    out << content;
    file.close();
    
    QMessageBox::information(this, "Export Successful", "Matrix exported to: " + filename);
}

void LiveResultPanel::onCopyLatexCode()
{
    if (!current_matrix_) return;
    QApplication::clipboard()->setText(latex_code_display_->toPlainText());
    setStatus("LaTeX code copied to clipboard");
}

void LiveResultPanel::onToggleLatexCode()
{
    latex_code_visible_ = !latex_code_visible_;
    latex_code_group_->setVisible(latex_code_visible_);
    toggle_latex_btn_->setText(latex_code_visible_ ? "Hide" : "Show");
}

void LiveResultPanel::onExportLatex()
{
    if (!current_matrix_) return;
    QString content = formatMatrixAsLatex(*current_matrix_);
    saveToFile(content, "LaTeX Files (*.tex)", ".tex");
}

void LiveResultPanel::onExportPython()
{
    if (!current_matrix_) return;
    QString content = formatMatrixAsPython(*current_matrix_);
    saveToFile(content, "Python Files (*.py)", ".py");
}

void LiveResultPanel::onExportMatlab()
{
    if (!current_matrix_) return;
    QString content = formatMatrixAsMatlab(*current_matrix_);
    saveToFile(content, "MATLAB Files (*.m)", ".m");
}

void LiveResultPanel::onExportMathematica()
{
    if (!current_matrix_) return;
    QString content = formatMatrixAsMathematica(*current_matrix_);
    saveToFile(content, "Mathematica Files (*.nb)", ".nb");
}

void LiveResultPanel::onExportJSON()
{
    if (!current_matrix_) return;
    QString content = formatMatrixAsJSON(*current_matrix_);
    saveToFile(content, "JSON Files (*.json)", ".json");
}
