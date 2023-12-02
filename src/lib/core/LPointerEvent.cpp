#include <LPointerEvent.h>

using namespace Louvre;

LEvent *LPointerEvent::copy() const
{
    return new LPointerEvent(*this);
}
