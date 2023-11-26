#ifndef DOCKITEM_H
#define DOCKITEM_H

#include <LTextureView.h>

class Surface;
class Dock;

using namespace Louvre;

class DockItem : public LTextureView
{
public:
    DockItem(class Surface *surface, Dock *dock);
    ~DockItem();

    void pointerEnterEvent(const LPointerMoveEvent &localPos) override;
    void pointerLeaveEvent(const LPointerMoveEvent &) override;
    void pointerButtonEvent(const LPointerButtonEvent &event) override;

    Dock *dock = nullptr;
    class Surface *surface = nullptr;
};

#endif // DOCKITEM_H
