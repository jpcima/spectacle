#pragma once
#include "NanoVG.hpp"
#include <functional>
class ColorPalette;

class ResizeHandle : public NanoWidget {
public:
    ResizeHandle(Widget *group, const ColorPalette &palette);

    bool onMouse(const MouseEvent &event) override;
    bool onMotion(const MotionEvent &event) override;
    void onNanoDisplay() override;

    std::function<void(DGL::Size<uint>)> RequestToResizeCallback;

private:
    const ColorPalette &fPalette;
    DGL::Size<uint> fMinWindowSize { 50u, 50u };
    bool fIsDragging = false;
};
