#include <LPointerPinchEndEvent.h>
#include <LCompositor.h>

using namespace Louvre;

LEvent *LPointerPinchEndEvent::copy() const
{
    return new LPointerPinchEndEvent(*this);
}

void LPointerPinchEndEvent::notify()
{
    if (compositor()->state() == LCompositor::Initialized)
        seat()->pointer()->pointerPinchEndEvent(*this);
}
