#include <LKeyboardLeaveEvent.h>
#include <LCompositor.h>
#include <LTime.h>

using namespace Louvre;

LKeyboardLeaveEvent::LKeyboardLeaveEvent() :
    LKeyboardEvent(Subtype::Leave, LCompositor::nextSerial(), LTime::ms(), nullptr)
{}

LEvent *LKeyboardLeaveEvent::copy() const
{
    return new LKeyboardLeaveEvent(*this);
}
