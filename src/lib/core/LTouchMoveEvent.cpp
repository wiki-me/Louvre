#include <LTouchMoveEvent.h>
#include <LCompositor.h>
#include <LTime.h>

using namespace Louvre;

LTouchMoveEvent::LTouchMoveEvent() :
    LTouchEvent(Subtype::Move, LCompositor::nextSerial(), LTime::ms(), nullptr),
    m_id(0) {}

void LTouchMoveEvent::notify()
{
    if (compositor()->state() == LCompositor::Initialized)
        seat()->touch()->touchMoveEvent(*this);
}
