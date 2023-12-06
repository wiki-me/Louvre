#include <LPointerHoldEndEvent.h>
#include <LCompositor.h>

using namespace Louvre;

LEvent *LPointerHoldEndEvent::copy() const
{
    return new LPointerHoldEndEvent(*this);
}

void LPointerHoldEndEvent::notify()
{
    if (compositor()->state() == LCompositor::Initialized)
        seat()->pointer()->pointerHoldEndEvent(*this);
}
