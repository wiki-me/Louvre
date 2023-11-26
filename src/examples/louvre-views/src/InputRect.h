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
    void (*onPointerButton)(InputRect *, void *, LPointer::Button button, LPointer::ButtonState state) = nullptr;

    void pointerEnterEvent(const LPointerMoveEvent &) override;
    void pointerLeaveEvent(const LPointerMoveEvent &) override;
    void pointerMoveEvent(const LPointerMoveEvent &) override;
    void pointerButtonEvent(const LPointerButtonEvent &event) override;
};

#endif // INPUTRECT_H
