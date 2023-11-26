#ifndef LPOINTERAXISEVENT_H
#define LPOINTERAXISEVENT_H

#include <LInputEvent.h>
#include <LPointer.h>

class Louvre::LPointerAxisEvent : public LInputEvent
{
public:
    LPointerAxisEvent();
    ~LPointerAxisEvent();

    inline void setAxis(const LPointF &axis)
    {
        m_axis = axis;
    }

    inline void setAxis(Float32 axisX, Float32 axisY)
    {
        m_axis.setX(axisX);
        m_axis.setY(axisY);
    }

    inline void setAxisX(Float32 axisX)
    {
        m_axis.setX(axisX);
    }

    inline void setAxisY(Float32 axisY)
    {
        m_axis.setY(axisY);
    }

    inline const LPointF &axis() const
    {
        return m_axis;
    }

    //

    inline void setAxis120(const LPointF &axis120)
    {
        m_axis120 = axis120;
    }

    inline void setAxis120(Float32 axis120X, Float32 axis120Y)
    {
        m_axis120.setX(axis120X);
        m_axis120.setY(axis120Y);
    }

    inline void setAxis120X(Float32 axis120X)
    {
        m_axis120.setX(axis120X);
    }

    inline void setAxis120Y(Float32 axis120Y)
    {
        m_axis120.setY(axis120Y);
    }

    inline const LPointF &axis120() const
    {
        return m_axis120;
    }

    //

    inline void setSource(LPointer::AxisSource source)
    {
        m_source = source;
    }

    inline LPointer::AxisSource source() const
    {
        return m_source;
    }

protected:
    LPointF m_axis;
    LPointF m_axis120;
    LPointer::AxisSource m_source;
private:
    friend class LInputBackend;
    void notify();
};

#endif // LPOINTERAXISEVENT_H
