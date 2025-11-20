# Symbolic Analysis Workbench

A C++ desktop application for the symbolic kinematic analysis of robotic manipulators. Built with Qt and ExprTk, it features a custom symbolic engine to define Denavit-Hartenberg parameters, calculate Forward Kinematics matrices, and compute geometric Jacobians in real-time.

## 🚀 Features

*   **Symbolic Math Engine:** Powered by [ExprTk](https://github.com/ArashPartow/exprtk), allowing for fast and flexible symbolic expression parsing and evaluation without heavy external dependencies.
*   **DH Parameter Interface:** Intuitive table-based input for standard Denavit-Hartenberg parameters ($\theta$, $d$, $a$, $\alpha$).
*   **Forward Kinematics:** Automatically computes symbolic transformation matrices for each link and the final end-effector pose.
*   **Jacobian Analysis:** Calculates the symbolic geometric Jacobian matrix for velocity analysis.
*   **Robot Presets:** Includes built-in configurations for common robots like PUMA 560, SCARA, and Stanford Arm.
*   **Modern GUI:** Built with Qt 6 for a responsive and native user experience.

## 🛠️ Tech Stack

*   **Language:** C++20
*   **GUI Framework:** Qt 6.x (Widgets)
*   **Build System:** CMake
*   **Math Backend:** ExprTk (Header-only, fetched via CMake)
*   **Testing:** GoogleTest

## 📦 Build Instructions

### Prerequisites
*   C++20 compatible compiler (MSVC 2022 recommended on Windows)
*   CMake 3.20 or higher
*   Qt 6.x installed and configured

### Steps

1.  **Clone the repository:**
    ```bash
    git clone [<repository-url>](https://github.com/Novav20/SymbolicAnalysisWorkbench.git)
    cd SymbolicAnalysisWorkbench
    ```

2.  **Configure with CMake:**
    ```bash
    cmake -S . -B build -DCMAKE_PREFIX_PATH="C:/Qt/6.x.x/msvc2019_64"
    ```
    *(Replace the Qt path with your actual installation path)*

3.  **Build:**
    ```bash
    cmake --build build --config Release
    ```

4.  **Run:**
    The executable will be located in `src/app/Release/SymbolicAnalysisWorkbench.exe` (on Windows).

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
