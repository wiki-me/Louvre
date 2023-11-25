#include <private/LCompositorPrivate.h>
#include <LPointerMoveEvent.h>

LPointerMoveEvent::LPointerMoveEvent(const LPointF &pos, bool absolute, UInt32 time) :
    m_isAbsolute(absolute),
    m_pos(pos)
{
    setTime(time);
}

Louvre::LPointerMoveEvent::LPointerMoveEvent() {}
LPointerMoveEvent::~LPointerMoveEvent() {}

void LPointerMoveEvent::notify()
{
    if (compositor()->state() == LCompositor::Initialized)
        seat()->pointer()->pointerMoveEvent(*this);
}
