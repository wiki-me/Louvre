#ifndef LEVENT_H
#define LEVENT_H

#include <LObject.h>

class Louvre::LEvent : public LObject
{
public:
    virtual ~LEvent() {};

    enum class Type : UInt8
    {
        Pointer,
        Keyboard,
        Touch
    };

    enum class Subtype : UInt8
    {
        Enter,
        Leave,
        Up,
        Down,
        Move,
        Button,
        Key,
        Modifiers,
        Scroll,
        Frame,
        Cancel
    };

    inline Type type() const
    {
        return m_type;
    }

    inline Subtype subtype() const
    {
        return m_subtype;
    }

    inline void setSerial(UInt32 serial)
    {
        m_serial = serial;
    }

    inline UInt32 serial() const
    {
        return m_serial;
    }

    inline void setTime(UInt32 time)
    {
        m_time = time;
    }

    inline UInt32 time() const
    {
        return m_time;
    }

    virtual LEvent *copy() const = 0;

protected:
    inline LEvent(Type type, Subtype subtype, UInt32 serial, UInt32 time) :
        m_type(type),
        m_subtype(subtype),
        m_serial(serial),
        m_time(time)
    {}

    Type m_type;
    Subtype m_subtype;
    UInt32 m_serial;
    UInt32 m_time;
};

#endif // LEVENT_H
