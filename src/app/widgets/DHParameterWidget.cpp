#include "DHParameterWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QScrollArea>
#include <QComboBox>
#include <QTextEdit>
#include <QLineEdit>
#include <QFileDialog>

DHParameterWidget::DHParameterWidget(QWidget *parent)
    : QWidget(parent)
    , main_layout_(nullptr)
    , tab_widget_(nullptr)
    , parameters_tab_(nullptr)
    , parameters_layout_(nullptr)
    , joint_management_group_(nullptr)
    , joint_management_layout_(nullptr)
    , add_joint_btn_(nullptr)
    , remove_joint_btn_(nullptr)
    , clear_all_btn_(nullptr)
    , joint_count_label_(nullptr)
    , dh_table_group_(nullptr)
    , dh_table_layout_(nullptr)
    , dh_table_scroll_(nullptr)
    , dh_parameter_table_(nullptr)
    , parameter_controls_group_(nullptr)
    , parameter_controls_layout_(nullptr)
    , validate_params_btn_(nullptr)
    , compute_transform_btn_(nullptr)
    , export_transforms_btn_(nullptr)
    , kinematics_tab_(nullptr)
    , kinematics_layout_(nullptr)
    , forward_kinematics_group_(nullptr)
    , forward_kinematics_layout_(nullptr)
    , compute_fk_btn_(nullptr)
    , fk_result_(nullptr)
    , jacobian_group_(nullptr)
    , jacobian_layout_(nullptr)
    , compute_jacobian_btn_(nullptr)
    , jacobian_result_(nullptr)
    , inverse_kinematics_tab_(nullptr)
    , inverse_kinematics_layout_(nullptr)
    , ik_assistance_group_(nullptr)
    , ik_assistance_layout_(nullptr)
    , ik_description_(nullptr)
    , target_pose_group_(nullptr)
    , target_pose_layout_(nullptr)
    , target_pose_edit_(nullptr)
    , ik_assist_btn_(nullptr)
    , ik_result_(nullptr)
    , workspace_tab_(nullptr)
    , workspace_layout_(nullptr)
    , workspace_analysis_group_(nullptr)
    , workspace_analysis_layout_(nullptr)
    , compute_workspace_btn_(nullptr)
    , workspace_result_(nullptr)
    , presets_tab_(nullptr)
    , presets_layout_(nullptr)
    , robot_presets_group_(nullptr)
    , robot_presets_layout_(nullptr)
    , preset_combo_(nullptr)
    , preset_description_(nullptr)
    , apply_preset_btn_(nullptr)
    , parameters_modified_(false)
{
    dh_calculator_ = std::make_unique<SymbolicCore::DHMatrixCalculator>();
    setupUI();
    connectSignals();
}

DHParameterWidget::~DHParameterWidget() = default;

void DHParameterWidget::setupUI()
{
    main_layout_ = new QVBoxLayout(this);
    
    // Create tab widget
    tab_widget_ = new QTabWidget(this);
    
    // Setup individual tabs
    setupParametersTab();
    setupKinematicsTab();
    setupInverseKinematicsTab();
    setupWorkspaceTab();
    setupPresetsTab();
    
    // Add tabs to widget
    tab_widget_->addTab(parameters_tab_, "DH Parameters");
    tab_widget_->addTab(kinematics_tab_, "Kinematics");
    tab_widget_->addTab(inverse_kinematics_tab_, "Inverse Kinematics");
    tab_widget_->addTab(workspace_tab_, "Workspace");
    tab_widget_->addTab(presets_tab_, "Robot Presets");
    
    main_layout_->addWidget(tab_widget_);
}

