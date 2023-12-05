#include <LLog.h>
#include <LPointer.h>
#include <LSeat.h>
#include <LCompositor.h>
#include <LCursor.h>
#include <LOutput.h>
#include <LPopupRole.h>
#include <LTime.h>
#include <LKeyboard.h>
#include <LDNDManager.h>
#include <LDNDIconRole.h>
#include <LCursorRole.h>
#include <LPointerMoveEvent.h>
#include <LPointerButtonEvent.h>
#include <LPointerScrollEvent.h>
#include <LToplevelResizeSession.h>

using namespace Louvre;

//! [pointerMoveEvent]
void LPointer::pointerMoveEvent(const LPointerMoveEvent &event)
{
    // Update the cursor position
    cursor()->move(event.delta());

    // Schedule repaint on outputs that intersect with the cursor if hardware composition is not supported.
    cursor()->repaintOutputs(true);

    bool activeDND = seat()->dndManager()->dragging() && seat()->dndManager()->startDragEvent()->type() != LEvent::Type::Touch;

    if (seat()->dndManager()->icon() && activeDND)
    {
        seat()->dndManager()->icon()->surface()->setPos(cursor()->pos());
        seat()->dndManager()->icon()->surface()->repaintOutputs();
    }

    bool activeResizing = false;

    for (LToplevelResizeSession *session : seat()->resizeSessions())
    {
        if (session->triggeringEvent()->type() != LEvent::Type::Touch)
        {
            activeResizing = true;
            session->setResizePointPos(cursor()->pos());
        }
    }

    if (activeResizing)
        return;

    if (seat()->movingToplevel())
    {
        seat()->updateMovingToplevelPos(cursor()->pos());

        seat()->movingToplevel()->surface()->repaintOutputs();

        if (seat()->movingToplevel()->maximized())
            seat()->movingToplevel()->configure(seat()->movingToplevel()->pendingState() &~ LToplevelRole::Maximized);

        return;
    }

    // If a surface had the left pointer button held down
    if (draggingSurface())
    {
        event.localPos = cursor()->pos() - draggingSurface()->rolePos();
        sendMoveEvent(event);
        return;
    }

    // Find the first surface under the cursor
    LSurface *surface = surfaceAt(cursor()->pos());

    if (surface)
    {
        event.localPos = cursor()->pos() - surface->rolePos();

        if (focus() == surface)
            sendMoveEvent(event);
        else
            setFocus(surface, event.localPos);

        if (activeDND)
        {
            if (seat()->dndManager()->focus() == surface)
                seat()->dndManager()->sendMoveEvent(event.localPos, event.ms());
            else
                seat()->dndManager()->setFocus(surface, event.localPos);
        }
    }
    else
    {
        setFocus(nullptr);

        if (activeDND)
            seat()->dndManager()->setFocus(nullptr, LPointF());

        cursor()->useDefault();
        cursor()->setVisible(true);
    }
}
//! [pointerMoveEvent]

//! [pointerButtonEvent]
void LPointer::pointerButtonEvent(const LPointerButtonEvent &event)
{
    bool activeDND = seat()->dndManager()->dragging() && seat()->dndManager()->startDragEvent()->type() != LEvent::Type::Touch;

    if (activeDND && event.state() == LPointerButtonEvent::Released && event.button() == LPointerButtonEvent::Left)
        seat()->dndManager()->drop();

    if (!focus())
    {
        LSurface *surface = surfaceAt(cursor()->pos());

        if (surface)
        {
            seat()->keyboard()->setFocus(surface);
            setFocus(surface);
            sendButtonEvent(event);

            if (!surface->popup())
                dismissPopups();
        }
        else
        {
            seat()->keyboard()->setFocus(nullptr);
            dismissPopups();
        }

        return;
    }

    sendButtonEvent(event);

    if (event.button() != LPointerButtonEvent::Left)
        return;

    // Left button pressed
    if (event.state() == LPointerButtonEvent::Pressed)
    {
        // We save the pointer focus surface to continue sending events to it even when the cursor
        // is outside of it (while the left button is being held down)
        setDraggingSurface(focus());

        if (!seat()->keyboard()->focus() || !focus()->isSubchildOf(seat()->keyboard()->focus()))
            seat()->keyboard()->setFocus(focus());

        if (focus()->toplevel() && !focus()->toplevel()->activated())
            focus()->toplevel()->configure(focus()->toplevel()->pendingState() | LToplevelRole::Activated);

        if (!focus()->popup())
            dismissPopups();

        if (focus() == compositor()->surfaces().back())
            return;

        if (focus()->parent())
            focus()->topmostParent()->raise();
        else
            focus()->raise();
    }
    // Left button released
    else
    {
        // Stop pointer toplevel resizing sessions
        for (std::list<LToplevelResizeSession*>::const_iterator it = seat()->resizeSessions().begin(); it != seat()->resizeSessions().end(); it++)
            if ((*it)->triggeringEvent()->type() != LEvent::Type::Touch)
                it = (*it)->stop();

        seat()->stopMovingToplevel();

        // We stop sending events to the surface on which the left button was being held down
        setDraggingSurface(nullptr);

        if (!focus()->inputRegion().containsPoint(cursor()->pos() - focus()->rolePos()))
        {
            setFocus(nullptr);
            cursor()->useDefault();
            cursor()->setVisible(true);
        }
    }
}
//! [pointerButtonEvent]

//! [pointerAxisEvent]
void LPointer::pointerScrollEvent(const LPointerScrollEvent &event)
{
    // Invert the scroll axis for natural scrolling
    LPointerScrollEvent ev = event;
    ev.invertX();
    ev.invertY();
    sendScrollEvent(ev);
}
//! [pointerAxisEvent]

//! [setCursorRequest]
void LPointer::setCursorRequest(LCursorRole *cursorRole)
{
    if (cursorRole)
    {
        cursor()->setTextureB(
                    cursorRole->surface()->texture(),
                    cursorRole->hotspotB());

        cursor()->setVisible(true);
    }
    // If `nullptr` is provided, it indicates that the client intends to hide the cursor.
    else
        cursor()->setVisible(false);
}
//! [setCursorRequest]
