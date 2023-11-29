#include <LPointerButtonEvent.h>
#include <LCompositor.h>

using namespace Louvre;

LPointerButtonEvent::LPointerButtonEvent() :
    LInputEvent(LInputEvent::PointerButton) {}

LPointerButtonEvent::~LPointerButtonEvent() {}

void LPointerButtonEvent::notify()
{
    if (compositor()->state() == LCompositor::Initialized)
        seat()->pointer()->pointerButtonEvent(*this);
}
