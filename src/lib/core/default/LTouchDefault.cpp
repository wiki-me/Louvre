#include <private/LTouchPrivate.h>
#include <LInputDevice.h>
#include <LTouchDownEvent.h>
#include <LTouchMoveEvent.h>
#include <LTouchUpEvent.h>
#include <LTouchFrameEvent.h>
#include <LTouchCancelEvent.h>
#include <LTouchPoint.h>
#include <LCursor.h>
#include <LOutput.h>
#include <LLog.h>

void LTouch::touchDownEvent(const LTouchDownEvent &event)
{
    // For simplicity we use the output where the cursor is positioned
    LOutput *output = cursor()->output();

    // Transform touch position to global position
    LPointF pos = output->pos() + (output->size() * event.pos());

    // Check if a surface was touched
    LSurface *surface = surfaceAt(pos);

    if (!surface)
        return;

    // Create a touch point
    sendTouchDownEvent(event, // For the touch id and event time
                       surface,
                       pos - surface->rolePos()); // Transform pos to local-surface coordinates
}

void LTouch::touchMoveEvent(const LTouchMoveEvent &event)
{
    // For simplicity we use the output where the cursor is positioned
    LOutput *output = cursor()->output();

    // Transform touch position to global position
    LPointF pos = output->pos() + (output->size() * event.pos());

    // Send the event
    for (LTouchPoint *tp : touchPoints())
        if (tp->id() == event.id())
            tp->sendTouchMoveEvent(event, pos - tp->surface()->rolePos());
}

void LTouch::touchUpEvent(const LTouchUpEvent &event)
{
    // Send the event
    for (LTouchPoint *tp : touchPoints())
        if (tp->id() == event.id())
            tp->sendTouchUpEvent(event);
}

void LTouch::touchFrameEvent(const LTouchFrameEvent &event)
{
    // Released touch points are destroyed after this event
    sendTouchFrameEvent(event);
}

void LTouch::touchCancelEvent(const LTouchCancelEvent &event)
{
    // All touch points are destroyed
    sendTouchCancelEvent(event);
}
