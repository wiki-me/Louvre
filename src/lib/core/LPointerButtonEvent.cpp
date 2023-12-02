#include <LPointerButtonEvent.h>
#include <LCompositor.h>
#include <LTime.h>

using namespace Louvre;

LPointerButtonEvent::LPointerButtonEvent() :
    LPointerEvent(Subtype::Button, LCompositor::nextSerial(), LTime::ms(), nullptr)
{}

LEvent *LPointerButtonEvent::copy() const
{
    return new LPointerButtonEvent(*this);
}

void LPointerButtonEvent::notify()
{
    if (compositor()->state() == LCompositor::Initialized)
        seat()->pointer()->pointerButtonEvent(*this);
}
