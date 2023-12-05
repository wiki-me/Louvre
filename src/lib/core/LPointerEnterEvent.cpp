#include <LPointerEnterEvent.h>

using namespace Louvre;

LEvent *LPointerEnterEvent::copy() const
{
    return new LPointerEnterEvent(*this);
}
