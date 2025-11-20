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
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTableWidgetItem>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QScrollArea>
#include <vector>
#include <memory>

#include "SymbolicCore/DHMatrixCalculator.h"

/**
 * @class DHParameterWidget
 * @brief Widget for managing Denavit-Hartenberg parameters and computing symbolic transformations
 * 
 * This widget provides:
 * - DH parameter input and management
 * - Symbolic transformation matrix computation
 * - Forward kinematics calculation
 * - Inverse kinematics assistance
 * - Workspace analysis
 */
class DHParameterWidget : public QWidget {
    Q_OBJECT

private slots:
    void onAddJoint();
    void onRemoveJoint();
    void onClearAll();
    void onParameterCellChanged(int row, int col);
    void onComputeForwardKinematics();
    void onComputeJacobian();
    void onComputeWorkspace();
    void onInverseKinematicsAssist();
    void onPresetRobotSelected();
    void onExportTransformations();
    void onValidateParameters();
    void onComputeSingleTransform();

private:
    void setupUI();
    void setupParametersTab();
    void setupKinematicsTab();
    void setupInverseKinematicsTab();
    void setupWorkspaceTab();
    void setupPresetsTab();
    void connectSignals();
    void updateParameterTable();
    void updateJointCount();
    std::vector<SymbolicCore::DHParameters> getDHParametersFromTable() const;
    SymbolicCore::DHParameters getDHParametersFromRow(int row) const;
    void setDHParametersToRow(int row, const SymbolicCore::DHParameters& params);
    void displayTransformationMatrix(const SymbolicCore::SymbolicMatrix& matrix);
    void displayJacobian(const SymbolicCore::SymbolicMatrix& jacobian);
    void displayWorkspaceInfo(const std::vector<std::vector<double>>& workspace);
    void validateCurrentParameters();
    void applyPreset(const QString& preset_name);

    // Main layout
    QVBoxLayout* main_layout_;
    QTabWidget* tab_widget_;
    
    // Parameters Tab
    QWidget* parameters_tab_;
    QVBoxLayout* parameters_layout_;
    
    // Joint management
    QGroupBox* joint_management_group_;
    QHBoxLayout* joint_management_layout_;
    QPushButton* add_joint_btn_;
    QPushButton* remove_joint_btn_;
    QPushButton* clear_all_btn_;
    QLabel* joint_count_label_;
    
    // DH parameter table
    QGroupBox* dh_table_group_;
    QVBoxLayout* dh_table_layout_;
    QScrollArea* dh_table_scroll_;
    QTableWidget* dh_parameter_table_;
    
    // Parameter controls
    QGroupBox* parameter_controls_group_;
    QHBoxLayout* parameter_controls_layout_;
    QPushButton* validate_params_btn_;
    QPushButton* compute_transform_btn_;
    QPushButton* export_transforms_btn_;
    
    // Kinematics Tab
    QWidget* kinematics_tab_;
    QVBoxLayout* kinematics_layout_;
    
    // Forward kinematics
    QGroupBox* forward_kinematics_group_;
    QVBoxLayout* forward_kinematics_layout_;
    QPushButton* compute_fk_btn_;
    QTextEdit* fk_result_;
    
    // Jacobian computation
    QGroupBox* jacobian_group_;
    QVBoxLayout* jacobian_layout_;
    QPushButton* compute_jacobian_btn_;
    QTextEdit* jacobian_result_;
    
    // Inverse Kinematics Tab
    QWidget* inverse_kinematics_tab_;
    QVBoxLayout* inverse_kinematics_layout_;
    
    // IK assistance
    QGroupBox* ik_assistance_group_;
    QVBoxLayout* ik_assistance_layout_;
    QLabel* ik_description_;
    QGroupBox* target_pose_group_;
    QVBoxLayout* target_pose_layout_;
    QTextEdit* target_pose_edit_;
    QPushButton* ik_assist_btn_;
    QTextEdit* ik_result_;
    
    // Workspace Tab
    QWidget* workspace_tab_;
    QVBoxLayout* workspace_layout_;
    
    QGroupBox* workspace_analysis_group_;
    QVBoxLayout* workspace_analysis_layout_;
    QPushButton* compute_workspace_btn_;
    QTextEdit* workspace_result_;
    
    // Presets Tab
    QWidget* presets_tab_;
    QVBoxLayout* presets_layout_;
    
    QGroupBox* robot_presets_group_;
    QVBoxLayout* robot_presets_layout_;
    QComboBox* preset_combo_;
    QLabel* preset_description_;
    QPushButton* apply_preset_btn_;
    
    // Core calculator
    std::unique_ptr<SymbolicCore::DHMatrixCalculator> dh_calculator_;
    
    // Current state
    bool parameters_modified_;

public:
    explicit DHParameterWidget(QWidget *parent = nullptr);
    ~DHParameterWidget();

    // Public interface
    void clear();
    void setDHParameters(const std::vector<SymbolicCore::DHParameters>& params);
    std::vector<SymbolicCore::DHParameters> getDHParameters() const;
    SymbolicCore::DHMatrixCalculator* getCalculator() const;

signals:
    void parametersUpdated();
    void forwardKinematicsComputed(const SymbolicCore::SymbolicMatrix& end_effector);
    void jacobianComputed(const SymbolicCore::SymbolicMatrix& jacobian);
    void errorOccurred(const QString& error);
};
