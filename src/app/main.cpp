#include <QApplication>
#include "windows/MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Set application properties
    app.setApplicationName("Symbolic Analyzer Workbench");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("Robotics Research");

    // Create and show main window
    MainWindow window;
    window.show();

    return app.exec();
}
