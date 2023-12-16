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
    if (triggererEvent().type() == LEvent::Type::Pointer && seat()->pointer()->focus() && seat()->pointer()->focus()->client() == origin()->client())
    {
        if (triggererEvent().subtype() == LEvent::Subtype::Button)
        {
            LPointerButtonEvent &pointerButtonEvent = (LPointerButtonEvent&)triggererEvent();

            if (pointerButtonEvent.button() == LPointerButtonEvent::Left && pointerButtonEvent.state() == LPointerButtonEvent::Pressed)
            {
                seat()->pointer()->setDraggingSurface(nullptr);

                if (icon())
                    icon()->surface()->setPos(cursor()->pos());
                return;
            }
        }
    }
    // Keyboard focus event
    else if (triggererEvent().type() == LEvent::Type::Keyboard && triggererEvent().subtype() == LEvent::Subtype::Enter &&
               seat()->keyboard()->focus() && seat()->keyboard()->focus()->client() == origin()->client())
    {
        seat()->pointer()->setDraggingSurface(nullptr);

        if (icon())
            icon()->surface()->setPos(cursor()->pos());

        return;
    }
    // Touch down event
    else if (triggererEvent().type() == LEvent::Type::Touch && triggererEvent().subtype() == LEvent::Subtype::Down)
    {
        LTouchDownEvent &touchDownEvent = (LTouchDownEvent&)triggererEvent();
        LTouchPoint *tp = seat()->touch()->findTouchPoint(touchDownEvent.id());

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
