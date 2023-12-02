#ifndef LPOINTERLEAVEEVENT_H
#define LPOINTERLEAVEEVENT_H

#include <LPointerEvent.h>
#include <LPoint.h>

class Louvre::LPointerLeaveEvent : public LPointerEvent
{
public:
    LPointerLeaveEvent();
    virtual ~LPointerLeaveEvent() {}
    virtual LEvent *copy() const override;
};
#endif // LPOINTERLEAVEEVENT_H
