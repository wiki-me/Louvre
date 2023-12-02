#ifndef LTOUCHEVENT_H
#define LTOUCHEVENT_H

#include <LInputEvent.h>

class Louvre::LTouchEvent : public LInputEvent
{
protected:
    inline LTouchEvent(Subtype subtype, UInt32 serial, UInt32 time, LInputDevice *device) :
        LInputEvent(Type::Touch, subtype, serial, time, device)
    {}
};

#endif // LTOUCHEVENT_H
