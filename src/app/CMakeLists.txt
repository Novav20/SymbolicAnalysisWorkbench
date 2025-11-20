# Defines the SymbolicAnalysisWorkbench GUI application executable
add_executable(SymbolicAnalysisWorkbench
  main.cpp
  "windows/MainWindow.h" "windows/MainWindow.cpp"
  "widgets/MatrixInputWidget.h" "widgets/MatrixInputWidget.cpp"
  "widgets/SymbolicResultWidget.h" "widgets/SymbolicResultWidget.cpp"
  "widgets/DHParameterWidget.h" "widgets/DHParameterWidget.cpp"
)

# Link the application against our core library and the Qt Widgets module.
target_link_libraries(SymbolicAnalysisWorkbench PRIVATE
  SymbolicCore
  Qt6::Widgets
)

# --- Runtime Deployment (Qt 6) ---
if(WIN32)
  # Let Qt add the appropriate deployment step for Windows (uses windeployqt internally).
  qt_finalize_executable(SymbolicAnalysisWorkbench)
endif()
