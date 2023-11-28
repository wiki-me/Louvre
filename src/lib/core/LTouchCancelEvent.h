#ifndef LTOUCHCANCELEVENT_H
#define LTOUCHCANCELEVENT_H

#include <LInputEvent.h>

class Louvre::LTouchCancelEvent : public LInputEvent
{
public:
    LTouchCancelEvent();
    ~LTouchCancelEvent();

private:
    friend class LInputBackend;
    void notify();
};

#endif // LTOUCHCANCELEVENT_H
