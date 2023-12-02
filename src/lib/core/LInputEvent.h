#ifndef LINPUTEVENT_H
#define LINPUTEVENT_H

#include <LEvent.h>

class Louvre::LInputEvent : public LEvent
{
public:

    void setDevice(LInputDevice *device);

    inline LInputDevice *device() const
    {
        return m_device;
    }

protected:
    LInputEvent(Type type, Subtype subtype, UInt32 serial, UInt32 time, LInputDevice *device);
    LInputDevice *m_device;
};

#endif // LINPUTEVENT_H
