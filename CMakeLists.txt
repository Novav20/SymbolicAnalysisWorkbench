cmake_minimum_required(VERSION 3.20)

project(SymbolicAnalysisWorkbench VERSION 1.0.0 LANGUAGES CXX)

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
FetchContent_Declare(
  eigen
  GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
  GIT_TAG 3.4.0
)
FetchContent_MakeAvailable(eigen)

# --- Fetch ExprTk (Header-only symbolic math library) ---
FetchContent_Declare(
  exprtk
  URL https://github.com/ArashPartow/exprtk/archive/refs/heads/master.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(exprtk)

# Create our own simple INTERFACE target.
# Since we used MakeAvailable, Eigen's CMakeLists.txt is processed and defines Eigen3::Eigen.
if(TARGET Eigen3::Eigen)
    # Eigen3::Eigen might be an ALIAS itself, so we cannot ALIAS it again.
    # Instead, create an interface library that links to it.
    add_library(eigen_interface INTERFACE)
    target_link_libraries(eigen_interface INTERFACE Eigen3::Eigen)
else()
    # Fallback in case the target isn't defined as expected
    add_library(eigen_interface INTERFACE)
    target_include_directories(eigen_interface INTERFACE ${eigen_SOURCE_DIR})
endif()

# --- GLM & GoogleTest (Well-Behaved Dependencies) ---
# Temporarily disable GLM to avoid download issues
# FetchContent_Declare(
#     glm
#     GIT_REPOSITORY https://github.com/g-truc/glm.git
#     GIT_TAG        0.9.9.8
# )

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
)

# Prevent GoogleTest from building its own test samples
set(INSTALL_GTEST OFF)
set(FETCHCONTENT_ADD_SUBDIRECTORY_EXCLUDE_FROM_ALL TRUE)
FetchContent_MakeAvailable(googletest)

# --- SymbolicCore Library ---
add_subdirectory(src/SymbolicCore)

# --- Application Executable ---
add_subdirectory(src/app)
