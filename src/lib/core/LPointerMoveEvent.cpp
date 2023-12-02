#include <LCompositor.h>
#include <LPointerMoveEvent.h>
#include <LTime.h>

using namespace Louvre;

LPointerMoveEvent::LPointerMoveEvent() :
    LPointerEvent(Subtype::Move, LCompositor::nextSerial(), LTime::ms(), nullptr)
{}

LEvent *LPointerMoveEvent::copy() const
{
    return new LPointerMoveEvent(*this);
}

void LPointerMoveEvent::notify()
{
    if (compositor()->state() == LCompositor::Initialized)
        seat()->pointer()->pointerMoveEvent(*this);
}
