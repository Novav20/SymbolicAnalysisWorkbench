#include "MainWindow.h"
#include "../widgets/MatrixInputWidget.h"
#include "../widgets/LiveResultPanel.h"
#include "../widgets/DHParameterWidget.h"
#include "SymbolicCore/SymbolicMatrix.h"
#include "SymbolicCore/DHMatrixCalculator.h"

#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QApplication>
#include <QtCore/QSettings>
#include <QCloseEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , central_widget_(nullptr)
    , main_layout_(nullptr)
    , main_splitter_(nullptr)
    , mode_toolbar_(nullptr)
    , matrix_mode_action_(nullptr)
    , dh_mode_action_(nullptr)
    , input_stack_(nullptr)
    , menu_bar_(nullptr)
    , status_bar_(nullptr)
    , status_label_(nullptr)
    , matrix_size_label_(nullptr)
    , project_modified_(false)
{
    setupUI();
    setupMenus();
    setupToolBar();
    setupStatusBar();
    connectSignals();
    updateWindowTitle();
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUI()
{
    // Create central widget and layout
    central_widget_ = new QWidget(this);
    main_layout_ = new QVBoxLayout(central_widget_);
    
    // Create main splitter (horizontal: left=input, right=output)
    main_splitter_ = new QSplitter(Qt::Horizontal, this);
    
    // LEFT PANEL: Stacked widget for input modes
    input_stack_ = new QStackedWidget();
    
    //Create input widgets
    matrix_input_widget_ = std::make_unique<MatrixInputWidget>();
    dh_parameter_widget_ = std::make_unique<DHParameterWidget>();
    
    // Add to stack (index 0 = Matrix, index 1 = DH)
    input_stack_->addWidget(matrix_input_widget_.get());
    input_stack_->addWidget(dh_parameter_widget_.get());
    input_stack_->setCurrentIndex(0); // Default to Matrix Input
    
    // RIGHT PANEL: Live result display
    result_panel_ = std::make_unique<LiveResultPanel>();
    
    // Add panels to splitter
    main_splitter_->addWidget(input_stack_);
    main_splitter_->addWidget(result_panel_.get());
    
    // Set initial sizes (40% left, 60% right)
    main_splitter_->setStretchFactor(0, 4);
    main_splitter_->setStretchFactor(1, 6);
    
    // Add splitter to layout
    main_layout_->addWidget(main_splitter_);
    
    // Set central widget
    setCentralWidget(central_widget_);
    
    // Set window properties
    resize(1400, 900);
    setMinimumSize(1000, 700);
}

void MainWindow::setupMenus()
{
    // Create menu bar
    menu_bar_ = menuBar();
    
    // File menu
    auto file_menu = menu_bar_->addMenu("&File");
    
    new_action_ = file_menu->addAction("&New Project");
    new_action_->setShortcut(QKeySequence::New);
    new_action_->setStatusTip("Create a new symbolic analysis project");
    connect(new_action_, &QAction::triggered, this, &MainWindow::newProject);
    
    open_action_ = file_menu->addAction("&Open Project");
    open_action_->setShortcut(QKeySequence::Open);
    open_action_->setStatusTip("Open an existing symbolic analysis project");
    connect(open_action_, &QAction::triggered, this, &MainWindow::openProject);
    
    save_action_ = file_menu->addAction("&Save Project");
    save_action_->setShortcut(QKeySequence::Save);
    save_action_->setStatusTip("Save the current project");
    connect(save_action_, &QAction::triggered, this, &MainWindow::saveProject);
    
    file_menu->addSeparator();
    
    export_latex_action_ = file_menu->addAction("Export to &LaTeX");
    export_latex_action_->setStatusTip("Export symbolic results to LaTeX format");
    connect(export_latex_action_, &QAction::triggered, this, &MainWindow::exportToLatex);
    
    export_python_action_ = file_menu->addAction("Export to &Python");
    export_python_action_->setStatusTip("Export symbolic results to Python code");
    connect(export_python_action_, &QAction::triggered, this, &MainWindow::exportToPython);
    
    file_menu->addSeparator();
    
    exit_action_ = file_menu->addAction("E&xit");
    exit_action_->setShortcut(QKeySequence::Quit);
    exit_action_->setStatusTip("Exit the application");
    connect(exit_action_, &QAction::triggered, this, &QWidget::close);
    
    // Help menu
    auto help_menu = menu_bar_->addMenu("&Help");
    
    about_action_ = help_menu->addAction("&About");
    about_action_->setStatusTip("Show information about the application");
    connect(about_action_, &QAction::triggered, this, &MainWindow::about);
    
    about_qt_action_ = help_menu->addAction("About &Qt");
    about_qt_action_->setStatusTip("Show information about Qt");
    connect(about_qt_action_, &QAction::triggered, &QApplication::aboutQt);
}

void MainWindow::setupToolBar()
{
    // Create toolbar for switching input modes
    mode_toolbar_ = addToolBar("Input Mode");
    mode_toolbar_->setMovable(false);
    
    // Matrix Input mode action
    matrix_mode_action_ = mode_toolbar_->addAction("📊 Matrix Input");
    matrix_mode_action_->setCheckable(true);
    matrix_mode_action_->setChecked(true);
    connect(matrix_mode_action_, &QAction::triggered, [this]() {
        input_stack_->setCurrentIndex(0);
        matrix_mode_action_->setChecked(true);
        dh_mode_action_->setChecked(false);
    });
    
    // DH Parameters mode action
    dh_mode_action_ = mode_toolbar_->addAction("🤖 DH Parameters");
    dh_mode_action_->setCheckable(true);
    connect(dh_mode_action_, &QAction::triggered, [this]() {
        input_stack_->setCurrentIndex(1);
        dh_mode_action_->setChecked(true);
        matrix_mode_action_->setChecked(false);
    });
}

void MainWindow::setupStatusBar()
{
    status_bar_ = statusBar();
    
    status_label_ = new QLabel("Ready");
    matrix_size_label_ = new QLabel("Matrix: N/A");
    
    status_bar_->addWidget(status_label_);
    status_bar_->addPermanentWidget(matrix_size_label_);
}

void MainWindow::connectSignals()
{
    // Connect matrix computation signals to LiveResultPanel
    if (matrix_input_widget_) {
        connect(matrix_input_widget_.get(), &MatrixInputWidget::matrixComputed,
                result_panel_.get(), &LiveResultPanel::displayMatrix);
        connect(matrix_input_widget_.get(), &MatrixInputWidget::matrixComputed,
                this, &MainWindow::onMatrixComputed);
    }
    
    // Connect DH parameter updates to LiveResultPanel
    if (dh_parameter_widget_) {
        connect(dh_parameter_widget_.get(), &DHParameterWidget::parametersUpdated,
                this, &MainWindow::onDHParametersUpdated);
        connect(dh_parameter_widget_.get(), &DHParameterWidget::forwardKinematicsComputed,
                result_panel_.get(), &LiveResultPanel::displayMatrix);
        connect(dh_parameter_widget_.get(), &DHParameterWidget::jacobianComputed,
                result_panel_.get(), &LiveResultPanel::displayMatrix);
    }
    
    // Connect error signals
    if (matrix_input_widget_) {
        connect(matrix_input_widget_.get(), &MatrixInputWidget::errorOccurred,
                this, &MainWindow::onErrorOccurred);
    }
    if (dh_parameter_widget_) {
        connect(dh_parameter_widget_.get(), &DHParameterWidget::errorOccurred,
                this, &MainWindow::onErrorOccurred);
    }
    if (result_panel_) {
        connect(result_panel_.get(), &LiveResultPanel::errorOccurred,
                this, &MainWindow::onErrorOccurred);
    }
}

void MainWindow::updateWindowTitle(const QString& projectPath)
{
    QString title = "Symbolic Analyzer Workbench";
    if (project_modified_) {
        title += "*";
    }
    if (!projectPath.isEmpty()) {
        title += " - " + projectPath;
    }
    setWindowTitle(title);
}

void MainWindow::newProject()
{
    if (project_modified_) {
        auto reply = QMessageBox::question(this, "Unsaved Changes",
                                          "The current project has unsaved changes. Do you want to save it?",
                                          QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        
        if (reply == QMessageBox::Save) {
            saveProject();
        } else if (reply == QMessageBox::Cancel) {
            return;
        }
    }
    
    // Reset all widgets
    matrix_input_widget_->clear();
    result_panel_->clear();
    dh_parameter_widget_->clear();
    
    current_project_path_.clear();
    project_modified_ = false;
    updateWindowTitle();
    
    status_label_->setText("New project created");
}

void MainWindow::openProject()
{
    if (project_modified_) {
        auto reply = QMessageBox::question(this, "Unsaved Changes",
                                          "The current project has unsaved changes. Do you want to save it?",
                                          QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        
        if (reply == QMessageBox::Save) {
            saveProject();
        } else if (reply == QMessageBox::Cancel) {
            return;
        }
    }
    
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    "Open Symbolic Analyzer Project",
                                                    "",
                                                    "Symbolic Analyzer Projects (*.saw);;All Files (*)");
    
    if (!fileName.isEmpty()) {
        // TODO: Implement project loading
        current_project_path_ = fileName;
        project_modified_ = false;
        updateWindowTitle(fileName);
        status_label_->setText("Project loaded: " + fileName);
    }
}

void MainWindow::onErrorOccurred(const QString& error)
{
    status_label_->setText("Error: " + error);
    QMessageBox::warning(this, "Error", error);
}

void MainWindow::saveProject()
{
    if (current_project_path_.isEmpty()) {
        QString fileName = QFileDialog::getSaveFileName(this,
                                                        "Save Symbolic Analyzer Project",
                                                        "",
                                                        "Symbolic Analyzer Projects (*.saw);;All Files (*)");
        
        if (!fileName.isEmpty()) {
            current_project_path_ = fileName;
        } else {
            return;
        }
    }
    
    // TODO: Implement project saving
    project_modified_ = false;
    updateWindowTitle(current_project_path_);
    status_label_->setText("Project saved: " + current_project_path_);
}

void MainWindow::exportToLatex()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    "Export to LaTeX",
                                                    "",
                                                    "LaTeX Files (*.tex);;All Files (*)");
    
    if (!fileName.isEmpty()) {
        // TODO: Implement LaTeX export
        status_label_->setText("Exported to LaTeX: " + fileName);
    }
}

