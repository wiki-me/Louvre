#include <private/LCompositorPrivate.h>
#include <LPointerButtonEvent.h>

using namespace Louvre;

LPointerButtonEvent::LPointerButtonEvent() {}
LPointerButtonEvent::~LPointerButtonEvent() {}

void LPointerButtonEvent::notify()
{
    if (compositor()->state() == LCompositor::Initialized)
        seat()->pointer()->pointerButtonEvent(this);
}
