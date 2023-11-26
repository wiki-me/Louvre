#include <LPointerAxisEvent.h>
#include <LCompositor.h>

using namespace Louvre;

LPointerAxisEvent::LPointerAxisEvent() : m_source(LPointer::AxisSource::Continuous) {}
LPointerAxisEvent::~LPointerAxisEvent() {}

void LPointerAxisEvent::notify()
{
    if (compositor()->state() == LCompositor::Initialized)
        seat()->pointer()->pointerAxisEvent(*this);
}