void MainWindow::exportToPython()
{
    QString fileName = QFileDialog::getSaveFileName(this,
                                                    "Export to Python",
                                                    "",
                                                    "Python Files (*.py);;All Files (*)");
    
    if (!fileName.isEmpty()) {
        // TODO: Implement Python export
        status_label_->setText("Exported to Python: " + fileName);
    }
}

void MainWindow::about()
{
    QMessageBox::about(this, "About Symbolic Analyzer Workbench",
                      "<h2>Symbolic Analyzer Workbench</h2>"
                      "<p>Version 1.0.0</p>"
                      "<p>A symbolic mathematics tool for robotics analysis.</p>"
                      "<p>Features:</p>"
                      "<ul>"
                      "<li>Symbolic matrix operations</li>"
                      "<li>Denavit-Hartenberg parameter calculations</li>"
                      "<li>Inverse kinematics assistance</li>"
                      "<li>Export to LaTeX and Python</li>"
                      "</ul>"
                      "<p>© 2025 Robotics Research</p>");
}

void MainWindow::onMatrixComputed(const SymbolicCore::SymbolicMatrix& matrix)
{
    // Update status
    QString size_text = QString("Matrix: %1x%2").arg(matrix.rows()).arg(matrix.cols());
    matrix_size_label_->setText(size_text);
    
    // Mark project as modified
    project_modified_ = true;
    updateWindowTitle();
    
    status_label_->setText("Matrix computation completed");
}

void MainWindow::onDHParametersUpdated()
{
    // Mark project as modified
    project_modified_ = true;
    updateWindowTitle();
    
    status_label_->setText("DH parameters updated");
}



void MainWindow::closeEvent(QCloseEvent *event)
{
    if (project_modified_) {
        auto reply = QMessageBox::question(this, "Unsaved Changes",
                                          "The current project has unsaved changes. Do you want to save it?",
                                          QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        
        if (reply == QMessageBox::Save) {
            saveProject();
            event->accept();
        } else if (reply == QMessageBox::Discard) {
            event->accept();
        } else {
            event->ignore();
        }
    } else {
        event->accept();
    }
}
