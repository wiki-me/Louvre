#ifndef INPUTRECT_H
#define INPUTRECT_H

#include <LLayerView.h>

using namespace Louvre;

class InputRect : public LLayerView
{
public:
    InputRect(LView *parent = nullptr, void *userData = nullptr, UInt32 id = 0);

    UInt32 id;
    void *userData;
    void (*onPointerEnter)(InputRect *, void *, const LPoint &localPos) = nullptr;
    void (*onPointerLeave)(InputRect *, void *) = nullptr;
    void (*onPointerMove)(InputRect *, void *, const LPoint &localPos) = nullptr;
    void (*onPointerButton)(InputRect *, void *, const LPointerButtonEvent &) = nullptr;

    void pointerEnterEvent(const LPointerEnterEvent &event) override;
    void pointerLeaveEvent(const LPointerLeaveEvent &) override;
    void pointerMoveEvent(const LPointerMoveEvent &event) override;
    void pointerButtonEvent(const LPointerButtonEvent &event) override;
};

#endif // INPUTRECT_H
