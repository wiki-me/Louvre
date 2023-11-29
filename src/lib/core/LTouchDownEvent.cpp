#include <LTouchDownEvent.h>
#include <LCompositor.h>

using namespace Louvre;

LTouchDownEvent::LTouchDownEvent() :
    LInputEvent(LInputEvent::TouchDown),
    m_id(0) {}

LTouchDownEvent::~LTouchDownEvent() {}

void LTouchDownEvent::notify()
{
    if (compositor()->state() == LCompositor::Initialized)
        seat()->touch()->touchDownEvent(*this);
}
