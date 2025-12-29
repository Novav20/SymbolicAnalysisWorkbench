# Defines the SymbolicAnalyzerWorkbench GUI application executable
add_executable(SymbolicAnalyzerWorkbench
  main.cpp
  "windows/MainWindow.h" "windows/MainWindow.cpp"
  "widgets/MatrixInputWidget.h" "widgets/MatrixInputWidget.cpp"
  "widgets/LiveResultPanel.h" "widgets/LiveResultPanel.cpp"
  "widgets/DHParameterWidget.h" "widgets/DHParameterWidget.cpp"
  "widgets/LatexOutputWidget.h" "widgets/LatexOutputWidget.cpp"
)

# Link the application against our core library and the Qt Widgets module.
target_link_libraries(SymbolicAnalyzerWorkbench PRIVATE
  SymbolicCore
  Qt6::Widgets
  LaTeX
)

# Copy MicroTeX font resources to the output directory
add_custom_command(TARGET SymbolicAnalyzerWorkbench POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/resources/microtex"
    "$<TARGET_FILE_DIR:SymbolicAnalyzerWorkbench>/resources/microtex"
  COMMENT "Copying MicroTeX font resources..."
)

# --- Runtime Deployment (Qt 6) ---
if(WIN32)
  # Let Qt add the appropriate deployment step for Windows (uses windeployqt internally).
  qt_finalize_executable(SymbolicAnalyzerWorkbench)
endif()
