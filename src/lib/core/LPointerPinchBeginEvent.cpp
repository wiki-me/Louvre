#include <LPointerPinchBeginEvent.h>
#include <LCompositor.h>

using namespace Louvre;

LEvent *LPointerPinchBeginEvent::copy() const
{
    return new LPointerPinchBeginEvent(*this);
}

void LPointerPinchBeginEvent::notify()
{
    if (compositor()->state() == LCompositor::Initialized)
        seat()->pointer()->pointerPinchBeginEvent(*this);
}
