#include <LKeyboardModifiersEvent.h>
#include <LCompositor.h>
#include <LTime.h>

using namespace Louvre;

LKeyboardModifiersEvent::LKeyboardModifiersEvent() :
    LKeyboardEvent(Subtype::Modifiers, LCompositor::nextSerial(), LTime::ms(), nullptr)
{}
