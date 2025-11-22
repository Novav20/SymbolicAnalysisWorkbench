# Define the SymbolicCore static library
add_library(SymbolicCore STATIC
    "${CMAKE_SOURCE_DIR}/include/SymbolicCore/SymbolicMatrix.h"
    "${CMAKE_SOURCE_DIR}/include/SymbolicCore/SymbolicScalar.h"
    "${CMAKE_SOURCE_DIR}/include/SymbolicCore/DHMatrixCalculator.h"
    "DHMatrixCalculator.cpp"
    "SymbolicScalar.cpp"
    "SymbolicMatrix.cpp"
)

# Add include directories
target_include_directories(SymbolicCore PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

# Link against Eigen and ExprTk
target_link_libraries(SymbolicCore PUBLIC Eigen3::Eigen exprtk_interface)

# Add /bigobj flag for MSVC to handle ExprTk's large templates
if(MSVC)
    target_compile_options(SymbolicCore PRIVATE /bigobj)
endif()

