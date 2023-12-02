#ifndef LPOINTERENTEREVENT_H
#define LPOINTERENTEREVENT_H

#include <LPointerEvent.h>
#include <LPoint.h>

class Louvre::LPointerEnterEvent : public LPointerEvent
{
public:
    LPointerEnterEvent();
    virtual ~LPointerEnterEvent() {}
    virtual LEvent *copy() const override;
    mutable LPointF localPos;
};

#endif // LPOINTERENTEREVENT_H
