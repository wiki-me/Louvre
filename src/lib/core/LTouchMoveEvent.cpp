#include <LTouchMoveEvent.h>
#include <LCompositor.h>

using namespace Louvre;

LTouchMoveEvent::LTouchMoveEvent() :
    LInputEvent(LInputEvent::TouchMove),
    m_id(0) {}

LTouchMoveEvent::~LTouchMoveEvent() {}

void LTouchMoveEvent::notify()
{
    if (compositor()->state() == LCompositor::Initialized)
        seat()->touch()->touchMoveEvent(*this);
}
