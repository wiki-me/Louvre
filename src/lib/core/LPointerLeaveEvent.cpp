#include <LPointerLeaveEvent.h>

using namespace Louvre;

LEvent *LPointerLeaveEvent::copy() const
{
    return new LPointerLeaveEvent(*this);
}
