#ifndef LPOINTERENTEREVENT_H
#define LPOINTERENTEREVENT_H

#include <LPointerEvent.h>
#include <LPoint.h>

class Louvre::LPointerEnterEvent : public LPointerEvent
{
public:
    LPointerEnterEvent();
    mutable LPointF localPos;
};

#endif // LPOINTERENTEREVENT_H
