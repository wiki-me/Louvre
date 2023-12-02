#ifndef LTOUCHFRAMEEVENT_H
#define LTOUCHFRAMEEVENT_H

#include <LTouchEvent.h>

class Louvre::LTouchFrameEvent : public LTouchEvent
{
public:
    LTouchFrameEvent();

private:
    friend class LInputBackend;
    void notify();
};

#endif // LTOUCHFRAMEEVENT_H
