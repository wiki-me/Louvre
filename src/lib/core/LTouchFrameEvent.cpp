#include <LTouchFrameEvent.h>
#include <LCompositor.h>

using namespace Louvre;

LTouchFrameEvent::LTouchFrameEvent() {}
LTouchFrameEvent::~LTouchFrameEvent() {}

void LTouchFrameEvent::notify()
{
    if (compositor()->state() == LCompositor::Initialized)
        seat()->touch()->touchFrameEvent(*this);
}
