#include <LTouchEvent.h>

using namespace Louvre;

LEvent *LTouchEvent::copy() const
{
    return new LTouchEvent(*this);
}
