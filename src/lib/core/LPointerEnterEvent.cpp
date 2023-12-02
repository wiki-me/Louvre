#include <LPointerEnterEvent.h>
#include <LCompositor.h>
#include <LTime.h>

using namespace Louvre;

LPointerEnterEvent::LPointerEnterEvent() :
    LPointerEvent(Subtype::Enter, LCompositor::nextSerial(), LTime::ms(), nullptr)
{}
