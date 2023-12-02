#ifndef LKEYBOARDENTEREVENT_H
#define LKEYBOARDENTEREVENT_H

#include <LKeyboardEvent.h>

class Louvre::LKeyboardEnterEvent : public LKeyboardEvent
{
public:
    LKeyboardEnterEvent();
    virtual ~LKeyboardEnterEvent() {};
    virtual LEvent *copy() const override;
};

#endif // LKEYBOARDENTEREVENT_H
