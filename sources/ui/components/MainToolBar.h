#pragma once
#include "Widget.hpp"
#include "ui/Geometry.h"
#include <string>
#include <vector>
#include <memory>
class FontEngine;

class MainToolBar : public Widget {
public:
    MainToolBar(Widget *group, FontEngine &fontEngine);
    void addButton(int id, const char *label, const char *icon);
    void setSelected(int id, bool sel);
    float getIdealWidth() const;

    class Listener {
    public:
        virtual ~Listener() {}
        virtual void onToolBarItemClicked(int id) = 0;
    };

    void setListener(Listener *l) { fListener = l; }

protected:
    void onDisplay() override;
    void onResize(const ResizeEvent &ev) override;
    bool onMouse(const MouseEvent &ev) override;

private:
    void updatePositionsAndSizes();

private:
    struct Item {
        int id = 0;
        int type = 0;
        std::string label;
        std::string icon;
        bool selected = false;
    };

    enum ItemType {
        kTypeIcon,
    };

private:
    FontEngine &fFontEngine;
    std::vector<Item> fLayout;
    std::vector<RectF> fItemRects;
    Listener *fListener = nullptr;
};
