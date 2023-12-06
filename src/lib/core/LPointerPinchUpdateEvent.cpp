#include <LPointerPinchUpdateEvent.h>
#include <LCompositor.h>

using namespace Louvre;

LEvent *LPointerPinchUpdateEvent::copy() const
{
    return new LPointerPinchUpdateEvent(*this);
}

void LPointerPinchUpdateEvent::notify()
{
    if (compositor()->state() == LCompositor::Initialized)
        seat()->pointer()->pointerPinchUpdateEvent(*this);
}
