#ifndef LPOINTERMOVEEVENT_H
#define LPOINTERMOVEEVENT_H

#include <LInputEvent.h>
#include <LPoint.h>
#include <LTime.h>

class Louvre::LPointerMoveEvent : public LInputEvent
{
public:
    LPointerMoveEvent(const LPointF &pos, bool absolute, UInt32 time = LTime::ms());
    LPointerMoveEvent();
    ~LPointerMoveEvent();

    inline void setIsAbsolute(bool absolute)
    {
        m_isAbsolute = absolute;
    }

    inline bool isAbsolute() const
    {
        return m_isAbsolute;
    }

    inline void setX(Float32 x)
    {
        m_pos.setX(x);
    }

    inline Float32 x() const
    {
        return m_pos.x();
    }

    inline void setY(Float32 y)
    {
        m_pos.setY(y);
    }

    inline Float32 y() const
    {
        return m_pos.y();
    }

    inline void setPos(Float32 x, Float32 y)
    {
        m_pos.setX(x);
        m_pos.setY(y);
    }

    inline void setPos(const LPointF &pos)
    {
        m_pos = pos;
    }

    const LPointF &pos() const
    {
        return m_pos;
    }

protected:
    bool m_isAbsolute = false;
    LPointF m_pos;
private:
    friend class LInputBackend;
    void notify();
};

#endif // LPOINTERMOVEEVENT_H
