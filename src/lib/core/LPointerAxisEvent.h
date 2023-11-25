#ifndef LPOINTERAXISEVENT_H
#define LPOINTERAXISEVENT_H

#include <LInputEvent.h>
#include <LPointer.h>

class Louvre::LPointerAxisEvent : public LInputEvent
{
public:
    /// @cond OMIT
    LCLASS_NO_COPY(LPointerAxisEvent)
    /// @endcond

    Float32 axisX() const;
    Float32 axisY() const;
    Float32 discreteX() const;
    Float32 discreteY() const;
    LPointer::AxisSource source() const;

protected:
    friend class LInputBackend;
    LPointerAxisEvent();
    ~LPointerAxisEvent();
    void notify();
};

#endif // LPOINTERAXISEVENT_H
