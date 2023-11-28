#ifndef POINTER_H
#define POINTER_H

#include <LPointer.h>

using namespace Louvre;

class Pointer : public LPointer
{
public:
    Pointer(Params *params);
    void pointerMoveEvent(const LPointerMoveEvent &event) override;
    void pointerButtonEvent(const LPointerButtonEvent &event) override;
    void pointerScrollEvent(const LPointerScrollEvent &event) override;

    void setCursorRequest(LCursorRole *cursorRole) override;

    LView *cursorOwner = nullptr;
};

#endif // POINTER_H
