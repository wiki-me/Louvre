#include <LPointerScrollEvent.h>
#include <LCompositor.h>

using namespace Louvre;

LPointerScrollEvent::LPointerScrollEvent() : m_source(LPointer::ScrollEventSource::Continuous) {}
LPointerScrollEvent::~LPointerScrollEvent() {}

void LPointerScrollEvent::notify()
{
    if (compositor()->state() == LCompositor::Initialized)
        seat()->pointer()->pointerScrollEvent(*this);
}
