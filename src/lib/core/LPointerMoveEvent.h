#ifndef LPOINTERMOVEEVENT_H
#define LPOINTERMOVEEVENT_H

#include <LInputEvent.h>

class Louvre::LPointerMoveEvent : public LInputEvent
{
public:
    LPointerMoveEvent();
    bool isAbsolute() const;
    Float32 x() const;
    Float32 y() const;
};

#endif // LPOINTERMOVEEVENT_H