void DHParameterWidget::setupParametersTab()
{
    parameters_tab_ = new QWidget();
    parameters_layout_ = new QVBoxLayout(parameters_tab_);
    
    // Joint management group
    joint_management_group_ = new QGroupBox("Joint Management", parameters_tab_);
    joint_management_layout_ = new QHBoxLayout(joint_management_group_);
    
    add_joint_btn_ = new QPushButton("Add Joint", joint_management_group_);
    remove_joint_btn_ = new QPushButton("Remove Joint", joint_management_group_);
    clear_all_btn_ = new QPushButton("Clear All", joint_management_group_);
    joint_count_label_ = new QLabel("Joints: 0", joint_management_group_);
    
    joint_management_layout_->addWidget(add_joint_btn_);
    joint_management_layout_->addWidget(remove_joint_btn_);
    joint_management_layout_->addWidget(clear_all_btn_);
    joint_management_layout_->addStretch();
    joint_management_layout_->addWidget(joint_count_label_);
    
    // DH parameter table
    dh_table_group_ = new QGroupBox("Denavit-Hartenberg Parameters", parameters_tab_);
    dh_table_layout_ = new QVBoxLayout(dh_table_group_);
    
    dh_table_scroll_ = new QScrollArea(dh_table_group_);
    dh_parameter_table_ = new QTableWidget(0, 5, dh_table_scroll_);
    dh_table_scroll_->setWidget(dh_parameter_table_);
    dh_table_scroll_->setWidgetResizable(true);
    
    // Setup table headers
    QStringList headers;
    headers << "Type" << "θ (theta)" << "d" << "a" << "α (alpha)";
    dh_parameter_table_->setHorizontalHeaderLabels(headers);
    dh_parameter_table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    
    dh_table_layout_->addWidget(dh_table_scroll_);
    
    // Parameter controls
    parameter_controls_group_ = new QGroupBox("Controls", parameters_tab_);
    parameter_controls_layout_ = new QHBoxLayout(parameter_controls_group_);
    
    validate_params_btn_ = new QPushButton("Validate Parameters", parameter_controls_group_);
    compute_transform_btn_ = new QPushButton("Compute Transform", parameter_controls_group_);
    export_transforms_btn_ = new QPushButton("Export Transformations", parameter_controls_group_);
    
    parameter_controls_layout_->addWidget(validate_params_btn_);
    parameter_controls_layout_->addWidget(compute_transform_btn_);
    parameter_controls_layout_->addWidget(export_transforms_btn_);
    
    parameters_layout_->addWidget(joint_management_group_);
    parameters_layout_->addWidget(dh_table_group_);
    parameters_layout_->addWidget(parameter_controls_group_);
}

void DHParameterWidget::setupKinematicsTab()
{
    kinematics_tab_ = new QWidget();
    kinematics_layout_ = new QVBoxLayout(kinematics_tab_);
    
    // Forward kinematics
    forward_kinematics_group_ = new QGroupBox("Forward Kinematics", kinematics_tab_);
    forward_kinematics_layout_ = new QVBoxLayout(forward_kinematics_group_);
    
    compute_fk_btn_ = new QPushButton("Compute Forward Kinematics", forward_kinematics_group_);
    fk_result_ = new QTextEdit(forward_kinematics_group_);
    fk_result_->setReadOnly(true);
    fk_result_->setFont(QFont("Courier", 10));
    
    forward_kinematics_layout_->addWidget(compute_fk_btn_);
    forward_kinematics_layout_->addWidget(fk_result_);
    
    // Jacobian computation
    jacobian_group_ = new QGroupBox("Jacobian Analysis", kinematics_tab_);
    jacobian_layout_ = new QVBoxLayout(jacobian_group_);
    
    compute_jacobian_btn_ = new QPushButton("Compute Jacobian", jacobian_group_);
    jacobian_result_ = new QTextEdit(jacobian_group_);
    jacobian_result_->setReadOnly(true);
    jacobian_result_->setFont(QFont("Courier", 10));
    
    jacobian_layout_->addWidget(compute_jacobian_btn_);
    jacobian_layout_->addWidget(jacobian_result_);
    
    kinematics_layout_->addWidget(forward_kinematics_group_);
    kinematics_layout_->addWidget(jacobian_group_);
}

