#include <LCursor.h>
#include "InputRect.h"

InputRect::InputRect(LView *parent, void *userData, UInt32 id) : LLayerView(parent)
{
    this->id = id;
    this->userData = userData;
    enableBlockPointer(true);
    enableInput(true);
}

void InputRect::pointerEnterEvent(const LPointerMoveEvent &)
{
    if (onPointerEnter)
        onPointerEnter(this, userData, cursor()->pos() - pos());
}

void InputRect::pointerLeaveEvent(const LPointerMoveEvent &)
{
    if (onPointerLeave)
        onPointerLeave(this, userData);
}

void InputRect::pointerMoveEvent(const LPointerMoveEvent &)
{
    if (onPointerMove)
        onPointerMove(this, userData, cursor()->pos() - pos());
}

void InputRect::pointerButtonEvent(LPointer::Button button, LPointer::ButtonState state)
{
    if (onPointerButton)
        onPointerButton(this, userData, button, state);
}
