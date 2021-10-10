#pragma once
#include "NanoVG.hpp"
#include "ui/Geometry.h"
#include <string>
#include <vector>
#include <functional>
class ColorPalette;

class SpinBoxChooser : public NanoSubWidget {
public:
    SpinBoxChooser(Widget *group, const ColorPalette &palette);
    void clearChoices();
    void addChoice(int32_t value, const char *text);

    const std::string &textAtIndex(int32_t index);

    int32_t valueIndex() const;
    void setValueIndex(int32_t index);

    int32_t value() const;
    void setValue(int32_t value);

    std::function<void(int32_t)> ValueChangedCallback;

protected:
    void onNanoDisplay() override;
    void onResize(const ResizeEvent &ev) override;
    bool onMouse(const MouseEvent &ev) override;

private:
    void updateLayout();

private:
    int32_t fValueIndex = 0;
    std::vector<std::pair<int32_t, std::string>> fChoices;
    const ColorPalette &fPalette;
    Rect fBoundsLeftButton;
    Rect fBoundsRightButton;
    Rect fBoundsCenterLabel;
};
