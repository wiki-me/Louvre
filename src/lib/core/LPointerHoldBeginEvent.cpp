#include <LPointerHoldBeginEvent.h>
#include <LCompositor.h>

using namespace Louvre;

LEvent *LPointerHoldBeginEvent::copy() const
{
    return new LPointerHoldBeginEvent(*this);
}

void LPointerHoldBeginEvent::notify()
{
    if (compositor()->state() == LCompositor::Initialized)
        seat()->pointer()->pointerHoldBeginEvent(*this);
}
