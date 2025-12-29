#pragma once

#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QToolButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QMenu>
#include <memory>
#include "LatexOutputWidget.h"

namespace SymbolicCore {
    class SymbolicMatrix;
}

/**
 * @class LiveResultPanel
 * @brief Real-time LaTeX rendering panel for symbolic results
 * 
 * This widget provides instant visual feedback for symbolic computations:
 * - Live LaTeX rendering via MicroTeX
 * - Collapsible LaTeX source code display
 * - Export dropdown (LaTeX, Python, MATLAB, Mathematica, JSON)
 * - Status indicator (Computing/Ready)
 */
class LiveResultPanel : public QWidget {
    Q_OBJECT

public:
    explicit LiveResultPanel(QWidget *parent = nullptr);
    ~LiveResultPanel();

    void displayMatrix(const SymbolicCore::SymbolicMatrix& matrix);
    void displayPreview(const SymbolicCore::SymbolicMatrix& matrix);
    void clear();
    void setStatus(const QString& status);
    SymbolicCore::SymbolicMatrix getCurrentMatrix() const;

signals:
    void exportRequested(const QString& format, const QString& content);
    void errorOccurred(const QString& error);

private slots:
    void onCopyLatexCode();
    void onToggleLatexCode();
    void onExportLatex();
    void onExportPython();
    void onExportMatlab();
    void onExportMathematica();
    void onExportJSON();

private:
    void setupUI();
    void connectSignals();
    void updateLatexRender();
    void updateLatexCodeDisplay();
    QString formatMatrixAsLatex(const SymbolicCore::SymbolicMatrix& matrix) const;
    QString formatMatrixAsPython(const SymbolicCore::SymbolicMatrix& matrix) const;
    QString formatMatrixAsMatlab(const SymbolicCore::SymbolicMatrix& matrix) const;
    QString formatMatrixAsMathematica(const SymbolicCore::SymbolicMatrix& matrix) const;
    QString formatMatrixAsJSON(const SymbolicCore::SymbolicMatrix& matrix) const;
    void saveToFile(const QString& content, const QString& filter, const QString& defaultSuffix);

    // Main layout
    QVBoxLayout* main_layout_;
    
    // Status bar
    QHBoxLayout* status_layout_;
    QLabel* status_label_;
    QLabel* preview_badge_;
    
    // LaTeX render area
    QGroupBox* render_group_;
    QVBoxLayout* render_layout_;
    LatexOutputWidget* latex_render_;
    
    // LaTeX code section (collapsible)
    QGroupBox* latex_code_group_;
    QVBoxLayout* latex_code_layout_;
    QTextEdit* latex_code_display_;
    QHBoxLayout* latex_code_buttons_;
    QPushButton* copy_latex_btn_;
    QPushButton* toggle_latex_btn_;
    
    // Export controls
    QHBoxLayout* export_layout_;
    QLabel* export_label_;
    QToolButton* export_dropdown_btn_;
    QMenu* export_menu_;
    
    // Current result
    std::shared_ptr<SymbolicCore::SymbolicMatrix> current_matrix_;
    bool is_preview_;
    bool latex_code_visible_;
};
