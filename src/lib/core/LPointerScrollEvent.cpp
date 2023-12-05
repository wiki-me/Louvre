#include <LPointerScrollEvent.h>
#include <LCompositor.h>

using namespace Louvre;

LEvent *LPointerScrollEvent::copy() const
{
    return new LPointerScrollEvent(*this);
}

void LPointerScrollEvent::notify()
{
    if (compositor()->state() == LCompositor::Initialized)
        seat()->pointer()->pointerScrollEvent(*this);
}
