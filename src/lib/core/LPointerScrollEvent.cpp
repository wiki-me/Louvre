#include <LPointerScrollEvent.h>
#include <LCompositor.h>
#include <LTime.h>

using namespace Louvre;

LPointerScrollEvent::LPointerScrollEvent() :
    LPointerEvent(Subtype::Scroll, LCompositor::nextSerial(), LTime::ms(), nullptr),
    m_source(Source::Continuous) {}

LEvent *LPointerScrollEvent::copy() const
{
    return new LPointerScrollEvent(*this);
}

void LPointerScrollEvent::notify()
{
    if (compositor()->state() == LCompositor::Initialized)
        seat()->pointer()->pointerScrollEvent(*this);
}
