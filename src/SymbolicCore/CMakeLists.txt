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
    ${symengine_SOURCE_DIR}
    ${symengine_BINARY_DIR}
    ${symengine_BINARY_DIR}/symengine/utilities/teuchos
)

# Link against Eigen and SymEngine
target_link_libraries(SymbolicCore PUBLIC Eigen3::Eigen symengine)

# Add /bigobj flag for MSVC (SymEngine templates might also need it)
if(MSVC)
    target_compile_options(SymbolicCore PRIVATE /bigobj)
endif()

