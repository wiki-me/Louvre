#ifndef LTOUCH_H
#define LTOUCH_H

#include <LInputEvent.h>

class Louvre::LTouch : public LInputEvent
{
public:
    struct Params;

    LTouch(Params *params);

    /// @cond OMIT
    LCLASS_NO_COPY(LTouch)
    /// @endcond

    virtual ~LTouch();

    const std::list<LTouchPoint*> &touchPoints() const;

    const LTouchPoint *sendTouchDownEvent(const LTouchDownEvent &event, LSurface *surface, const LPointF &localPos);

    void sendTouchFrameEvent(const LTouchFrameEvent &event);
    void sendTouchCancelEvent(const LTouchCancelEvent &event);

    virtual void touchDownEvent(const LTouchDownEvent &event);
    virtual void touchMoveEvent(const LTouchMoveEvent &event);
    virtual void touchUpEvent(const LTouchUpEvent &event);
    virtual void touchFrameEvent(const LTouchFrameEvent &event);
    virtual void touchCancelEvent(const LTouchCancelEvent &event);

    LPRIVATE_IMP_UNIQUE(LTouch)
};

#endif // LTOUCH_H