void DHParameterWidget::setupInverseKinematicsTab()
{
    inverse_kinematics_tab_ = new QWidget();
    inverse_kinematics_layout_ = new QVBoxLayout(inverse_kinematics_tab_);
    
    // IK assistance
    ik_assistance_group_ = new QGroupBox("Inverse Kinematics Assistance", inverse_kinematics_tab_);
    ik_assistance_layout_ = new QVBoxLayout(ik_assistance_group_);
    
    ik_description_ = new QLabel(
        "This tool helps with symbolic inverse kinematics by post-multiplying "
        "transformation matrices by their inverses to isolate joint variables. "
        "Enter the target end-effector pose below.",
        ik_assistance_group_
    );
    ik_description_->setWordWrap(true);
    
    // Target pose input
    target_pose_group_ = new QGroupBox("Target End-Effector Pose", ik_assistance_group_);
    target_pose_layout_ = new QVBoxLayout(target_pose_group_);
    
    target_pose_edit_ = new QTextEdit(target_pose_group_);
    target_pose_edit_->setPlaceholderText(
        "Enter 4x4 transformation matrix or pose information:\n"
        "[r11, r12, r13, px]\n"
        "[r21, r22, r23, py]\n"
        "[r31, r32, r33, pz]\n"
        "[0,   0,   0,   1]"
    );
    target_pose_edit_->setMaximumHeight(120);
    
    target_pose_layout_->addWidget(target_pose_edit_);
    
    ik_assist_btn_ = new QPushButton("Generate IK Equations", ik_assistance_group_);
    ik_result_ = new QTextEdit(ik_assistance_group_);
    ik_result_->setReadOnly(true);
    ik_result_->setFont(QFont("Courier", 10));
    
    ik_assistance_layout_->addWidget(ik_description_);
    ik_assistance_layout_->addWidget(target_pose_group_);
    ik_assistance_layout_->addWidget(ik_assist_btn_);
    ik_assistance_layout_->addWidget(ik_result_);
    
    inverse_kinematics_layout_->addWidget(ik_assistance_group_);
}

void DHParameterWidget::setupWorkspaceTab()
{
    workspace_tab_ = new QWidget();
    workspace_layout_ = new QVBoxLayout(workspace_tab_);
    
    workspace_analysis_group_ = new QGroupBox("Workspace Analysis", workspace_tab_);
    workspace_analysis_layout_ = new QVBoxLayout(workspace_analysis_group_);
    
    compute_workspace_btn_ = new QPushButton("Compute Workspace", workspace_analysis_group_);
    workspace_result_ = new QTextEdit(workspace_analysis_group_);
    workspace_result_->setReadOnly(true);
    workspace_result_->setFont(QFont("Courier", 10));
    
    workspace_analysis_layout_->addWidget(compute_workspace_btn_);
    workspace_analysis_layout_->addWidget(workspace_result_);
    
    workspace_layout_->addWidget(workspace_analysis_group_);
}

void DHParameterWidget::setupPresetsTab()
{
    presets_tab_ = new QWidget();
    presets_layout_ = new QVBoxLayout(presets_tab_);
    
    robot_presets_group_ = new QGroupBox("Robot Configurations", presets_tab_);
    robot_presets_layout_ = new QVBoxLayout(robot_presets_group_);
    
    preset_combo_ = new QComboBox(robot_presets_group_);
    preset_combo_->addItems({
        "2-DOF Planar",
        "3-DOF Planar", 
        "SCARA Robot",
        "Stanford Arm",
        "PUMA 560",
        "KUKA KR6",
        "ABB IRB 120",
        "Custom 6-DOF"
    });
    
    preset_description_ = new QLabel("Select a robot configuration to load preset DH parameters.", robot_presets_group_);
    preset_description_->setWordWrap(true);
    
    apply_preset_btn_ = new QPushButton("Apply Preset", robot_presets_group_);
    
    robot_presets_layout_->addWidget(preset_combo_);
    robot_presets_layout_->addWidget(preset_description_);
    robot_presets_layout_->addWidget(apply_preset_btn_);
    
    presets_layout_->addWidget(robot_presets_group_);
}

