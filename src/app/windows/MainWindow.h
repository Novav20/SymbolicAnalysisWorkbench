#pragma once

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QAction>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QLabel>
#include <memory>

class MatrixInputWidget;
class SymbolicResultWidget;
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
 * providing tabs for different types of symbolic computations relevant
 * to robotics analysis.
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
    void setupStatusBar();
    void connectSignals();
    void updateWindowTitle(const QString& projectPath = "");

    // UI Components
    QWidget* central_widget_;
    QVBoxLayout* main_layout_;
    QTabWidget* tab_widget_;
    
    // Tab widgets
    std::unique_ptr<MatrixInputWidget> matrix_input_widget_;
    std::unique_ptr<SymbolicResultWidget> symbolic_result_widget_;
    std::unique_ptr<DHParameterWidget> dh_parameter_widget_;
    
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
