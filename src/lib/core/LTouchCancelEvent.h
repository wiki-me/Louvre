#ifndef LTOUCHCANCELEVENT_H
#define LTOUCHCANCELEVENT_H

#include <LTouchEvent.h>

class Louvre::LTouchCancelEvent : public LTouchEvent
{
public:
    LTouchCancelEvent();
    virtual ~LTouchCancelEvent() {}
    virtual LEvent *copy() const override;

private:
    friend class LInputBackend;
    void notify();
};

#endif // LTOUCHCANCELEVENT_H
