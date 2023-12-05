#include <LTouchFrameEvent.h>
#include <LCompositor.h>

using namespace Louvre;

LEvent *LTouchFrameEvent::copy() const
{
    return new LTouchFrameEvent(*this);
}

void LTouchFrameEvent::notify()
{
    if (compositor()->state() == LCompositor::Initialized)
        seat()->touch()->touchFrameEvent(*this);
}
