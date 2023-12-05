#include <protocols/XdgShell/private/RXdgSurfacePrivate.h>
#include <protocols/XdgShell/RXdgToplevel.h>
#include <private/LBaseSurfaceRolePrivate.h>
#include <LToplevelResizeSession.h>
#include <LToplevelMoveSession.h>
#include <LCompositor.h>
#include <LCursor.h>
#include <LOutput.h>
#include <LSeat.h>
#include <LPointer.h>
#include <LKeyboard.h>
#include <LTouchDownEvent.h>
#include <LTouchPoint.h>

using namespace Louvre;

//! [rolePos]
const LPoint &LToplevelRole::rolePos() const
{
    m_rolePos = surface()->pos() - xdgSurfaceResource()->imp()->currentWindowGeometry.topLeft();
    return m_rolePos;
}
//! [rolePos]

//! [startMoveRequest]
void LToplevelRole::startMoveRequest(const LEvent &triggeringEvent)
{
    // Left pointer button click
    if (triggeringEvent.type() == LEvent::Type::Pointer && surface()->hasPointerFocus())
    {
        if (triggeringEvent.subtype() == LEvent::Subtype::Button)
        {
            LPointerButtonEvent &pointerButtonEvent = (LPointerButtonEvent&)triggeringEvent;

            if (pointerButtonEvent.button() == LPointerButtonEvent::Left && pointerButtonEvent.state() == LPointerButtonEvent::Pressed)
            {
                startMoveSession(triggeringEvent, cursor()->pos());
                return;
            }
        }
    }
    // Keyboard focus event
    else if (triggeringEvent.type() == LEvent::Type::Keyboard && triggeringEvent.subtype() == LEvent::Subtype::Enter && surface()->hasKeyboardFocus())
    {
        startMoveSession(triggeringEvent, cursor()->pos());
        return;
    }
    // Touch down event
    else if (triggeringEvent.type() == LEvent::Type::Touch && triggeringEvent.subtype() == LEvent::Subtype::Down)
    {
        LTouchDownEvent &touchDownEvent = (LTouchDownEvent&)triggeringEvent;
        LTouchPoint *tp = seat()->touch()->findTouchPoint(touchDownEvent.id());

        if (tp && tp->surface() == surface())
        {
            startMoveSession(triggeringEvent, LTouch::toGlobal(cursor()->output(), tp->pos()));
            return;
        }
    }
}
//! [startMoveRequest]

//! [startResizeRequest]
void LToplevelRole::startResizeRequest(const LEvent &triggeringEvent, ResizeEdge edge)
{
    // Left pointer button click
    if (triggeringEvent.type() == LEvent::Type::Pointer && surface()->hasPointerFocus())
    {
        if (triggeringEvent.subtype() == LEvent::Subtype::Button)
        {
            LPointerButtonEvent &pointerButtonEvent = (LPointerButtonEvent&)triggeringEvent;

            if (pointerButtonEvent.button() == LPointerButtonEvent::Left && pointerButtonEvent.state() == LPointerButtonEvent::Pressed)
            {
                startResizeSession(triggeringEvent, edge, cursor()->pos());
                return;
            }
        }
    }
    // Keyboard focus event
    else if (triggeringEvent.type() == LEvent::Type::Keyboard && triggeringEvent.subtype() == LEvent::Subtype::Enter && surface()->hasKeyboardFocus())
    {
        startResizeSession(triggeringEvent, edge, cursor()->pos());
        return;
    }
    // Touch down event
    else if (triggeringEvent.type() == LEvent::Type::Touch && triggeringEvent.subtype() == LEvent::Subtype::Down)
    {
        LTouchDownEvent &touchDownEvent = (LTouchDownEvent&)triggeringEvent;
        LTouchPoint *tp = seat()->touch()->findTouchPoint(touchDownEvent.id());

        if (tp && tp->surface() == surface())
        {
            startResizeSession(triggeringEvent, edge, LTouch::toGlobal(cursor()->output(), tp->pos()));
            return;
        }
    }
}
//! [startResizeRequest]

