#include <LTouchUpEvent.h>
#include <LCompositor.h>

using namespace Louvre;

LEvent *LTouchUpEvent::copy() const
{
    return new LTouchUpEvent(*this);
}

void LTouchUpEvent::notify()
{
    if (compositor()->state() == LCompositor::Initialized)
        seat()->touch()->touchUpEvent(*this);
}
