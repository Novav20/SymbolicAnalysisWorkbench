#pragma once

#include <QWidget>
#include "render.h"

class LatexOutputWidget : public QWidget {
    Q_OBJECT

public:
    explicit LatexOutputWidget(QWidget* parent = nullptr);
    ~LatexOutputWidget() override;

    void setLatex(const std::wstring& latex);
    void clear();

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    tex::TeXRender* render_;
    std::wstring latex_;
};
