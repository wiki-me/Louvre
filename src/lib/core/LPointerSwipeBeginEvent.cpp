#include <LPointerSwipeBeginEvent.h>
#include <LCompositor.h>

using namespace Louvre;

LEvent *LPointerSwipeBeginEvent::copy() const
{
    return new LPointerSwipeBeginEvent(*this);
}

void LPointerSwipeBeginEvent::notify()
{
    if (compositor()->state() == LCompositor::Initialized)
        seat()->pointer()->pointerSwipeBeginEvent(*this);
}
