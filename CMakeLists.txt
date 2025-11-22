cmake_minimum_required(VERSION 3.20)

project(SymbolicAnalyzerWorkbench VERSION 1.0.0 LANGUAGES CXX)

enable_testing()

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# MSVC specific settings for exception handling
if(MSVC)
  add_compile_options(/EHsc)
endif()

# --- Qt Configuration ---
# Enable automatic MOC, UIC, and RCC processing
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# This is essential for building Qt applications with CMake.
# The CMAKE_PREFIX_PATH tells CMake where to look for installed packages like Qt.
set(CMAKE_PREFIX_PATH "C:/Qt/6.9.1/msvc2022_64" CACHE PATH "Path to Qt installation")

# Find the Qt6 package and its required Widgets component.
# REQUIRED means CMake will fail with an error if Qt is not found.
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# --- FetchContent for Dependencies ---
include(FetchContent)

# Set the base directory where dependencies will be downloaded and built
# Persist FetchContent downloads across build directory wipes (similar to RoboDesigner)
set(FETCHCONTENT_BASE_DIR "${CMAKE_SOURCE_DIR}/.cache/fetchcontent" CACHE PATH "Base dir for FetchContent" FORCE)
file(MAKE_DIRECTORY "${FETCHCONTENT_BASE_DIR}")

# --- Fetch Eigen ---
# --- Fetch Eigen ---
FetchContent_Declare(
  eigen
  GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
  GIT_TAG 3.4.0
)
FetchContent_Populate(eigen)
# Create our own simple INTERFACE target. We CANNOT use a "::" in the name.
add_library(eigen_interface INTERFACE)
target_include_directories(eigen_interface INTERFACE ${eigen_SOURCE_DIR})

# --- Fetch ExprTk ---
FetchContent_Declare(
  exprtk
  GIT_REPOSITORY https://github.com/ArashPartow/exprtk.git
  GIT_TAG master
)
FetchContent_Populate(exprtk)
# Create interface library for ExprTk
add_library(exprtk_interface INTERFACE)
target_include_directories(exprtk_interface INTERFACE ${exprtk_SOURCE_DIR})

# --- GoogleTest ---
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
)
# Exclude from all to prevent installing
FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
  FetchContent_Populate(googletest)
  add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

# --- SymbolicCore Library ---
add_subdirectory(src/SymbolicCore)

# --- Application Executable ---
add_subdirectory(src/app)

# --- Tests ---
add_subdirectory(tests)
