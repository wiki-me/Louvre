#include "LLog.h"
#include <LDNDManager.h>
#include <LSeat.h>
#include <LPointer.h>
#include <LCompositor.h>
#include <LDataSource.h>
#include <LTouchPoint.h>
#include <LTouchDownEvent.h>
#include <LCursor.h>
#include <LOutput.h>

using namespace Louvre;

//! [startDragRequest]
void LDNDManager::startDragRequest()
{
    // Left pointer button click
    if (startDragEvent()->type() == LEvent::Type::Pointer && seat()->pointer()->focus() && seat()->pointer()->focus()->client() == origin()->client())
    {
        if (startDragEvent()->subtype() == LEvent::Subtype::Button)
        {
            LPointerButtonEvent *pointerButtonEvent = (LPointerButtonEvent*)startDragEvent();

            if (pointerButtonEvent->button() == LPointerButtonEvent::Left && pointerButtonEvent->state() == LPointerButtonEvent::Pressed)
            {
                seat()->pointer()->setDraggingSurface(nullptr);

                if (icon())
                    icon()->surface()->setPos(cursor()->pos());
                return;
            }
        }
    }
    // Keyboard focus event
    else if (startDragEvent()->type() == LEvent::Type::Keyboard && startDragEvent()->subtype() == LEvent::Subtype::Enter &&
               seat()->keyboard()->focus() && seat()->keyboard()->focus()->client() == origin()->client())
    {
        seat()->pointer()->setDraggingSurface(nullptr);

        if (icon())
            icon()->surface()->setPos(cursor()->pos());

        return;
    }
    // Touch down event
    else if (startDragEvent()->type() == LEvent::Type::Touch && startDragEvent()->subtype() == LEvent::Subtype::Down)
    {
        LTouchDownEvent *touchDownEvent = (LTouchDownEvent*)startDragEvent();
        LTouchPoint *tp = seat()->touch()->findTouchPoint(touchDownEvent->id());

        if (tp && tp->surface() && tp->surface()->client() == origin()->client())
        {
            if (icon())
                icon()->surface()->setPos(LTouch::toGlobal(cursor()->output(), tp->pos()));
            return;
        }
    }

    cancel();
}
//! [startDragRequest]

//! [cancelled]
void LDNDManager::cancelled()
{
    if (icon())
        icon()->surface()->repaintOutputs();
}
//! [cancelled]
