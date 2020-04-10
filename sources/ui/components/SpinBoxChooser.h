#pragma once
#include "Widget.hpp"
#include "ui/Geometry.h"
#include <string>
#include <vector>
class FontEngine;

class SpinBoxChooser : public Widget {
public:
    SpinBoxChooser(Widget *group, FontEngine &fontEngine);
    void addChoice(int32_t value, const char *text);

    int32_t valueIndex() const;
    void setValueIndex(int32_t index);

protected:
    void onDisplay() override;
    void onResize(const ResizeEvent &ev) override;
    bool onMouse(const MouseEvent &ev) override;

private:
    void updateLayout();

private:
    int32_t fValueIndex = 0;
    std::vector<std::pair<int32_t, std::string>> fChoices;
    FontEngine &fFontEngine;
    Rect fBoundsLeftButton;
    Rect fBoundsRightButton;
    Rect fBoundsCenterLabel;
};
