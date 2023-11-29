#include <LDNDManager.h>
#include <LSeat.h>
#include <LPointer.h>
#include <LCompositor.h>
#include <LDataSource.h>
#include <LTouchPoint.h>

using namespace Louvre;

//! [startDragRequest]
void LDNDManager::startDragRequest()
{
    switch (eventSource())
    {
    case InputEventSource::Pointer:
        if (origin()->hasPointerFocus())
            seat()->pointer()->setDraggingSurface(nullptr);
        else
            cancel();
        break;
    case InputEventSource::Touch:
        for (LTouchPoint *tp : seat()->touch()->touchPoints())
        {
            if (tp->id() == touchPointId())
                break;
        }
        cancel();
        break;
    case InputEventSource::Keyboard:
        cancel();
        break;
    default:
        break;
    }
}
//! [startDragRequest]

//! [cancelled]
void LDNDManager::cancelled()
{
    if (icon())
        icon()->surface()->repaintOutputs();
}
//! [cancelled]
