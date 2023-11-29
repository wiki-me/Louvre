#include <LTouchUpEvent.h>
#include <LCompositor.h>

using namespace Louvre;

LTouchUpEvent::LTouchUpEvent() :
    LInputEvent(LInputEvent::TouchUp),
    m_id(0) {}

LTouchUpEvent::~LTouchUpEvent() {}

void LTouchUpEvent::notify()
{
    if (compositor()->state() == LCompositor::Initialized)
        seat()->touch()->touchUpEvent(*this);
}
