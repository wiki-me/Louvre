#ifndef LPOINTERAXISEVENT_H
#define LPOINTERAXISEVENT_H

#include <LInputEvent.h>
#include <LPointer.h>

class Louvre::LPointerScrollEvent : public LInputEvent
{
public:
    LPointerScrollEvent();
    ~LPointerScrollEvent();

    inline void setAxes(const LPointF &axes)
    {
        m_axes = axes;
    }

    inline void setAxes(Float32 x, Float32 y)
    {
        m_axes.setX(x);
        m_axes.setY(y);
    }

    inline void setX(Float32 x)
    {
        m_axes.setX(x);
    }

    inline void setY(Float32 y)
    {
        m_axes.setY(y);
    }

    inline const LPointF &axes() const
    {
        return m_axes;
    }

    //

    inline void setAxes120(const LPointF &axes)
    {
        m_axes120 = axes;
    }

    inline void setAxes120(Float32 x, Float32 y)
    {
        m_axes120.setX(x);
        m_axes120.setY(y);
    }

    inline void set120X(Float32 x)
    {
        m_axes120.setX(x);
    }

    inline void set120Y(Float32 y)
    {
        m_axes120.setY(y);
    }

    inline const LPointF &axes120() const
    {
        return m_axes120;
    }

    inline void invertX()
    {
        m_axes.setX(-m_axes.x());
        m_axes120.setX(-m_axes120.x());
    }

    inline void invertY()
    {
        m_axes.setY(-m_axes.y());
        m_axes120.setY(-m_axes120.y());
    }

    //

    inline void setSource(LPointer::ScrollEventSource source)
    {
        m_source = source;
    }

    inline LPointer::ScrollEventSource source() const
    {
        return m_source;
    }

protected:
    LPointF m_axes;
    LPointF m_axes120;
    LPointer::ScrollEventSource m_source;
private:
    friend class LInputBackend;
    void notify();
};

#endif // LPOINTERAXISEVENT_H