//! [resizingChanged]
void LToplevelRole::resizingChanged()
{
    /* No default implementation */
}
//! [resizingChanged]

//! [configureRequest]
void LToplevelRole::configureRequest()
{
    // Request the client to draw its own window decorations
    setDecorationMode(ClientSide);

    // Activates the toplevel with size (0,0) so that the client can decide the size
    configure(LSize(0,0), pendingState() | Activated);
}
//! [configureRequest]

//! [titleChanged]
void LToplevelRole::titleChanged()
{
    /* No default implementation */
}
//! [titleChanged]

//! [appIdChanged]
void LToplevelRole::appIdChanged()
{
    /* No default implementation */
}
//! [appIdChanged]

//! [geometryChanged]
void LToplevelRole::geometryChanged()
{
    /* No default implementation */
}
//! [geometryChanged]

//! [decorationModeChanged]
void LToplevelRole::decorationModeChanged()
{
    /* No default implementation */
}
//! [decorationModeChanged]

//! [preferredDecorationModeChanged]
void LToplevelRole::preferredDecorationModeChanged()
{
    /* No default implementation */
}
//! [preferredDecorationModeChanged]

//! [setMaximizedRequest]
void LToplevelRole::setMaximizedRequest()
{
    LOutput *output = compositor()->cursor()->output();
    configure(output->size(), Activated | Maximized);
}
//! [setMaximizedRequest]

//! [unsetMaximizedRequest]
void LToplevelRole::unsetMaximizedRequest()
{
    configure(pendingState() &~ Maximized);
}
//! [unsetMaximizedRequest]

//! [maximizedChanged]
void LToplevelRole::maximizedChanged()
{
    LOutput *output = cursor()->output();

    if (maximized())
    {
        surface()->raise();
        surface()->setPos(output->pos());
        surface()->setMinimized(false);
    }
}
//! [maximizedChanged]

//! [setFullscreenRequest]
void LToplevelRole::setFullscreenRequest(LOutput *destOutput)
{
    LOutput *output;

    if (destOutput)
        output = destOutput;
    else
        output = cursor()->output();

    configure(output->size(), Activated | Fullscreen);
}
//! [setFullscreenRequest]

//! [unsetFullscreenRequest]
void LToplevelRole::unsetFullscreenRequest()
{
    configure(pendingState() &~ Fullscreen);
}
//! [unsetFullscreenRequest]

//! [fullscreenChanged]
void LToplevelRole::fullscreenChanged()
{
    if (fullscreen())
    {
        surface()->setPos(cursor()->output()->pos());
        surface()->raise();
    }
}
//! [fullscreenChanged]

//! [activatedChanged]
void LToplevelRole::activatedChanged()
{
    if (activated())
        seat()->keyboard()->setFocus(surface());

    surface()->repaintOutputs();
}
//! [activatedChanged]

//! [statesChanged]
void LToplevelRole::statesChanged()
{
    /* No default implementation */
}
//! [statesChanged]

//! [maxSizeChanged]
void LToplevelRole::maxSizeChanged()
{
    /* No default implementation */
}
//! [maxSizeChanged]

//! [minSizeChanged]
void LToplevelRole::minSizeChanged()
{
    /* No default implementation */
}
//! [minSizeChanged]

//! [setMinimizedRequest]
void LToplevelRole::setMinimizedRequest()
{
    surface()->setMinimized(true);

    if (surface() == seat()->pointer()->focus())
        seat()->pointer()->setFocus(nullptr);

    if (surface() == seat()->keyboard()->focus())
        seat()->keyboard()->setFocus(nullptr);

    if (moveSession())
        moveSession()->stop();

    if (resizeSession())
        resizeSession()->stop();
}
//! [setMinimizedRequest]

//! [showWindowMenuRequest]
void LToplevelRole::showWindowMenuRequest(Int32 x, Int32 y)
{
    L_UNUSED(x);
    L_UNUSED(y);

    /* Here the compositor should render a context menu showing
     * the minimize, maximize and fullscreen options */
}
//! [showWindowMenuRequest]
