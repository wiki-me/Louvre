#include <LTouchFrameEvent.h>
#include <LCompositor.h>
#include <LTime.h>

using namespace Louvre;

LTouchFrameEvent::LTouchFrameEvent() :
    LTouchEvent(Subtype::Frame, LCompositor::nextSerial(), LTime::ms(), nullptr)
{}

void LTouchFrameEvent::notify()
{
    if (compositor()->state() == LCompositor::Initialized)
        seat()->touch()->touchFrameEvent(*this);
}
