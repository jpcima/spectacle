#pragma once
#include "Widget.hpp"

class SelectionRectangle : public Widget {
public:
    explicit SelectionRectangle(Widget *group);
    void onDisplay() override;
};
