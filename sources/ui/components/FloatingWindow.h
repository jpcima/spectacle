#pragma once
#include "Widget.hpp"
#include <vector>

class FloatingWindow : public Widget {
public:
    explicit FloatingWindow(Widget *group);
    /* TODO */

    void moveAlong(Widget *w);

protected:
    void onDisplay() override;
    void onPositionChanged(const PositionChangedEvent &ev) override;
    bool onMouse(const MouseEvent &ev) override;
    bool onMotion(const MotionEvent &ev) override;

private:
    std::vector<Widget *> fMoveAlong;
    bool fIsDragging = false;
    DGL::Point<int> fDragMouseOrigin;
    DGL::Point<int> fDragStartingWindowPos;
};
