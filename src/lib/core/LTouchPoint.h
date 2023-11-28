#ifndef LTOUCHPOINT_H
#define LTOUCHPOINT_H

#include <LObject.h>

class Louvre::LTouchPoint : public LObject
{
public:
    bool sendTouchMoveEvent(const LTouchMoveEvent &event, const LPointF &localPos);
    bool sendTouchUpEvent(const LTouchUpEvent &event);
    LSurface *surface() const;
private:
    friend class LTouch;
    LTouchPoint(const LTouchDownEvent &event, LSurface *surface);
    ~LTouchPoint();

    LSurface *m_surface = nullptr;
    Int32 m_id = 0;
    bool m_isReleased = false;
    std::list<LTouchPoint*>::iterator m_link;
};

#endif // LTOUCHPOINT_H
