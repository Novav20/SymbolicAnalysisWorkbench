#pragma once

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QAction>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QLabel>
#include <memory>

class MatrixInputWidget;
class LiveResultPanel;
class DHParameterWidget;

namespace SymbolicCore {
    class SymbolicMatrix;
    class DHMatrixCalculator;
}

/**
 * @class MainWindow
 * @brief Main application window for Symbolic Analysis Workbench
 * 
 * This is the main GUI interface for the symbolic analysis workbench,
 * providing a split-panel layout with input controls on the left and
 * live LaTeX output on the right.
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

private slots:
    void newProject();
    void openProject();
    void saveProject();
    void exportToLatex();
    void exportToPython();
    void about();
    void onMatrixComputed(const SymbolicCore::SymbolicMatrix& matrix);
    void onDHParametersUpdated();
    void onErrorOccurred(const QString& error);

private:
    void setupUI();
    void setupMenus();
    void setupToolBar();
    void setupStatusBar();
    void connectSignals();
    void updateWindowTitle(const QString& projectPath = "");

    // UI Components
    QWidget* central_widget_;
    QVBoxLayout* main_layout_;
    QSplitter* main_splitter_;
    
    // Toolbar for input mode switching
    QToolBar* mode_toolbar_;
    QAction* matrix_mode_action_;
    QAction* dh_mode_action_;
    
    // Left panel: Input widgets
    QStackedWidget* input_stack_;
    std::unique_ptr<MatrixInputWidget> matrix_input_widget_;
    std::unique_ptr<DHParameterWidget> dh_parameter_widget_;
    
    // Right panel: Live result display
    std::unique_ptr<LiveResultPanel> result_panel_;
    
    // Menu and status bar
    QMenuBar* menu_bar_;
    QStatusBar* status_bar_;
    
    // Actions
    QAction* new_action_;
    QAction* open_action_;
    QAction* save_action_;
    QAction* export_latex_action_;
    QAction* export_python_action_;
    QAction* exit_action_;
    QAction* about_action_;
    QAction* about_qt_action_;
    
    // Status bar labels
    QLabel* status_label_;
    QLabel* matrix_size_label_;
    
    // Current project
    QString current_project_path_;
    bool project_modified_;

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;
};
