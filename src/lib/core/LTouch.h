#ifndef LTOUCH_H
#define LTOUCH_H

#include <LInputEvent.h>

class Louvre::LTouch : public LObject
{
public:
    struct Params;

    LTouch(Params *params);

    /// @cond OMIT
    LCLASS_NO_COPY(LTouch)
    /// @endcond

    virtual ~LTouch();

    // Returns the first surface which contains the given point, considering they role position and input region.
    // If no surface is found this returns nullptr.
    LSurface *surfaceAt(const LPoint &point);

    // List of all currently active touchpoints.
    // Each touchpoint has a unique id.
    const std::list<LTouchPoint*> &touchPoints() const;

    // Creates a new touch point. If there is already a touch point with the same id, that touchpoint is returned.
    LTouchPoint *createTouchPoint(const LTouchDownEvent &event);

    // Finds and returns the touch point that matches the id. Nullptr is returns if no touch point is found.
    LTouchPoint *findTouchPoint(Int32 id) const;

    // Send a frame events to all clients whith surfaces assigned a touch point.
    // Clients wait for this event before processing the previous sent touch events.
    // Touch points that are no longer pressed are destroyed after this.
    void sendFrameEvent(const LTouchFrameEvent &event);

    // Sends a cancel event to clients with surfaces assigned a touchpoint.
    // All current touchpoints are destroyed after this.
    void sendCancelEvent(const LTouchCancelEvent &event);

    virtual void touchDownEvent(const LTouchDownEvent &event);
    virtual void touchMoveEvent(const LTouchMoveEvent &event);
    virtual void touchUpEvent(const LTouchUpEvent &event);
    virtual void touchFrameEvent(const LTouchFrameEvent &event);
    virtual void touchCancelEvent(const LTouchCancelEvent &event);

    LPRIVATE_IMP_UNIQUE(LTouch)
};

#endif // LTOUCH_H
