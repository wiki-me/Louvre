#include <LScene.h>
#include <LCursor.h>
#include <LXCursor.h>
#include <LView.h>
#include <LCursorRole.h>
#include <LLog.h>
#include <LTime.h>
#include <LSurfaceView.h>
#include <LSurface.h>
#include <LOutput.h>
#include <LPointerMoveEvent.h>
#include <LPointerButtonEvent.h>
#include <LPointerScrollEvent.h>
#include <LInputDevice.h>

#include "Global.h"
#include "Pointer.h"
#include "Compositor.h"

Pointer::Pointer(Params *params) : LPointer(params) {}

void Pointer::pointerMoveEvent(const Louvre::LPointerMoveEvent &event)
{
    LView *view = G::scene()->handlePointerMoveEvent(event);

    if (seat()->movingToplevel() || seat()->resizingToplevel())
        cursor()->output()->repaint();

    if (seat()->resizingToplevel() || cursorOwner)
        return;

    // Let the client set the cursor during DND
    if (seat()->dndManager()->dragging())
        return;

    if (view)
    {
        if (view->type() == LView::Surface)
        {
            LSurfaceView *surfView = (LSurfaceView*)view;

            if (lastCursorRequest() && lastCursorRequest()->surface()->client() == surfView->surface()->client())
            {
                cursor()->setTextureB(lastCursorRequest()->surface()->texture(), lastCursorRequest()->hotspotB());
                cursor()->setVisible(true);
            }
            else
            {
                if (lastCursorRequestWasHide())
                    cursor()->setVisible(false);
                else
                {
                    cursor()->setVisible(true);
                    cursor()->useDefault();
                }
            }
        }

        return;
    }

    cursor()->useDefault();
    cursor()->setVisible(true);
}

void Pointer::pointerButtonEvent(const LPointerButtonEvent &event)
{
    if (event.button() == LPointerButtonEvent::Left && event.state() == LPointerButtonEvent::Released)
    {
        G::enableDocks(true);
        G::compositor()->updatePointerBeforePaint = true;
    }

    G::scene()->handlePointerButtonEvent(event);
}

void Pointer::pointerScrollEvent(const LPointerScrollEvent &event)
{
    LPointerScrollEvent ev = event;
    ev.invertX();
    ev.invertY();
    G::scene()->handlePointerScrollEvent(ev);
}

void Pointer::setCursorRequest(LCursorRole *cursorRole)
{
    if (seat()->resizingToplevel() || cursorOwner)
        return;

    if (cursorRole)
    {
        cursor()->setTextureB(
                    cursorRole->surface()->texture(),
                    cursorRole->hotspotB());

        cursor()->setVisible(true);
    }
    // If nullptr means the client wants to hide the cursor
    else
        cursor()->setVisible(false);
}
