#include <LPointerMoveEvent.h>
#include <LCompositor.h>

using namespace Louvre;

LEvent *LPointerMoveEvent::copy() const
{
    return new LPointerMoveEvent(*this);
}

void LPointerMoveEvent::notify()
{
    if (compositor()->state() == LCompositor::Initialized)
        seat()->pointer()->pointerMoveEvent(*this);
}
