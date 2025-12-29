#include "LatexOutputWidget.h"
#include "latex.h"
#include "platform/qt/graphic_qt.h"
#include <QPainter>
#include <QCoreApplication>

LatexOutputWidget::LatexOutputWidget(QWidget* parent)
    : QWidget(parent), render_(nullptr)
{
    // Initialize MicroTeX once with physical font files
    static bool initialized = false;
    if (!initialized) {
        // Get the application directory and construct path to font resources
        QString appDir = QCoreApplication::applicationDirPath();
        QString resPath = appDir + "/resources/microtex/res";
        
        // Initialize MicroTeX with the physical font directory
        tex::LaTeX::init(resPath.toStdString());
        initialized = true;
    }
}

LatexOutputWidget::~LatexOutputWidget() {
    if (render_) {
        delete render_;
    }
}

void LatexOutputWidget::setLatex(const std::wstring& latex) {
    latex_ = latex;
    if (render_) {
        delete render_;
        render_ = nullptr;
    }
    
    if (!latex.empty()) {
        try {
            // Parse LaTeX string
            // tex::LaTeX::parse(string, width, textSize, lineSpace, color)
            render_ = tex::LaTeX::parse(
                latex, 
                width() - 20, // width
                20.f,         // text size
                10.f,         // line space (approx half text size)
                0xffffffff    // color (ARGB: fully opaque white)
            );
            
            // Set render foreground to white for dark theme
            if (render_) {
                render_->setForeground(0xffffffff);
            }
        } catch (const std::exception& e) {
            // MicroTeX parsing failed - render will remain nullptr
            // The widget will just show nothing rather than crash
            render_ = nullptr;
        }
    }
    
    update(); // Trigger repaint
}

void LatexOutputWidget::clear() {
    setLatex(L"");
}

void LatexOutputWidget::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    if (render_) {
        // Draw centered
        int x = (width() - render_->getWidth()) / 2;
        int y = (height() - render_->getHeight()) / 2;
        
        // Use Qt-specific Graphics2D implementation
        tex::Graphics2D_qt graphics(&painter);
        render_->draw(graphics, x, y);
    }
}
