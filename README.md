# Symbolic Analyzer Workbench

A C++ desktop application for the symbolic kinematic analysis of robotic manipulators. Built with Qt and ExprTk, it features a custom symbolic engine to define Denavit-Hartenberg parameters, calculate Forward Kinematics matrices, and compute geometric Jacobians in real-time.

## 🚀 Features

*   **Symbolic Math Engine:** Powered by [SymEngine](https://github.com/symengine/symengine), a fast symbolic manipulation library.
*   **DH Parameter Interface:** Intuitive table-based input for standard Denavit-Hartenberg parameters ($\theta$, $d$, $a$, $\alpha$).
*   **Forward Kinematics:** Automatically computes symbolic transformation matrices for each link and the final end-effector pose.
*   **Jacobian Analysis:** Calculates the symbolic geometric Jacobian matrix for velocity analysis.
*   **Robot Presets:** Includes built-in configurations for common robots like PUMA 560, SCARA, and Stanford Arm.
*   **Modern GUI:** Built with Qt 6 for a responsive and native user experience.

## 🛠️ Tech Stack

*   **Language:** C++20
*   **GUI Framework:** Qt 6.x (Widgets)
*   **Build System:** CMake
*   **Math Backend:** SymEngine
*   **Dependencies:** Boost, Eigen3, TinyXML2, MicroTeX
*   **Testing:** GoogleTest

## 📦 Build Instructions

### Prerequisites
*   **C++20 compatible compiler** (GCC on Linux, MSVC on Windows)
*   **CMake 3.20+**
*   **Conda Environment** (Recommended): We recommend using the `robodesigner_env` or similar containing:
    *   `qt6-main`, `eigen`, `symengine`, `boost`, `tinyxml2`, `flint`, `gtest`

### Steps

1.  **Clone the repository:**
    ```bash
    git clone https://github.com/Novav20/SymbolicAnalyzerWorkbench.git
    cd SymbolicAnalyzerWorkbench
    ```

2.  **Configure with CMake:**
    ```bash
    # Activate your conda environment first
    conda activate robodesigner_env
    
    cmake -S . -B build
    ```

3.  **Build:**
    ```bash
    cmake --build build --parallel
    ```

4.  **Run:**
    ```bash
    ./build/src/app/SymbolicAnalyzerWorkbench
    ```

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
