#include <private/LCompositorPrivate.h>
#include <LPointerMoveEvent.h>

Louvre::LPointerMoveEvent::LPointerMoveEvent() {}

bool Louvre::LPointerMoveEvent::isAbsolute() const
{
    return compositor()->imp()->inputBackend->pointerMoveEventGetIsAbsolute(this);
}

Float32 LPointerMoveEvent::x() const
{
    return compositor()->imp()->inputBackend->pointerMoveEventGetX(this);
}

Float32 LPointerMoveEvent::y() const
{
    return compositor()->imp()->inputBackend->pointerMoveEventGetY(this);
}
