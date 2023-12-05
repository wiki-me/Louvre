#include <LKeyboardLeaveEvent.h>

using namespace Louvre;

LEvent *LKeyboardLeaveEvent::copy() const
{
    return new LKeyboardLeaveEvent(*this);
}
