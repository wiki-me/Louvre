#ifndef LINPUTEVENT_H
#define LINPUTEVENT_H

#include <LObject.h>

class Louvre::LInputEvent : public LObject
{
public:

    enum Type : UInt8
    {
        PointerMove,
        PointerButton,
        PointerScroll,
        KeyboardKey,
        TouchDown,
        TouchUp,
        TouchMove,
        TouchFrame,
        TouchCancel
    };

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

    inline Type type() const
    {
        return m_type;
    }
protected:
    LInputEvent(Type type);
    ~LInputEvent();
    LInputDevice *m_device;
    UInt32 m_time;
    Type m_type;
};

#endif // LINPUTEVENT_H
