#ifndef LTOUCHPOINT_H
#define LTOUCHPOINT_H

#include <LObject.h>
#include <LPoint.h>

class Louvre::LTouchPoint : public LObject
{
public:
    LCLASS_NO_COPY(LTouchPoint)

    struct Serials
    {
        UInt32 down = 0;
        UInt32 up = 0;
    };

    // Id of the touch point. Each touch point has a unique id, ther cant be two touchpoints with the same id.
    Int32 id() const;

    // Indicates if the touch point is being pressed. Initially all touch points are pressed when created.
    // This can later change if the down() or up() methods are called.
    // If LTouch::frame() is called and the touchpoint is not pressed, it is destroyed.
    bool isPressed() const;

    // Surface being touched by this touch point. Set with the down() method. If no surface is assigned to this touchpoint, it returns nullptr.
    LSurface *surface() const;

    // Last down event
    const LTouchDownEvent &lastDownEvent() const;

    // Last move event
    const LTouchMoveEvent &lastMoveEvent() const;

    // Last up event
    const LTouchUpEvent &lastUpEvent() const;

    // Marks the touch points as being pressed.
    // If the surface parameter is used, the event is sent to that surface, with the position given by localSurfacePos.
    // If the surface is differnet to the current surface, the previous surfaces is sent an up and frame event.
    // If the surface param is nullptr unsets surface.
    // If the current assigned surface is destroyed, it is automatically unset.
    // Returns true on surccees and false if the ids dont match.
    bool sendDownEvent(const LTouchDownEvent &event, LSurface *surface = nullptr);

    // If there is an assigned surface, this event notifies the client about the touchpoint moviement.
    // Returns true on surccees and false if the ids dont match.
    bool sendMoveEvent(const LTouchMoveEvent &event);

    // If there is an assigned surface, this event notifies the client about the touchpoint being released.
    // If LTouch::frame() is called and the touchpoint is not pressed, it is destroyed.
    // Returns true on surccees and false if the ids dont match.
    bool sendUpEvent(const LTouchUpEvent &event);

LPRIVATE_IMP_UNIQUE(LTouchPoint)
    friend class LTouch;
    LTouchPoint(const LTouchDownEvent &event);
    ~LTouchPoint();
};

#endif // LTOUCHPOINT_H
