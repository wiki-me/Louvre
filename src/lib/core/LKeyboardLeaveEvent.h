#ifndef LKEYBOARDLEAVEEVENT_H
#define LKEYBOARDLEAVEEVENT_H

#include <LKeyboardEvent.h>

class Louvre::LKeyboardLeaveEvent : public LKeyboardEvent
{
public:
    LKeyboardLeaveEvent();
    virtual ~LKeyboardLeaveEvent() {}
    virtual LEvent *copy() const override;
};

#endif // LKEYBOARDLEAVEEVENT_H
