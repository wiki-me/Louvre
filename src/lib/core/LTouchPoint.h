#ifndef LTOUCHPOINT_H
#define LTOUCHPOINT_H

#include <LObject.h>

class Louvre::LTouchPoint : public LObject
{
public:
    LCLASS_NO_COPY(LTouchPoint)

    bool sendTouchMoveEvent(const LTouchMoveEvent &event, const LPointF &localPos);
    bool sendTouchUpEvent(const LTouchUpEvent &event);

    inline LSurface *surface() const
    {
        return m_surface;
    }

    inline Int32 id() const
    {
        return m_id;
    }
private:
    friend class LTouch;
    friend class Louvre::Protocols::Wayland::RSurface;
    LTouchPoint(const LTouchDownEvent &event, LSurface *surface, const LPointF &localPos);
    ~LTouchPoint();

    void sendTouchFrameEvent(const LTouchFrameEvent &event);
    void sendTouchCancelEvent(const LTouchCancelEvent &event);
    const std::list<LTouchPoint*>::iterator destroy();

    LSurface *m_surface = nullptr;
    Int32 m_id = 0;
    bool m_isReleased = false;
    std::list<LTouchPoint*>::iterator m_link;
};

#endif // LTOUCHPOINT_H
