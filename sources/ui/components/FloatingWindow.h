#pragma once
#include "Widget.hpp"
#include <vector>
class ColorPalette;

class FloatingWindow : public Widget {
public:
    FloatingWindow(Widget *group, ColorPalette &palette);

    void setMoveLimits(DGL::Point<int> origin, DGL::Size<uint> size);
    void repositionWithinLimits();
    void moveAlong(Widget *w);
    void setAllVisible(bool visible);

protected:
    void onDisplay() override;
    void onPositionChanged(const PositionChangedEvent &ev) override;
    bool onMouse(const MouseEvent &ev) override;
    bool onMotion(const MotionEvent &ev) override;

private:
    DGL::Point<int> restrictWithinLimits(DGL::Point<int> pos);

private:
    ColorPalette &fPalette;
    std::vector<Widget *> fMoveAlong;
    bool fIsDragging = false;
    DGL::Point<int> fDragMouseOrigin;
    DGL::Point<int> fDragStartingWindowPos;
    DGL::Point<int> fLimitOrigin;
    DGL::Size<uint> fLimitSize;
};
