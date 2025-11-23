cmake_minimum_required(VERSION 3.24)

project(SymbolicAnalyzerWorkbench VERSION 1.0.0 LANGUAGES CXX)

enable_testing()

# Suppress FetchContent_Populate deprecation warning
cmake_policy(SET CMP0169 OLD)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# MSVC specific settings for exception handling
if(MSVC)
  add_compile_options(/EHsc /bigobj)
endif()

# --- Qt Configuration ---
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_PREFIX_PATH "C:/Qt/6.9.1/msvc2022_64" CACHE PATH "Path to Qt installation")
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# --- FetchContent for Dependencies ---
include(FetchContent)

set(FETCHCONTENT_BASE_DIR "${CMAKE_SOURCE_DIR}/.cache/fetchcontent" CACHE PATH "Base dir for FetchContent" FORCE)
file(MAKE_DIRECTORY "${FETCHCONTENT_BASE_DIR}")

# --- Fetch Eigen ---
# Disable Eigen tests, docs, and examples
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(EIGEN_BUILD_DOC OFF CACHE BOOL "" FORCE)
set(EIGEN_BUILD_BLAS OFF CACHE BOOL "" FORCE)
set(EIGEN_BUILD_LAPACK OFF CACHE BOOL "" FORCE)
set(EIGEN_BUILD_PKGCONFIG OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
  eigen
  GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
  GIT_TAG 3.4.0
)

# Manually populate to use EXCLUDE_FROM_ALL (hides all Eigen test targets)
FetchContent_GetProperties(eigen)
if(NOT eigen_POPULATED)
  FetchContent_Populate(eigen)
  add_subdirectory(${eigen_SOURCE_DIR} ${eigen_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

# --- Fetch Boost (Headers for SymEngine) ---
# SymEngine with INTEGER_CLASS=boostmp needs Boost headers.
# We use SourceForge which is a very reliable mirror.
FetchContent_Declare(
  Boost
  URL https://downloads.sourceforge.net/project/boost/boost/1.84.0/boost_1_84_0.zip
  DOWNLOAD_EXTRACT_TIMESTAMP ON
)
FetchContent_MakeAvailable(Boost)

# Handle potential subdirectory in Boost archive (boost_1_84_0)
if(EXISTS "${boost_SOURCE_DIR}/boost_1_84_0")
  set(Boost_INCLUDE_DIR "${boost_SOURCE_DIR}/boost_1_84_0" CACHE PATH "" FORCE)
else()
  set(Boost_INCLUDE_DIR "${boost_SOURCE_DIR}" CACHE PATH "" FORCE)
endif()

include_directories(${Boost_INCLUDE_DIR})

# --- Fetch SymEngine ---
# Configure SymEngine to use Boost.Multiprecision (header-only)
set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(BUILD_BENCHMARKS OFF CACHE BOOL "" FORCE)
set(SYMENGINE_USE_GMP OFF CACHE BOOL "" FORCE)
set(SYMENGINE_USE_MPFR OFF CACHE BOOL "" FORCE)
set(SYMENGINE_USE_MPC OFF CACHE BOOL "" FORCE)
set(SYMENGINE_USE_PTHREAD OFF CACHE BOOL "" FORCE)
set(INTEGER_CLASS "boostmp" CACHE STRING "" FORCE)
set(WITH_SYMENGINE_THREAD_SAFE OFF CACHE BOOL "" FORCE) # Disable thread safety to avoid pthread dep

FetchContent_Declare(
  symengine
  GIT_REPOSITORY https://github.com/symengine/symengine.git
  GIT_TAG v0.11.2
)
FetchContent_MakeAvailable(symengine)

# --- Fetch TinyXML2 ---
set(tinyxml2_BUILD_TESTING OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
  tinyxml2
  GIT_REPOSITORY https://github.com/leethomason/tinyxml2.git
  GIT_TAG master
)
# Manually populate to use EXCLUDE_FROM_ALL (hides xmltest target)
FetchContent_GetProperties(tinyxml2)
if(NOT tinyxml2_POPULATED)
  FetchContent_Populate(tinyxml2)
  add_subdirectory(${tinyxml2_SOURCE_DIR} ${tinyxml2_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

# --- Hack: Satisfy MicroTeX's find_package(tinyxml2) ---
# MicroTeX calls find_package(tinyxml2 CONFIG REQUIRED).
# Since we added tinyxml2 via add_subdirectory, the target tinyxml2::tinyxml2 already exists.
# We just need to fool find_package into thinking it found the config file.
set(DUMMY_TINYXML2_DIR "${CMAKE_BINARY_DIR}/dummy_tinyxml2")
file(MAKE_DIRECTORY "${DUMMY_TINYXML2_DIR}")
file(WRITE "${DUMMY_TINYXML2_DIR}/tinyxml2Config.cmake" "# Dummy config file to satisfy find_package\n")
set(tinyxml2_DIR "${DUMMY_TINYXML2_DIR}" CACHE PATH "Path to dummy tinyxml2 config" FORCE)

# --- Fetch MicroTeX ---
FetchContent_Declare(
  microtex
  GIT_REPOSITORY https://github.com/NanoMichael/MicroTeX.git
  GIT_TAG master
)
# MicroTeX options
set(MICROTEX_USE_OPENFONT OFF CACHE BOOL "" FORCE)
set(MICROTEX_USE_CCT OFF CACHE BOOL "" FORCE)
set(QT ON CACHE BOOL "" FORCE) # Enable Qt support
set(BUILD_EXAMPLE OFF CACHE BOOL "" FORCE) # Disable MicroTeX examples

# Manually populate to use EXCLUDE_FROM_ALL (hides LaTeXWin32Sample target)
FetchContent_GetProperties(microtex)
if(NOT microtex_POPULATED)
  FetchContent_Populate(microtex)
  add_subdirectory(${microtex_SOURCE_DIR} ${microtex_BINARY_DIR} EXCLUDE_FROM_ALL)
  
  # Copy MicroTeX font resources to project directory
  # This happens during CMake configuration, so fonts are available for the build
  # but don't need to be committed to Git
  message(STATUS "Copying MicroTeX font resources...")
  file(COPY ${microtex_SOURCE_DIR}/res/
       DESTINATION ${CMAKE_SOURCE_DIR}/resources/microtex/res/)
  message(STATUS "MicroTeX fonts copied to ${CMAKE_SOURCE_DIR}/resources/microtex/res/")
endif()

# --- GoogleTest ---
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG        v1.14.0
)
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
