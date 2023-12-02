#ifndef LKEYBOARDEVENT_H
#define LKEYBOARDEVENT_H

#include <LInputEvent.h>

class Louvre::LKeyboardEvent : public LInputEvent
{
protected:
    inline LKeyboardEvent(Subtype subtype, UInt32 serial, UInt32 time, LInputDevice *device) :
        LInputEvent(Type::Keyboard, subtype, serial, time, device)
    {}
    virtual ~LKeyboardEvent() {}
    virtual LEvent *copy() const override;
};

#endif // LKEYBOARDEVENT_H