void DHParameterWidget::connectSignals()
{
    // Parameter management
    connect(add_joint_btn_, &QPushButton::clicked, this, &DHParameterWidget::onAddJoint);
    connect(remove_joint_btn_, &QPushButton::clicked, this, &DHParameterWidget::onRemoveJoint);
    connect(clear_all_btn_, &QPushButton::clicked, this, &DHParameterWidget::onClearAll);
    
    // Parameter changes
    connect(dh_parameter_table_, &QTableWidget::cellChanged,
            this, &DHParameterWidget::onParameterCellChanged);
    
    // Parameter controls
    connect(validate_params_btn_, &QPushButton::clicked, this, &DHParameterWidget::onValidateParameters);
    connect(compute_transform_btn_, &QPushButton::clicked, this, &DHParameterWidget::onComputeSingleTransform);
    connect(export_transforms_btn_, &QPushButton::clicked, this, &DHParameterWidget::onExportTransformations);
    
    // Kinematics
    connect(compute_fk_btn_, &QPushButton::clicked, this, &DHParameterWidget::onComputeForwardKinematics);
    connect(compute_jacobian_btn_, &QPushButton::clicked, this, &DHParameterWidget::onComputeJacobian);
    
    // Inverse kinematics
    connect(ik_assist_btn_, &QPushButton::clicked, this, &DHParameterWidget::onInverseKinematicsAssist);
    
    // Workspace
    connect(compute_workspace_btn_, &QPushButton::clicked, this, &DHParameterWidget::onComputeWorkspace);
    
    // Presets
    connect(preset_combo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &DHParameterWidget::onPresetRobotSelected);
    connect(apply_preset_btn_, &QPushButton::clicked, this, &DHParameterWidget::onPresetRobotSelected);
}

void DHParameterWidget::onAddJoint()
{
    int current_row = dh_parameter_table_->rowCount();
    dh_parameter_table_->setRowCount(current_row + 1);
    
    // Initialize with default values
    // Initialize with default values
    auto type_combo = new QComboBox();
    type_combo->addItems({"Revolute", "Prismatic"});
    
    auto theta_item = new QTableWidgetItem("θ" + QString::number(current_row + 1));
    auto d_item = new QTableWidgetItem("0");
    auto a_item = new QTableWidgetItem("1");
    auto alpha_item = new QTableWidgetItem("0");
    
    dh_parameter_table_->setCellWidget(current_row, 0, type_combo);
    dh_parameter_table_->setItem(current_row, 1, theta_item);
    dh_parameter_table_->setItem(current_row, 2, d_item);
    dh_parameter_table_->setItem(current_row, 3, a_item);
    dh_parameter_table_->setItem(current_row, 4, alpha_item);
    
    updateJointCount();
    parameters_modified_ = true;
}

void DHParameterWidget::onRemoveJoint()
{
    int current_row = dh_parameter_table_->currentRow();
    if (current_row >= 0) {
        dh_parameter_table_->removeRow(current_row);
        updateJointCount();
        parameters_modified_ = true;
    }
}

void DHParameterWidget::onClearAll()
{
    dh_parameter_table_->setRowCount(0);
    updateJointCount();
    parameters_modified_ = true;
}

void DHParameterWidget::onParameterCellChanged(int row, int col)
{
    parameters_modified_ = true;
    
    // Validate input
    auto item = dh_parameter_table_->item(row, col);
    if (item) {
        QString text = item->text().trimmed();
        
        // Basic validation for symbolic expressions
        QRegularExpression validPattern("^[a-zA-Z0-9_+\\-*/().,π\\s]+$");
        if (!text.isEmpty() && !validPattern.match(text).hasMatch()) {
            item->setText("0");
        }
    }
}

void DHParameterWidget::onComputeForwardKinematics()
{
    try {
        auto params = getDHParametersFromTable();
        dh_calculator_->setDHParameters(params);
        
        auto end_effector = dh_calculator_->computeEndEffectorPosition();
        displayTransformationMatrix(end_effector);
        
        emit forwardKinematicsComputed(end_effector);
        
    } catch (const std::exception& e) {
        emit errorOccurred(QString("Forward kinematics computation failed: %1").arg(e.what()));
    }
}

