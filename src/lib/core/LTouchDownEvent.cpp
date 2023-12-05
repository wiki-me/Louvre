#include <LTouchDownEvent.h>
#include <LCompositor.h>

using namespace Louvre;

LEvent *LTouchDownEvent::copy() const
{
    return new LTouchDownEvent(*this);
}

void LTouchDownEvent::notify()
{
    if (compositor()->state() == LCompositor::Initialized)
        seat()->touch()->touchDownEvent(*this);
}
