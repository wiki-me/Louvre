#include <LKeyboardModifiersEvent.h>

using namespace Louvre;

LEvent *LKeyboardModifiersEvent::copy() const
{
    return new LKeyboardModifiersEvent(*this);
}