void DHParameterWidget::onComputeJacobian()
{
    try {
        auto params = getDHParametersFromTable();
        dh_calculator_->setDHParameters(params);
        
        auto jacobian = dh_calculator_->computeGeometricJacobian();
        displayJacobian(jacobian);
        
        emit jacobianComputed(jacobian);
        
    } catch (const std::exception& e) {
        emit errorOccurred(QString("Jacobian computation failed: %1").arg(e.what()));
    }
}

void DHParameterWidget::onComputeWorkspace()
{
}

void DHParameterWidget::onComputeSingleTransform()
{
    // Same implementation as Forward Kinematics - compute end effector position
    try {
        auto params = getDHParametersFromTable();
        dh_calculator_->setDHParameters(params);
        
        auto end_effector = dh_calculator_->computeEndEffectorPosition();
        displayTransformationMatrix(end_effector);
        
        // Emit signal to show in right panel
        emit forwardKinematicsComputed(end_effector);
        emit parametersUpdated();
        
    } catch (const std::exception& e) {
        emit errorOccurred(QString("Transform computation failed: %1").arg(e.what()));
    }
}

void DHParameterWidget::displayWorkspaceInfo(const std::vector<std::vector<double>>& workspace)
{
    QString result = "Workspace Analysis Results:\n\n";
    result += QString("Sample points: %1\n").arg(workspace.size());
    
    if (!workspace.empty()) {
        result += "Sample positions:\n";
        for (size_t i = 0; i < std::min(workspace.size(), size_t(5)); ++i) {
            for (double val : workspace[i]) {
                result += QString("  %1").arg(val, 8, 'f', 4);
            }
            result += "\n";
        }
    }
    
    workspace_result_->setText(result);
}

void DHParameterWidget::validateCurrentParameters()
{
    try {
        auto params = getDHParametersFromTable();
        dh_calculator_->setDHParameters(params);
        
        if (dh_calculator_->isValidDHConfiguration()) {
            QMessageBox::information(this, "Validation", "DH parameters are valid!");
        } else {
            auto errors = dh_calculator_->getValidationErrors();
            QString error_text = "Validation Errors:\n";
            for (const auto& error : errors) {
                error_text += "- " + QString::fromStdString(error) + "\n";
            }
            QMessageBox::warning(this, "Validation Failed", error_text);
        }
        
    } catch (const std::exception& e) {
        emit errorOccurred(QString("Validation failed: %1").arg(e.what()));
    }
}

