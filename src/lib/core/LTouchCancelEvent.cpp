#include <LTouchCancelEvent.h>
#include <LCompositor.h>

using namespace Louvre;

LTouchCancelEvent::LTouchCancelEvent() {}
LTouchCancelEvent::~LTouchCancelEvent() {}

void LTouchCancelEvent::notify()
{
    if (compositor()->state() == LCompositor::Initialized)
        seat()->touch()->touchCancelEvent(*this);
}
