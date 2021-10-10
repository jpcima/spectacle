#include "SpinBoxChooser.h"
#include "ui/FontEngine.h"
#include "ui/NanoVGHelpers.h"
#include "plugin/ColorPalette.h"
#include "Window.hpp"

SpinBoxChooser::SpinBoxChooser(Widget *group, const ColorPalette &palette)
    : NanoSubWidget(group),
      fPalette(palette)
{
    updateLayout();
}

void SpinBoxChooser::clearChoices()
{
    fChoices.clear();
    fValueIndex = 0;
}

void SpinBoxChooser::addChoice(int32_t value, const char *text)
{
    if (text)
        fChoices.emplace_back(value, text);
    else
        fChoices.emplace_back(value, std::to_string(value));
    setValueIndex(fValueIndex);
}

const std::string &SpinBoxChooser::textAtIndex(int32_t index)
{
    if ((uint32_t)index >= fChoices.size()) {
        static const std::string empty;
        return empty;
    }
    return fChoices[(uint32_t)index].second;
}

int32_t SpinBoxChooser::valueIndex() const
{
    return fValueIndex;
}

void SpinBoxChooser::setValueIndex(int32_t index)
{
    int32_t count = fChoices.size();
    index = std::max(0, std::min(count - 1, index));

    if (index == fValueIndex)
        return;

    fValueIndex = index;
    if (ValueChangedCallback)
        ValueChangedCallback(value());
    repaint();
}

int32_t SpinBoxChooser::value() const
{
    if (fValueIndex < 0 || (uint32_t)fValueIndex >= fChoices.size())
        return 0;

    return fChoices[fValueIndex].first;
}

void SpinBoxChooser::setValue(int32_t value)
{
    int32_t i = -1;

    for (uint32_t index = 0, size = fChoices.size(); i == -1 && index < size; ++index) {
        if (fChoices[index].first == value)
            i = index;
    }

    if (i != -1)
        setValueIndex(i);
}

void SpinBoxChooser::onNanoDisplay()
{
    const ColorPalette &cp = fPalette;
    FontEngine fe(*this, cp);

    const int h = getHeight();

    ///
    Font fontAwesome;
    fontAwesome.name = "awesome";
    fontAwesome.size = 0.5 * h;
    fontAwesome.color = cp[Colors::text_normal];
    Font fontRegular;
    fontRegular.name = "regular";
    fontRegular.size = 12.0;
    fontRegular.color = cp[Colors::text_normal];

    ///
    beginPath();
    roundedRectWithCorners(*this, fBoundsLeftButton, 10.0, RectangleNW|RectangleSW);
    fillColor(Colors::fromRGBA8(cp[Colors::spin_box_fill]));
    fill();
    fe.drawInBox("\uf053", fontAwesome, fBoundsLeftButton, kAlignCenter|kAlignInside);

    ///
    beginPath();
    roundedRectWithCorners(*this, fBoundsRightButton, 10.0, RectangleNE|RectangleSE);
    fillColor(Colors::fromRGBA8(cp[Colors::spin_box_fill]));
    fill();
    fe.drawInBox("\uf054", fontAwesome, fBoundsRightButton, kAlignCenter|kAlignInside);

    ///
    beginPath();
    ::rect(*this, fBoundsCenterLabel);
    fillColor(Colors::fromRGBA8(cp[Colors::spin_box_back]));
    fill();
    if (!fChoices.empty())
        fe.drawInBox(fChoices[fValueIndex].second.c_str(), fontRegular, fBoundsCenterLabel, kAlignCenter|kAlignInside);
}

void SpinBoxChooser::onResize(const ResizeEvent &ev)
{
    NanoSubWidget::onResize(ev);
    updateLayout();
}

bool SpinBoxChooser::onMouse(const MouseEvent &ev)
{
    if (ev.press && ev.button == 1) {
        if (fBoundsLeftButton.contains(ev.pos.getX(), ev.pos.getY())) {
            setValueIndex(fValueIndex - 1);
            return true;
        }
        else if (fBoundsRightButton.contains(ev.pos.getX(), ev.pos.getY())) {
            setValueIndex(fValueIndex + 1);
            return true;
        }
    }
    return false;
}

void SpinBoxChooser::updateLayout()
{
    const int w = getWidth();
    const int h = getHeight();

    const Rect bounds{0, 0, w, h};

    fBoundsLeftButton = bounds.from_left(h);
    fBoundsRightButton = bounds.from_right(h);
    fBoundsCenterLabel = bounds.reduced({h, 0});
}
