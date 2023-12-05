#include <LTouchCancelEvent.h>
#include <LCompositor.h>

using namespace Louvre;

LEvent *LTouchCancelEvent::copy() const
{
    return new LTouchCancelEvent(*this);
}

void LTouchCancelEvent::notify()
{
    if (compositor()->state() == LCompositor::Initialized)
        seat()->touch()->touchCancelEvent(*this);
}
