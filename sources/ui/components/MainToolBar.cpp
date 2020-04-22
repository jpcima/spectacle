#include "MainToolBar.h"
#include "ui/FontEngine.h"
#include "ui/Cairo++.h"
#include "plugin/ColorPalette.h"
#include "Window.hpp"
#include "Cairo.hpp"

MainToolBar::MainToolBar(Widget *group, FontEngine &fontEngine, ColorPalette &palette)
    : Widget(group),
      fFontEngine(fontEngine),
      fPalette(palette)
{
}

void MainToolBar::addButton(int id, const char *label, const char *icon)
{
    Item item;
    item.id = id;
    item.type = kTypeIcon;
    item.label = label;
    item.icon = icon;
    fLayout.push_back(std::move(item));
    updatePositionsAndSizes();
    repaint();
}

void MainToolBar::setSelected(int id, bool sel)
{
    Item *item = nullptr;

    for (size_t i = 0, n = fLayout.size(); i < n && !item; ++i) {
        if (fLayout[i].id == id)
            item = &fLayout[i];
    }

    if (item && item->selected != sel) {
        item->selected = sel;
        repaint();
    }
}

float MainToolBar::getIdealWidth() const
{
    if (fLayout.empty())
        return 0.0;

    const RectF &rect = fItemRects.back();
    return rect.x + rect.w + 2.0;
}

void MainToolBar::onDisplay()
{
    cairo_t *cr = getParentWindow().getGraphicsContext().cairo;
    FontEngine &fe = fFontEngine;
    const ColorPalette &cp = fPalette;

    const double w = getWidth();
    const double h = getHeight();

    cairo_set_source_rgba8(cr, cp[Colors::tool_bar_back]);
    cairo_rounded_rectangle_with_corners(cr, RectF{0.0, 0.0, w, h}, 10.0, RectangleSE);
    cairo_fill(cr);

    Font fontLabel;
    fontLabel.name = "regular";
    fontLabel.size = 12.0;
    fontLabel.color = cp[Colors::text_normal];
    Font fontIcons;
    fontIcons.name = "awesome";
    fontIcons.size = h - 1.5 * fontLabel.size;
    fontIcons.color = cp[Colors::text_normal];

    Font fontLabelSelected = fontLabel;
    Font fontIconsSelected = fontIcons;
    const ColorRGBA8 selectionColor = cp[Colors::text_active];
    fontLabelSelected.color = selectionColor;
    fontIconsSelected.color = selectionColor;

    for (size_t i = 0, n = fLayout.size(); i < n; ++i) {
        const Item &item = fLayout[i];
        RectF rect = fItemRects[i];

        switch (item.type) {
        case kTypeIcon:
            fe.drawInBox(cr, item.icon.c_str(), !item.selected ? fontIcons : fontIconsSelected, rect, kAlignCenter|kAlignTop|kAlignInside);
            fe.drawInBox(cr, item.label.c_str(), !item.selected ? fontLabel : fontLabelSelected, rect, kAlignCenter|kAlignBottom|kAlignInside);
            break;
        }
    }
}

void MainToolBar::onResize(const ResizeEvent &ev)
{
    (void)ev;
    updatePositionsAndSizes();
    repaint();
}

bool MainToolBar::onMouse(const MouseEvent &ev)
{
    if (ev.press && ev.button == 1) {
        for (size_t i = 0, n = fLayout.size(); i < n; ++i) {
            const Item &item = fLayout[i];
            const RectF rect = fItemRects[i];

            const int evX = ev.pos.getX();
            const int evY = ev.pos.getY();

            const bool inside = evX >= rect.x && evX < rect.x + rect.w &&
                evY >= rect.y && evY < rect.y + rect.h;

            switch (item.type) {
            case kTypeIcon:
                if (inside) {
                    if (fListener)
                        fListener->onToolBarItemClicked(item.id);
                    return true;
                }
                break;
            }
        }
    }

    return false;
}

void MainToolBar::updatePositionsAndSizes()
{
    //const double w = getWidth();
    const double h = getHeight();

    fItemRects.clear();

    double x = 0.0;
    for (const Item &item : fLayout) {
        RectF rect;
        rect.x = x;
        rect.y = 0.0;

        switch (item.type) {
        default:
        case kTypeIcon:
            rect.w = h;
            rect.h = h;
            break;
        }

        rect = rect.reduced(2.0);

        x += rect.w;
        fItemRects.push_back(rect);
    }
}
