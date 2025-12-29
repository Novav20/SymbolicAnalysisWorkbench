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

# --- Conda Environment Setup ---
# Tell CMake to search for packages in the Conda environment FIRST
if(DEFINED ENV{CONDA_PREFIX})
    list(APPEND CMAKE_PREFIX_PATH $ENV{CONDA_PREFIX})
    list(APPEND CMAKE_PREFIX_PATH $ENV{CONDA_PREFIX}/lib/cmake)
    link_directories($ENV{CONDA_PREFIX}/lib)
endif()

# --- Runtime Search Path Configuration ---
set(CMAKE_BUILD_RPATH "$ENV{CONDA_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH "$ENV{CONDA_PREFIX}/lib")
set(CMAKE_BUILD_RPATH_USE_ORIGIN ON)

# --- Qt Configuration ---
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

# --- Dependencies (System Level) ---
find_package(Eigen3 3.4 REQUIRED)
find_package(GTest REQUIRED)

# --- FetchContent for Dependencies ---
include(FetchContent)

set(FETCHCONTENT_BASE_DIR "${CMAKE_SOURCE_DIR}/.cache/fetchcontent" CACHE PATH "Base dir for FetchContent" FORCE)
file(MAKE_DIRECTORY "${FETCHCONTENT_BASE_DIR}")

# --- Find Boost (Headers for SymEngine) ---
find_package(Boost REQUIRED)
set(Boost_INCLUDE_DIR ${Boost_INCLUDE_DIRS})

# --- Find SymEngine (Manual) ---
find_path(SYMENGINE_INCLUDE_DIR NAMES symengine/basic.h)
find_library(SYMENGINE_LIBRARY NAMES symengine)
find_library(FLINT_LIBRARY NAMES flint) # SymEngine depends on Flint

if(SYMENGINE_INCLUDE_DIR AND SYMENGINE_LIBRARY)
    message(STATUS "Found SymEngine: ${SYMENGINE_LIBRARY}")
    if(FLINT_LIBRARY)
        message(STATUS "Found Flint: ${FLINT_LIBRARY}")
    endif()

    # Define a clean target to link against
    if(NOT TARGET symengine)
        add_library(symengine UNKNOWN IMPORTED)
        set_target_properties(symengine PROPERTIES
            IMPORTED_LOCATION "${SYMENGINE_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${SYMENGINE_INCLUDE_DIR}"
        )
        if(FLINT_LIBRARY)
            set_property(TARGET symengine APPEND PROPERTY INTERFACE_LINK_LIBRARIES "${FLINT_LIBRARY}")
        endif()
    endif()
else()
    message(FATAL_ERROR "SymEngine not found! Please install it (e.g. mamba install symengine)")
endif()

# --- Find TinyXML2 ---
find_package(tinyxml2 REQUIRED)

# --- Fetch MicroTeX ---
FetchContent_Declare(
  microtex
  GIT_REPOSITORY https://github.com/NanoMichael/MicroTeX.git
  GIT_TAG master
  GIT_SHALLOW ON
  GIT_PROGRESS ON
)
# MicroTeX options
set(MICROTEX_USE_OPENFONT OFF CACHE BOOL "" FORCE)
set(MICROTEX_USE_CCT OFF CACHE BOOL "" FORCE)
set(QT ON CACHE BOOL "" FORCE)
set(BUILD_EXAMPLE OFF CACHE BOOL "" FORCE)

# Manually populate to use EXCLUDE_FROM_ALL
FetchContent_GetProperties(microtex)
if(NOT microtex_POPULATED)
  FetchContent_Populate(microtex)
  add_subdirectory(${microtex_SOURCE_DIR} ${microtex_BINARY_DIR} EXCLUDE_FROM_ALL)
  
  message(STATUS "Copying MicroTeX font resources...")
  file(COPY ${microtex_SOURCE_DIR}/res/
       DESTINATION ${CMAKE_SOURCE_DIR}/resources/microtex/res/)
  message(STATUS "MicroTeX fonts copied to ${CMAKE_SOURCE_DIR}/resources/microtex/res/")
endif()

# --- SymbolicCore Library ---
add_subdirectory(src/SymbolicCore)

# --- Application Executable ---
add_subdirectory(src/app)

# --- Tests ---
add_subdirectory(tests)
