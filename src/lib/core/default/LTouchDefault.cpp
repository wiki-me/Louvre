#include <private/LTouchPrivate.h>
#include <LKeyboard.h>
#include <LDNDManager.h>
#include <LDNDIconRole.h>
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
    // Creates or returns an existing touch point with the event id
    LTouchPoint *tp = createTouchPoint(event);

    // Transform touch position to global position
    LPointF globalPos = toGlobal(cursor()->output(), event.pos());

    cursor()->setPos(globalPos);

    // Check if a surface was touched
    LSurface *surface = surfaceAt(globalPos);

    if (surface)
    {
        event.localPos = globalPos - surface->rolePos();
        tp->sendDownEvent(event, surface);
        seat()->keyboard()->setFocus(surface);
        surface->raise();
    }
    else
        tp->sendDownEvent(event);
}

void LTouch::touchMoveEvent(const LTouchMoveEvent &event)
{
    LTouchPoint *tp = findTouchPoint(event.id());

    if (!tp)
        return;

    // For simplicity we use the output where the cursor is positioned
    LOutput *output = cursor()->output();

    // Transform touch position to global position
    LPointF globalPos = output->pos() + (output->size() * event.pos());

    // Handle DND session
    LDNDManager *dnd = seat()->dndManager();

    if (dnd->dragging() && dnd->startDragEvent()->type() == LEvent::Type::Touch && dnd->startDragEvent()->subtype() == LEvent::Subtype::Down)
    {
        LTouchDownEvent *touchDownEvent = (LTouchDownEvent*)dnd->startDragEvent();

        if (touchDownEvent->id() == tp->id())
        {
            if (dnd->icon())
            {
                dnd->icon()->surface()->setPos(globalPos);
                dnd->icon()->surface()->repaintOutputs();
            }

            LSurface *surface = surfaceAt(globalPos);

            if (surface)
            {
                if (dnd->focus() == surface)
                    dnd->sendMoveEvent(globalPos - surface->rolePos(), event.time());
                else
                    dnd->setFocus(surface, globalPos - surface->rolePos());
            }
            else
                dnd->setFocus(nullptr, LPoint());
        }
    }

    // Send the event
    if (tp->surface())
    {
        event.localPos = globalPos -tp->surface()->rolePos();
        tp->sendMoveEvent(event);
    }
    else
        tp->sendMoveEvent(event);
}

void LTouch::touchUpEvent(const LTouchUpEvent &event)
{
    LTouchPoint *tp = findTouchPoint(event.id());

    if (!tp)
        return;

    LDNDManager *dnd = seat()->dndManager();

    if (dnd->dragging() && dnd->startDragEvent()->type() == LEvent::Type::Touch && dnd->startDragEvent()->subtype() == LEvent::Subtype::Down)
    {
        LTouchDownEvent *touchDownEvent = (LTouchDownEvent*)dnd->startDragEvent();

        if (touchDownEvent->id() == tp->id())
            dnd->drop();
    }

    // Send the event
    tp->sendUpEvent(event);
}

void LTouch::touchFrameEvent(const LTouchFrameEvent &event)
{
    // Released touch points are destroyed after this event
    sendFrameEvent(event);
}

void LTouch::touchCancelEvent(const LTouchCancelEvent &event)
{
    // All touch points are destroyed
    sendCancelEvent(event);
}
