#include <LTouchUpEvent.h>
#include <LCompositor.h>
#include <LTime.h>

using namespace Louvre;

LTouchUpEvent::LTouchUpEvent() :
    LTouchEvent(Subtype::Up, LCompositor::nextSerial(), LTime::ms(), nullptr),
    m_id(0) {}

LEvent *LTouchUpEvent::copy() const
{
    return new LTouchUpEvent(*this);
}

void LTouchUpEvent::notify()
{
    if (compositor()->state() == LCompositor::Initialized)
        seat()->touch()->touchUpEvent(*this);
}
