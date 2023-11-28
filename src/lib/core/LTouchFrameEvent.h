#ifndef LTOUCHFRAMEEVENT_H
#define LTOUCHFRAMEEVENT_H

#include <LInputEvent.h>

class Louvre::LTouchFrameEvent : public LInputEvent
{
public:
    LTouchFrameEvent();
    ~LTouchFrameEvent();

private:
    friend class LInputBackend;
    void notify();
};

#endif // LTOUCHFRAMEEVENT_H
