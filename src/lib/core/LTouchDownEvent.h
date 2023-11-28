#ifndef LTOUCHDOWNEVENT_H
#define LTOUCHDOWNEVENT_H

#include <LInputEvent.h>
#include <LPoint.h>

class Louvre::LTouchDownEvent : public LInputEvent
{
public:
    LTouchDownEvent();
    ~LTouchDownEvent();

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

    inline void setId(UInt32 id)
    {
        m_id = id;
    }

    inline UInt32 id() const
    {
        return m_id;
    }

protected:
    LPointF m_pos;
    UInt32 m_id;
private:
    friend class LInputBackend;
    void notify();
};

#endif // LTOUCHDOWNEVENT_H
