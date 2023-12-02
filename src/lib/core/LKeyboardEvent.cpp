#include <LKeyboardEvent.h>

using namespace Louvre;

LEvent *LKeyboardEvent::copy() const
{
    return new LKeyboardEvent(*this);
}
