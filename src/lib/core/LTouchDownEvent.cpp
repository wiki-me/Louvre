#include <LTouchDownEvent.h>
#include <LCompositor.h>
#include <LTime.h>

using namespace Louvre;

LTouchDownEvent::LTouchDownEvent() :
    LTouchEvent(Subtype::Down, LCompositor::nextSerial(), LTime::ms(), nullptr),
    m_id(0) {}

LEvent *LTouchDownEvent::copy() const
{
    return new LTouchDownEvent(*this);
}

void LTouchDownEvent::notify()
{
    if (compositor()->state() == LCompositor::Initialized)
        seat()->touch()->touchDownEvent(*this);
}
