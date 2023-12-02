#include <LTouchCancelEvent.h>
#include <LCompositor.h>
#include <LTime.h>

using namespace Louvre;

LTouchCancelEvent::LTouchCancelEvent() :
    LTouchEvent(Subtype::Cancel, LCompositor::nextSerial(), LTime::ms(), nullptr)
{}

void LTouchCancelEvent::notify()
{
    if (compositor()->state() == LCompositor::Initialized)
        seat()->touch()->touchCancelEvent(*this);
}
