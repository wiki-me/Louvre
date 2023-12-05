#include <LTouchMoveEvent.h>
#include <LCompositor.h>

using namespace Louvre;

LEvent *LTouchMoveEvent::copy() const
{
    return new LTouchMoveEvent(*this);
}

void LTouchMoveEvent::notify()
{
    if (compositor()->state() == LCompositor::Initialized)
        seat()->touch()->touchMoveEvent(*this);
}