void DHParameterWidget::applyPreset(const QString& preset_name)
{
    // Clear current parameters
    onClearAll();
    
    if (preset_name == "2-DOF Planar") {
        // Add 2 joints for planar manipulator
        onAddJoint();
        onAddJoint();
        
        // Set parameters
        dh_parameter_table_->item(0, 1)->setText("θ1");
        dh_parameter_table_->item(0, 2)->setText("0");
        dh_parameter_table_->item(0, 3)->setText("1");
        dh_parameter_table_->item(0, 4)->setText("0");
        
        dh_parameter_table_->item(1, 1)->setText("θ2");
        dh_parameter_table_->item(1, 2)->setText("0");
        dh_parameter_table_->item(1, 3)->setText("1");
        dh_parameter_table_->item(1, 4)->setText("0");
        
    } else if (preset_name == "SCARA Robot") {
        // Add 4 joints for SCARA
        onAddJoint();
        onAddJoint();
        onAddJoint();
        onAddJoint();
        
        // Set typical SCARA parameters
        dh_parameter_table_->item(0, 1)->setText("θ1");
        dh_parameter_table_->item(0, 2)->setText("0");
        dh_parameter_table_->item(0, 3)->setText("a1");
        dh_parameter_table_->item(0, 4)->setText("0");
        
        dh_parameter_table_->item(1, 1)->setText("θ2");
        dh_parameter_table_->item(1, 2)->setText("0");
        dh_parameter_table_->item(1, 3)->setText("a2");
        dh_parameter_table_->item(1, 4)->setText("0");
        
        // Joint 3 is Prismatic
        auto type_combo = qobject_cast<QComboBox*>(dh_parameter_table_->cellWidget(2, 0));
        if (type_combo) type_combo->setCurrentIndex(1); // Prismatic
        
        dh_parameter_table_->item(2, 1)->setText("0");
        dh_parameter_table_->item(2, 2)->setText("d3");
        dh_parameter_table_->item(2, 3)->setText("0");
        dh_parameter_table_->item(2, 4)->setText("0");
        
        dh_parameter_table_->item(3, 1)->setText("θ4");
        dh_parameter_table_->item(3, 2)->setText("0");
        dh_parameter_table_->item(3, 3)->setText("0");
        dh_parameter_table_->item(3, 4)->setText("0");
        
    } else if (preset_name == "PUMA 560") {
        // Add 6 joints for PUMA 560
        for (int i = 0; i < 6; ++i) {
            onAddJoint();
        }
        
        // Set typical PUMA 560 parameters
        dh_parameter_table_->item(0, 1)->setText("θ1");
        dh_parameter_table_->item(0, 2)->setText("0");
        dh_parameter_table_->item(0, 3)->setText("0");
        dh_parameter_table_->item(0, 4)->setText("-π/2");
        
        dh_parameter_table_->item(1, 1)->setText("θ2");
        dh_parameter_table_->item(1, 2)->setText("0");
        dh_parameter_table_->item(1, 3)->setText("a2");
        dh_parameter_table_->item(1, 4)->setText("0");
        
        dh_parameter_table_->item(2, 1)->setText("θ3");
        dh_parameter_table_->item(2, 2)->setText("d3");
        dh_parameter_table_->item(2, 3)->setText("a3");
        dh_parameter_table_->item(2, 4)->setText("0");
        
        dh_parameter_table_->item(3, 1)->setText("θ4");
        dh_parameter_table_->item(3, 2)->setText("d4");
        dh_parameter_table_->item(3, 3)->setText("0");
        dh_parameter_table_->item(3, 4)->setText("-π/2");
        
        dh_parameter_table_->item(4, 1)->setText("θ5");
        dh_parameter_table_->item(4, 2)->setText("0");
        dh_parameter_table_->item(4, 3)->setText("0");
        dh_parameter_table_->item(4, 4)->setText("π/2");
        
        dh_parameter_table_->item(5, 1)->setText("θ6");
        dh_parameter_table_->item(5, 2)->setText("d6");
        dh_parameter_table_->item(5, 3)->setText("0");
        dh_parameter_table_->item(5, 4)->setText("0");
    } else if (preset_name == "Stanford Arm") {
         // Add 6 joints for Stanford Arm
        for (int i = 0; i < 6; ++i) {
            onAddJoint();
        }
        
        // Joint 3 is Prismatic
        auto type_combo = qobject_cast<QComboBox*>(dh_parameter_table_->cellWidget(2, 0));
        if (type_combo) type_combo->setCurrentIndex(1); // Prismatic
        
        // Set parameters (placeholder values for now)
        dh_parameter_table_->item(0, 1)->setText("θ1");
        dh_parameter_table_->item(0, 4)->setText("-90");
        
        dh_parameter_table_->item(1, 1)->setText("θ2");
        dh_parameter_table_->item(1, 2)->setText("d2");
        dh_parameter_table_->item(1, 4)->setText("90");
        
        dh_parameter_table_->item(2, 1)->setText("0"); // Fixed theta
        dh_parameter_table_->item(2, 2)->setText("d3"); // Variable d
        
        dh_parameter_table_->item(3, 1)->setText("θ4");
        dh_parameter_table_->item(3, 4)->setText("-90");
        
        dh_parameter_table_->item(4, 1)->setText("θ5");
        dh_parameter_table_->item(4, 4)->setText("90");
        
        dh_parameter_table_->item(5, 1)->setText("θ6");
        dh_parameter_table_->item(5, 2)->setText("d6");
    }
    
    parameters_modified_ = true;
    emit parametersUpdated();
}

void DHParameterWidget::clear()
{
    onClearAll();
    fk_result_->clear();
    jacobian_result_->clear();
    workspace_result_->clear();
    ik_result_->clear();
    target_pose_edit_->clear();
}

