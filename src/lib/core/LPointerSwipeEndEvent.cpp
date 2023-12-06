#include <LPointerSwipeEndEvent.h>
#include <LCompositor.h>

using namespace Louvre;

LEvent *LPointerSwipeEndEvent::copy() const
{
    return new LPointerSwipeEndEvent(*this);
}

void LPointerSwipeEndEvent::notify()
{
    if (compositor()->state() == LCompositor::Initialized)
        seat()->pointer()->pointerSwipeEndEvent(*this);
}
