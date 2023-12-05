#include <LKeyboardEnterEvent.h>

using namespace Louvre;

LEvent *LKeyboardEnterEvent::copy() const
{
    return new LKeyboardEnterEvent(*this);
}