void DHParameterWidget::setDHParameters(const std::vector<SymbolicCore::DHParameters>& params)
{
    onClearAll();
    
    for (size_t i = 0; i < params.size(); ++i) {
        onAddJoint();
        setDHParametersToRow(static_cast<int>(i), params[i]);
    }
    
    parameters_modified_ = true;
    emit parametersUpdated();
}

std::vector<SymbolicCore::DHParameters> DHParameterWidget::getDHParameters() const
{
    return getDHParametersFromTable();
}

SymbolicCore::DHMatrixCalculator* DHParameterWidget::getCalculator() const
{
    return dh_calculator_.get();
}
// Missing functions from DHParameterWidget.cpp



#include "SymbolicCore/DHMatrixCalculator.h"

#include "SymbolicCore/SymbolicMatrix.h"

#include "SymbolicCore/SymbolicScalar.h"



void DHParameterWidget::updateJointCount()

{

    int count = dh_parameter_table_->rowCount();

    joint_count_label_->setText(QString("Joints: %1").arg(count));

}



SymbolicCore::DHParameters DHParameterWidget::getDHParametersFromRow(int row) const
{
    auto type_combo = qobject_cast<QComboBox*>(dh_parameter_table_->cellWidget(row, 0));
    SymbolicCore::JointType type = SymbolicCore::JointType::Revolute;
    
    if (type_combo && type_combo->currentText() == "Prismatic") {
        type = SymbolicCore::JointType::Prismatic;
    }

    auto theta_item = dh_parameter_table_->item(row, 1);
    auto d_item = dh_parameter_table_->item(row, 2);
    auto a_item = dh_parameter_table_->item(row, 3);
    auto alpha_item = dh_parameter_table_->item(row, 4);
    
    QString theta_text = theta_item ? theta_item->text().trimmed() : "0";
    QString d_text = d_item ? d_item->text().trimmed() : "0";
    QString a_text = a_item ? a_item->text().trimmed() : "0";
    QString alpha_text = alpha_item ? alpha_item->text().trimmed() : "0";
    
    // Parse as symbolic expressions
    auto theta = SymbolicCore::SymbolicScalar::parse(theta_text.toStdString());
    auto d = SymbolicCore::SymbolicScalar::parse(d_text.toStdString());
    auto a = SymbolicCore::SymbolicScalar::parse(a_text.toStdString());
    auto alpha = SymbolicCore::SymbolicScalar::parse(alpha_text.toStdString());
    
    return SymbolicCore::DHParameters(theta, d, a, alpha, type);

}



std::vector<SymbolicCore::DHParameters> DHParameterWidget::getDHParametersFromTable() const

{

    std::vector<SymbolicCore::DHParameters> params;

    

    for (int i = 0; i < dh_parameter_table_->rowCount(); ++i) {

        params.push_back(getDHParametersFromRow(i));

    }

    

    return params;

}



void DHParameterWidget::setDHParametersToRow(int row, const SymbolicCore::DHParameters& params)
{
    auto type_combo = new QComboBox();
    type_combo->addItems({"Revolute", "Prismatic"});
    
    if (params.type == SymbolicCore::JointType::Prismatic) {
        type_combo->setCurrentIndex(1);
    } else {
        type_combo->setCurrentIndex(0);
    }

    auto theta_item = new QTableWidgetItem(QString::fromStdString(params.theta.toString()));
    auto d_item = new QTableWidgetItem(QString::fromStdString(params.d.toString()));
    auto a_item = new QTableWidgetItem(QString::fromStdString(params.a.toString()));
    auto alpha_item = new QTableWidgetItem(QString::fromStdString(params.alpha.toString()));
    
    dh_parameter_table_->setCellWidget(row, 0, type_combo);
    dh_parameter_table_->setItem(row, 1, theta_item);
    dh_parameter_table_->setItem(row, 2, d_item);
    dh_parameter_table_->setItem(row, 3, a_item);
    dh_parameter_table_->setItem(row, 4, alpha_item);

}



void DHParameterWidget::displayTransformationMatrix(const SymbolicCore::SymbolicMatrix& matrix)

