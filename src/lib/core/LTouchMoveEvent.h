#ifndef LTOUCHMOVEEVENT_H
#define LTOUCHMOVEEVENT_H

#include <LTouchEvent.h>
#include <LPoint.h>

class Louvre::LTouchMoveEvent : public LTouchEvent
{
public:
    LTouchMoveEvent();

    inline void setPos(const LPointF &pos)
    {
        m_pos = pos;
    }

    inline void setPos(Float32 x, Float32 y)
    {
        m_pos.setX(x);
        m_pos.setY(y);
    }

    inline void setX(Float32 x)
    {
        m_pos.setX(x);
    }

    inline void setY(Float32 y)
    {
        m_pos.setY(y);
    }

    inline const LPointF &pos() const
    {
        return m_pos;
    }

    inline void setId(Int32 id)
    {
        m_id = id;
    }

    inline Int32 id() const
    {
        return m_id;
    }

    mutable LPointF localPos;

protected:
    LPointF m_pos;
    Int32 m_id;
private:
    friend class LInputBackend;
    void notify();
};

#endif // LTOUCHMOVEEVENT_H
