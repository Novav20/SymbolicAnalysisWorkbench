# Test executable for SymbolicCore functionality
add_executable(SymbolicWorkbenchTests
    "main_test.cpp"
    "prismatic_test.cpp"
)

target_link_libraries(SymbolicWorkbenchTests PRIVATE 
    SymbolicCore 
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(SymbolicWorkbenchTests)