{

    QString result = "Transformation Matrix:\n\n";

    result += QString::fromStdString(matrix.toString()) + "\n\n";

    

    // Add LaTeX representation

    result += "LaTeX Representation:\n";

    result += QString::fromStdString(matrix.toLatex()) + "\n";

    

    fk_result_->setText(result);

}



void DHParameterWidget::displayJacobian(const SymbolicCore::SymbolicMatrix& jacobian)

{

    QString result = "Geometric Jacobian:\n\n";

    result += QString::fromStdString(jacobian.toString()) + "\n\n";

    

    result += QString("Size: %1x%2\n").arg(jacobian.rows()).arg(jacobian.cols());

    

    if (jacobian.rows() == jacobian.cols()) {

        try {

            auto det = jacobian.determinant();

            result += QString("Determinant: %1\n").arg(QString::fromStdString(det.toString()));

        } catch (...) {

            result += "Determinant: Unable to compute\n";

        }

    }

    

    jacobian_result_->setText(result);

}



void DHParameterWidget::onInverseKinematicsAssist()

{

    try {

        auto params = getDHParametersFromTable();

        dh_calculator_->setDHParameters(params);

        

        auto end_effector = dh_calculator_->computeEndEffectorPosition();

        

        QString result = "Inverse Kinematics Assistance:\n\n";

        result += "End Effector Transform:\n";

        result += QString::fromStdString(end_effector.toString()) + "\n\n";

        result += "Note: Post-multiplication and decoupling features not yet implemented for ExprTk backend.\n";

        

        ik_result_->setText(result);

        

    } catch (const std::exception& e) {

        emit errorOccurred(QString("Inverse kinematics assistance failed: %1").arg(e.what()));

    }

}



void DHParameterWidget::onValidateParameters()

{

    validateCurrentParameters();

}



void DHParameterWidget::onExportTransformations()

{

    try {

        auto params = getDHParametersFromTable();

        dh_calculator_->setDHParameters(params);

        

        auto transforms = dh_calculator_->computeAllTransformMatrices();

        

        QString result = "Transformation Matrices:\n\n";

        for (size_t i = 0; i < transforms.size(); ++i) {

            result += QString("T%1:\n").arg(i);

            result += QString::fromStdString(transforms[i].toString()) + "\n\n";

        }

        

        auto cumulative = dh_calculator_->computeEndEffectorPosition();

        result += "Cumulative Transform (End Effector):\n";

        result += QString::fromStdString(cumulative.toString()) + "\n";

        

        QMessageBox::information(this, "Transformations", result);

        

    } catch (const std::exception& e) {

        emit errorOccurred(QString("Export failed: %1").arg(e.what()));

    }

}



void DHParameterWidget::onPresetRobotSelected()

{

    QString preset = preset_combo_->currentText();

    

    QString description;

    if (preset == "2-DOF Planar") {

        description = "2-DOF planar manipulator with all revolute joints in the same plane.";

    } else if (preset == "3-DOF Planar") {

        description = "3-DOF planar manipulator with all revolute joints in the same plane.";

    } else if (preset == "SCARA Robot") {

        description = "Selective Compliance Assembly Robot Arm - 4-DOF with revolute joints and one prismatic.";

    } else if (preset == "Stanford Arm") {

        description = "Stanford Arm - 6-DOF with spherical wrist and prismatic joint.";

    } else if (preset == "PUMA 560") {

        description = "PUMA 560 - Classic 6-DOF industrial robot with spherical wrist.";

    } else if (preset == "KUKA KR6") {

        description = "KUKA KR6 - 6-DOF industrial robot with standard kinematic configuration.";

    } else if (preset == "ABB IRB 120") {

        description = "ABB IRB 120 - Compact 6-DOF industrial robot.";

    } else if (preset == "Custom 6-DOF") {

        description = "Generic 6-DOF configuration template.";

    }

    

    preset_description_->setText(description);

    

    // Apply preset if button was clicked

    if (sender() == apply_preset_btn_) {

        applyPreset(preset);

    }

}

