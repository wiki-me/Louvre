#include <LPointerButtonEvent.h>
#include <LCompositor.h>

using namespace Louvre;

LEvent *LPointerButtonEvent::copy() const
{
    return new LPointerButtonEvent(*this);
}

void LPointerButtonEvent::notify()
{
    if (compositor()->state() == LCompositor::Initialized)
        seat()->pointer()->pointerButtonEvent(*this);
}
