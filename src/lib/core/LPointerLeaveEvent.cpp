#include <LPointerLeaveEvent.h>
#include <LCompositor.h>
#include <LTime.h>

using namespace Louvre;

LPointerLeaveEvent::LPointerLeaveEvent() :
    LPointerEvent(Subtype::Leave, LCompositor::nextSerial(), LTime::ms(), nullptr)
{}
