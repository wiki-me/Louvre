#ifndef TOPBAR_H
#define TOPBAR_H

#include <LLayerView.h>
#include <LSolidColorView.h>

class Output;

using namespace Louvre;

class Topbar : public LLayerView
{
public:
    Topbar(Output *output);
    ~Topbar();

    void update();

    Output *output;

    // White bar
    LSolidColorView *background;

    // Louvre logo
    LTextureView *logo = nullptr;

    // Clock text
    LTextureView *clock = nullptr;

    // Current app title
    LTextureView *appName = nullptr;

    void pointerEnterEvent(const LPointerMoveEvent &) override;
    void pointerMoveEvent(const LPointerMoveEvent &) override;
};

#endif // TOPBAR_H
