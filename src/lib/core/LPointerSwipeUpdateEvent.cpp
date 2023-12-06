#include <LPointerSwipeUpdateEvent.h>
#include <LCompositor.h>

using namespace Louvre;

LEvent *LPointerSwipeUpdateEvent::copy() const
{
    return new LPointerSwipeUpdateEvent(*this);
}

void LPointerSwipeUpdateEvent::notify()
{
    if (compositor()->state() == LCompositor::Initialized)
        seat()->pointer()->pointerSwipeUpdateEvent(*this);
}
