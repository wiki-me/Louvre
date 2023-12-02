#include <LKeyboardEnterEvent.h>
#include <LCompositor.h>
#include <LTime.h>

using namespace Louvre;

LKeyboardEnterEvent::LKeyboardEnterEvent() :
    LKeyboardEvent(Subtype::Enter, LCompositor::nextSerial(), LTime::ms(), nullptr)
{}

LEvent *LKeyboardEnterEvent::copy() const
{
    return new LKeyboardEnterEvent(*this);
}
