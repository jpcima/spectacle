#pragma once
#include "Widget.hpp"
#include <vector>

class FloatingWindow : public Widget {
public:
    explicit FloatingWindow(Widget *group);

    void setMoveLimits(DGL::Point<int> origin, DGL::Size<uint> size);
    void repositionWithinLimits();
    void moveAlong(Widget *w);

protected:
    void onDisplay() override;
    void onPositionChanged(const PositionChangedEvent &ev) override;
    bool onMouse(const MouseEvent &ev) override;
    bool onMotion(const MotionEvent &ev) override;

private:
    DGL::Point<int> restrictWithinLimits(DGL::Point<int> pos);

private:
    std::vector<Widget *> fMoveAlong;
    bool fIsDragging = false;
    DGL::Point<int> fDragMouseOrigin;
    DGL::Point<int> fDragStartingWindowPos;
    DGL::Point<int> fLimitOrigin;
    DGL::Size<uint> fLimitSize;
};
