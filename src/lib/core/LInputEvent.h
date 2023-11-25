#ifndef LINPUTEVENT_H
#define LINPUTEVENT_H

#include <LObject.h>

class Louvre::LInputEvent : public LObject
{
public:
    inline void setDevice(LInputDevice *device)
    {
        m_device = device;
    }

    inline LInputDevice *device() const
    {
        return m_device;
    }

    inline void setTime(UInt32 ms)
    {
        m_time = ms;
    }

    inline UInt32 time() const
    {
        return m_time;
    }

protected:
    LInputEvent();
    ~LInputEvent();
    LInputDevice *m_device;
    UInt32 m_time;
};

#endif // LINPUTEVENT_H
