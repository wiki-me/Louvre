#include <private/LScenePrivate.h>
#include <private/LViewPrivate.h>
#include <private/LSceneViewPrivate.h>
#include <private/LSurfacePrivate.h>
#include <LSurfaceView.h>
#include <LOutput.h>
#include <LCursor.h>
#include <LSeat.h>
#include <LDNDManager.h>
#include <LDNDIconRole.h>
#include <LPointer.h>
#include <LCompositor.h>
#include <LToplevelResizeSession.h>
#include <LToplevelMoveSession.h>
#include <LLauncher.h>
#include <LPointerSwipeBeginEvent.h>
#include <LPointerPinchBeginEvent.h>
#include <LPointerHoldBeginEvent.h>
#include <unistd.h>

using LVS = LView::LViewPrivate::LViewState;
using LVES = LView::LViewPrivate::LViewEventsState;

LScene::LScene() : LPRIVATE_INIT_UNIQUE(LScene)
{
    imp()->view.setPos(0);
    LView *baseView = &imp()->view;
    baseView->imp()->currentScene = this;
    baseView->imp()->setFlag(LVS::IsScene, true);
}

LScene::~LScene() {}

const std::list<LView *> &LScene::pointerFocus() const
{
    return imp()->pointerFocus;
}

const std::list<LView *> &LScene::keyboardFocus() const
{
    return imp()->keyboardFocus;
}

void LScene::handleInitializeGL(LOutput *output)
{
    imp()->mutex.lock();
    imp()->view.imp()->fb = output->framebuffer();
    imp()->mutex.unlock();
}

void LScene::handlePaintGL(LOutput *output)
{
    imp()->mutex.lock();
    imp()->view.imp()->fb = output->framebuffer();
    imp()->view.render();
    imp()->mutex.unlock();
}

void LScene::handleMoveGL(LOutput *output)
{
    imp()->mutex.lock();
    imp()->view.imp()->fb = output->framebuffer();
    imp()->view.damageAll(output);
    imp()->mutex.unlock();
}

void LScene::handleResizeGL(LOutput *output)
{
    imp()->mutex.lock();
    imp()->view.damageAll(output);
    imp()->mutex.unlock();
}

void LScene::handleUninitializeGL(LOutput *output)
{
    L_UNUSED(output);
    imp()->mutex.lock();
    auto it = imp()->view.imp()->threadsMap.find(output->threadId());

    if (it != imp()->view.imp()->threadsMap.end())
        imp()->view.imp()->threadsMap.erase(it);
    imp()->mutex.unlock();
}

void LScene::handlePointerMoveEvent(const LPointerMoveEvent &event, EventOptionsFlags options)
{
    // Prevent recursive calls
    if (imp()->handlingPointerMove)
        return;

    imp()->currentPointerMoveEvent = event;

    imp()->currentPointerEnterEvent.setDevice(event.device());
    imp()->currentPointerEnterEvent.setMs(event.ms());
    imp()->currentPointerEnterEvent.setUs(event.us());
    imp()->currentPointerEnterEvent.setSerial(event.serial());

    imp()->currentPointerLeaveEvent.setDevice(event.device());
    imp()->currentPointerLeaveEvent.setMs(event.ms());
    imp()->currentPointerLeaveEvent.setUs(event.us());
    imp()->currentPointerLeaveEvent.setSerial(event.serial());

    cursor()->move(event.delta());

    imp()->listChanged = false;
    imp()->pointerIsBlocked = false;

    imp()->handlingPointerMove = true;
    LView::LViewPrivate::removeEventFlagWithChildren(mainView(), LVES::PointerMoveDone);
    imp()->handlePointerMove(mainView());
    imp()->handlingPointerMove = false;

    if (!(options & WaylandEvents))
        return;

    LSurface *surface = nullptr;
    LPointF localPos;
    LSurfaceView *firstSurfaceView = nullptr;

    for (LView *view : pointerFocus())
        if (view->type() == LView::Surface)
        {
            firstSurfaceView = (LSurfaceView*)view;
        }

    if (firstSurfaceView)
    {
        localPos = imp()->viewLocalPos(firstSurfaceView , cursor()->pos());
        surface = firstSurfaceView->surface();
    }

    // Repaint cursor outputs if hardware composition is not supported
    cursor()->repaintOutputs(true);

    // Update the drag & drop icon (if there was one)
    if (seat()->dndManager()->icon())
    {
        seat()->dndManager()->icon()->surface()->setPos(cursor()->pos());
        seat()->dndManager()->icon()->surface()->repaintOutputs();
    }

    bool activeResizing = false;

    for (LToplevelResizeSession *session : seat()->resizeSessions())
    {
        if (session->triggeringEvent().type() != LEvent::Type::Touch)
        {
            activeResizing = true;
            session->setResizePointPos(cursor()->pos());
        }
    }

    if (activeResizing)
        return;

    bool activeMoving = false;

    for (LToplevelMoveSession *session : seat()->moveSessions())
    {
        if (session->triggeringEvent().type() != LEvent::Type::Touch)
        {
            activeMoving = true;
            session->setMovePointPos(cursor()->pos());
            session->toplevel()->surface()->repaintOutputs();

            if (session->toplevel()->maximized())
                session->toplevel()->configure(session->toplevel()->pendingState() &~ LToplevelRole::Maximized);
        }
    }

    if (activeMoving)
        return;

    // DO NOT GET CONFUSED! If we are in a drag & drop session, we call setDragginSurface(NULL) in case there is a surface being dragged.
    if (seat()->dndManager()->dragging())
        seat()->pointer()->setDraggingSurface(nullptr);

    // If there was a surface holding the left pointer button
    if (seat()->pointer()->draggingSurface())
    {
        if (seat()->pointer()->draggingSurface()->imp()->lastPointerEventView)
        {
            imp()->currentPointerMoveEvent.localPos = imp()->viewLocalPos(seat()->pointer()->draggingSurface()->imp()->lastPointerEventView, cursor()->pos());
            seat()->pointer()->sendMoveEvent(imp()->currentPointerMoveEvent);
        }
        else
        {
            imp()->currentPointerMoveEvent.localPos = cursor()->pos() - seat()->pointer()->draggingSurface()->rolePos();
            seat()->pointer()->sendMoveEvent(imp()->currentPointerMoveEvent);
        }

        return;
    }

    if (!surface)
        seat()->pointer()->setFocus(nullptr);
    else
    {
        surface->imp()->lastPointerEventView = (LSurfaceView*)firstSurfaceView;

        if (seat()->pointer()->focus() == surface)
        {
            imp()->currentPointerMoveEvent.localPos = localPos;
            seat()->pointer()->sendMoveEvent(imp()->currentPointerMoveEvent);
        }
        else
            seat()->pointer()->setFocus(surface, localPos);
    }

    return;
}

void LScene::handlePointerButtonEvent(const LPointerButtonEvent &event, EventOptionsFlags options)
{
    // Prevent recursive calls
    if (imp()->handlingPointerButton)
        return;

    imp()->pointerListChanged = false;
    imp()->handlingPointerButton = true;

    for (LView *view : imp()->pointerFocus)
        view->imp()->removeEventFlag(LVES::PointerButtonDone);

retry:

    for (LView *view : imp()->pointerFocus)
    {
        if (view->imp()->hasEventFlag(LVES::PointerButtonDone))
            continue;

        view->imp()->addEventFlag(LVES::PointerButtonDone);
        view->pointerButtonEvent(event);

        if (imp()->pointerListChanged)
        {
            imp()->pointerListChanged = false;
            goto retry;
        }
    }

    imp()->handlingPointerButton = false;

    if (!(options & WaylandEvents))
        return;

    if (event.button() == LPointerButtonEvent::Left &&
        event.state() == LPointerButtonEvent::Released)
        seat()->dndManager()->drop();

    if (!seat()->pointer()->focus())
    {
        LSurface *surface = nullptr;
        LView *view = viewAt(cursor()->pos());

        if (view && view->type() == LView::Surface)
        {
            LSurfaceView *surfaceView = (LSurfaceView*)view;
            surface = surfaceView->surface();
        }

        if (surface)
        {
            if (seat()->keyboard()->grabbingSurface() && seat()->keyboard()->grabbingSurface()->client() != surface->client())
            {
                seat()->keyboard()->setGrabbingSurface(nullptr, nullptr);
                seat()->pointer()->dismissPopups();
            }

            if (!seat()->keyboard()->focus() || !surface->isSubchildOf(seat()->keyboard()->focus()))
                seat()->keyboard()->setFocus(surface);

            seat()->pointer()->setFocus(surface, imp()->viewLocalPos(view, cursor()->pos()));
            seat()->pointer()->sendButtonEvent(event);
        }
        // If no surface under the cursor
        else
        {
            seat()->keyboard()->setGrabbingSurface(nullptr, nullptr);
            seat()->pointer()->dismissPopups();
        }

        return;
    }

    seat()->pointer()->sendButtonEvent(event);

    if (event.button() != LPointerButtonEvent::Left)
        return;

    // Left button pressed
    if (event.state() == LPointerButtonEvent::Pressed)
    {
        /* We save the pointer focus surface in order to continue sending events to it even when the cursor
         * is outside of it (while the left button is being held down)*/
        seat()->pointer()->setDraggingSurface(seat()->pointer()->focus());

        if (seat()->keyboard()->grabbingSurface() && seat()->keyboard()->grabbingSurface()->client() != seat()->pointer()->focus()->client())
        {
            seat()->keyboard()->setGrabbingSurface(nullptr, nullptr);
            seat()->pointer()->dismissPopups();
        }

        if (!seat()->pointer()->focus()->popup())
            seat()->pointer()->dismissPopups();

        if (!seat()->keyboard()->focus() || !seat()->pointer()->focus()->isSubchildOf(seat()->keyboard()->focus()))
            seat()->keyboard()->setFocus(seat()->pointer()->focus());

        if (seat()->pointer()->focus()->toplevel() && !seat()->pointer()->focus()->toplevel()->activated())
            seat()->pointer()->focus()->toplevel()->configure(seat()->pointer()->focus()->toplevel()->pendingState() | LToplevelRole::Activated);

        // Raise surface
        if (seat()->pointer()->focus() == compositor()->surfaces().back())
            return;

        if (seat()->pointer()->focus()->parent())
            seat()->pointer()->focus()->topmostParent()->raise();
        else
            seat()->pointer()->focus()->raise();
    }
    // Left button released
    else
    {
        // Stop pointer toplevel resizing sessions
        for (std::list<LToplevelResizeSession*>::const_iterator it = seat()->resizeSessions().begin(); it != seat()->resizeSessions().end(); it++)
            if ((*it)->triggeringEvent().type() != LEvent::Type::Touch)
                it = (*it)->stop();

        // Stop pointer toplevel moving sessions
        for (std::list<LToplevelMoveSession*>::const_iterator it = seat()->moveSessions().begin(); it != seat()->moveSessions().end(); it++)
            if ((*it)->triggeringEvent().type() != LEvent::Type::Touch)
                it = (*it)->stop();

        // We stop sending events to the surface on which the left button was being held down
        seat()->pointer()->setDraggingSurface(nullptr);

        if (seat()->pointer()->focus()->imp()->lastPointerEventView)
        {
            if (!imp()->pointerIsOverView(seat()->pointer()->focus()->imp()->lastPointerEventView, cursor()->pos()))
            {
                seat()->keyboard()->setGrabbingSurface(nullptr, nullptr);
                seat()->pointer()->setFocus(nullptr);
            }
        }
        else
        {
            if (!seat()->pointer()->focus()->inputRegion().containsPoint(cursor()->pos() - seat()->pointer()->focus()->pos()))
            {
                seat()->keyboard()->setGrabbingSurface(nullptr, nullptr);
                seat()->pointer()->setFocus(nullptr);
            }
        }
    }
}

void LScene::handlePointerScrollEvent(const LPointerScrollEvent &event, EventOptionsFlags options)
{
    // Prevent recursive calls
    if (imp()->handlingPointerScrollEvent)
        return;

    imp()->pointerListChanged = false;
    imp()->handlingPointerScrollEvent = true;

    for (LView *view : imp()->pointerFocus)
        view->imp()->removeEventFlag(LVES::PointerScrollDone);

retry:

    for (LView *view : imp()->pointerFocus)
    {
        if (view->imp()->hasEventFlag(LVES::PointerScrollDone))
            continue;

        view->imp()->addEventFlag(LVES::PointerScrollDone);
        view->pointerScrollEvent(event);

        if (imp()->pointerListChanged)
        {
            imp()->pointerListChanged = false;
            goto retry;
        }
    }

    imp()->handlingPointerScrollEvent = false;

    if (!(options & WaylandEvents))
        return;

    seat()->pointer()->sendScrollEvent(event);
}

void LScene::handlePointerSwipeBeginEvent(const LPointerSwipeBeginEvent &event, EventOptionsFlags options)
{
    // Prevent recursive calls
    if (imp()->handlingPointerSwipeBeginEvent)
        return;

    imp()->pointerSwipeEndEvent.setFingers(event.fingers());
    imp()->pointerSwipeEndEvent.setDevice(event.device());
    imp()->pointerListChanged = false;
    imp()->handlingPointerSwipeBeginEvent = true;

    for (LView *view : imp()->pointerFocus)
        view->imp()->removeEventFlag(LVES::PointerSwipeBeginDone);

retry:

    for (LView *view : imp()->pointerFocus)
    {
        if (view->imp()->hasEventFlag(LVES::PointerSwipeBeginDone))
            continue;

        view->imp()->addEventFlag(LVES::PointerSwipeBeginDone);

        if (!view->imp()->hasEventFlag(LVES::PendingSwipeEnd))
        {
            view->imp()->addEventFlag(LVES::PendingSwipeEnd);
            view->pointerSwipeBeginEvent(event);
        }

        if (imp()->pointerListChanged)
        {
            imp()->pointerListChanged = false;
            goto retry;
        }
    }

    imp()->handlingPointerSwipeBeginEvent = false;

    if (!(options & WaylandEvents))
        return;

    seat()->pointer()->sendSwipeBeginEvent(event);
}

void LScene::handlePointerSwipeUpdateEvent(const LPointerSwipeUpdateEvent &event, EventOptionsFlags options)
{
    // Prevent recursive calls
    if (imp()->handlingPointerSwipeUpdateEvent)
        return;

    imp()->pointerListChanged = false;
    imp()->handlingPointerSwipeUpdateEvent = true;

    for (LView *view : imp()->pointerFocus)
        view->imp()->removeEventFlag(LVES::PointerSwipeUpdateDone);

retry:

    for (LView *view : imp()->pointerFocus)
    {
        if (view->imp()->hasEventFlag(LVES::PointerSwipeUpdateDone))
            continue;

        view->imp()->addEventFlag(LVES::PointerSwipeUpdateDone);

        if (view->imp()->hasEventFlag(LVES::PendingSwipeEnd))
            view->pointerSwipeUpdateEvent(event);

        if (imp()->pointerListChanged)
        {
            imp()->pointerListChanged = false;
            goto retry;
        }
    }

    imp()->handlingPointerSwipeUpdateEvent = false;

    if (!(options & WaylandEvents))
        return;

    seat()->pointer()->sendSwipeUpdateEvent(event);
}

void LScene::handlePointerSwipeEndEvent(const LPointerSwipeEndEvent &event, EventOptionsFlags options)
{
    // Prevent recursive calls
    if (imp()->handlingPointerSwipeEndEvent)
        return;

    imp()->pointerListChanged = false;
    imp()->handlingPointerSwipeEndEvent = true;

    for (LView *view : imp()->pointerFocus)
        view->imp()->removeEventFlag(LVES::PointerSwipeEndDone);

retry:

    for (LView *view : imp()->pointerFocus)
    {
        if (view->imp()->hasEventFlag(LVES::PointerSwipeEndDone))
            continue;

        view->imp()->addEventFlag(LVES::PointerSwipeEndDone);

        if (view->imp()->hasEventFlag(LVES::PendingSwipeEnd))
        {
            view->imp()->removeEventFlag(LVES::PendingSwipeEnd);
            view->pointerSwipeEndEvent(event);
        }

        if (imp()->pointerListChanged)
        {
            imp()->pointerListChanged = false;
            goto retry;
        }
    }

    imp()->handlingPointerSwipeEndEvent = false;

    if (!(options & WaylandEvents))
        return;

    seat()->pointer()->sendSwipeEndEvent(event);
}

void LScene::handlePointerPinchBeginEvent(const LPointerPinchBeginEvent &event, EventOptionsFlags options)
{
    // Prevent recursive calls
    if (imp()->handlingPointerPinchBeginEvent)
        return;

    imp()->pointerPinchEndEvent.setFingers(event.fingers());
    imp()->pointerPinchEndEvent.setDevice(event.device());
    imp()->pointerListChanged = false;
    imp()->handlingPointerPinchBeginEvent = true;

    for (LView *view : imp()->pointerFocus)
        view->imp()->removeEventFlag(LVES::PointerPinchBeginDone);

retry:

    for (LView *view : imp()->pointerFocus)
    {
        if (view->imp()->hasEventFlag(LVES::PointerPinchBeginDone))
            continue;

        view->imp()->addEventFlag(LVES::PointerPinchBeginDone);

        if (!view->imp()->hasEventFlag(LVES::PendingPinchEnd))
        {
            view->imp()->addEventFlag(LVES::PendingPinchEnd);
            view->pointerPinchBeginEvent(event);
        }

        if (imp()->pointerListChanged)
        {
            imp()->pointerListChanged = false;
            goto retry;
        }
    }

    imp()->handlingPointerPinchBeginEvent = false;

    if (!(options & WaylandEvents))
        return;

    seat()->pointer()->sendPinchBeginEvent(event);
}

void LScene::handlePointerPinchUpdateEvent(const LPointerPinchUpdateEvent &event, EventOptionsFlags options)
{
    // Prevent recursive calls
    if (imp()->handlingPointerPinchUpdateEvent)
        return;

    imp()->pointerListChanged = false;
    imp()->handlingPointerPinchUpdateEvent = true;

    for (LView *view : imp()->pointerFocus)
        view->imp()->removeEventFlag(LVES::PointerPinchUpdateDone);

retry:

    for (LView *view : imp()->pointerFocus)
    {
        if (view->imp()->hasEventFlag(LVES::PointerPinchUpdateDone))
            continue;

        view->imp()->addEventFlag(LVES::PointerPinchUpdateDone);

        if (view->imp()->hasEventFlag(LVES::PendingPinchEnd))
            view->pointerPinchUpdateEvent(event);

        if (imp()->pointerListChanged)
        {
            imp()->pointerListChanged = false;
            goto retry;
        }
    }

    imp()->handlingPointerPinchUpdateEvent = false;

    if (!(options & WaylandEvents))
        return;

    seat()->pointer()->sendPinchUpdateEvent(event);
}

void LScene::handlePointerPinchEndEvent(const LPointerPinchEndEvent &event, EventOptionsFlags options)
{
    // Prevent recursive calls
    if (imp()->handlingPointerPinchEndEvent)
        return;

    imp()->pointerListChanged = false;
    imp()->handlingPointerPinchEndEvent = true;

    for (LView *view : imp()->pointerFocus)
        view->imp()->removeEventFlag(LVES::PointerPinchEndDone);

retry:

    for (LView *view : imp()->pointerFocus)
    {
        if (view->imp()->hasEventFlag(LVES::PointerPinchEndDone))
            continue;

        view->imp()->addEventFlag(LVES::PointerPinchEndDone);

        if (view->imp()->hasEventFlag(LVES::PendingPinchEnd))
        {
            view->imp()->removeEventFlag(LVES::PendingPinchEnd);
            view->pointerPinchEndEvent(event);
        }

        if (imp()->pointerListChanged)
        {
            imp()->pointerListChanged = false;
            goto retry;
        }
    }

    imp()->handlingPointerPinchEndEvent = false;

    if (!(options & WaylandEvents))
        return;

    seat()->pointer()->sendPinchEndEvent(event);
}

void LScene::handlePointerHoldBeginEvent(const LPointerHoldBeginEvent &event, EventOptionsFlags options)
{
    // Prevent recursive calls
    if (imp()->handlingPointerHoldBeginEvent)
        return;

    imp()->pointerHoldEndEvent.setFingers(event.fingers());
    imp()->pointerHoldEndEvent.setDevice(event.device());
    imp()->pointerListChanged = false;
    imp()->handlingPointerHoldBeginEvent = true;

    for (LView *view : imp()->pointerFocus)
        view->imp()->removeEventFlag(LVES::PointerHoldBeginDone);

retry:

    for (LView *view : imp()->pointerFocus)
    {
        if (view->imp()->hasEventFlag(LVES::PointerHoldBeginDone))
            continue;

        view->imp()->addEventFlag(LVES::PointerHoldBeginDone);

        if (!view->imp()->hasEventFlag(LVES::PendingHoldEnd))
        {
            view->imp()->addEventFlag(LVES::PendingHoldEnd);
            view->pointerHoldBeginEvent(event);
        }

        if (imp()->pointerListChanged)
        {
            imp()->pointerListChanged = false;
            goto retry;
        }
    }

    imp()->handlingPointerHoldBeginEvent = false;

    if (!(options & WaylandEvents))
        return;

    seat()->pointer()->sendHoldBeginEvent(event);
}

void LScene::handlePointerHoldEndEvent(const LPointerHoldEndEvent &event, EventOptionsFlags options)
{
    // Prevent recursive calls
    if (imp()->handlingPointerHoldEndEvent)
        return;

    imp()->pointerListChanged = false;
    imp()->handlingPointerHoldEndEvent = true;

    for (LView *view : imp()->pointerFocus)
        view->imp()->removeEventFlag(LVES::PointerHoldEndDone);

retry:

    for (LView *view : imp()->pointerFocus)
    {
        if (view->imp()->hasEventFlag(LVES::PointerHoldEndDone))
            continue;

        view->imp()->addEventFlag(LVES::PointerHoldEndDone);

        if (view->imp()->hasEventFlag(LVES::PendingHoldEnd))
        {
            view->imp()->removeEventFlag(LVES::PendingHoldEnd);
            view->pointerHoldEndEvent(event);
        }

        if (imp()->pointerListChanged)
        {
            imp()->pointerListChanged = false;
            goto retry;
        }
    }

    imp()->handlingPointerHoldEndEvent = false;

    if (!(options & WaylandEvents))
        return;

    seat()->pointer()->sendHoldEndEvent(event);
}

void LScene::handleKeyEvent(const LKeyboardKeyEvent &event, EventOptionsFlags options)
{
    // Prevent recursive calls
    if (imp()->handlingKeyEvent)
        return;

    imp()->keyboardListChanged = false;
    imp()->handlingKeyEvent = true;

    for (LView *view : imp()->keyboardFocus)
        view->imp()->removeEventFlag(LVES::KeyDone);

retry:

    for (LView *view : imp()->keyboardFocus)
    {
        if (view->imp()->hasEventFlag(LVES::KeyDone))
            continue;

        view->imp()->addEventFlag(LVES::KeyDone);
        view->keyEvent(event);

        if (imp()->keyboardListChanged)
        {
            imp()->keyboardListChanged = false;
            goto retry;
        }
    }

    imp()->handlingKeyEvent = false;

    if (options & WaylandEvents)
        seat()->keyboard()->sendKeyEvent(event);

    if (!(options & AuxFunc))
        return;

    bool L_CTRL = seat()->keyboard()->isKeyCodePressed(KEY_LEFTCTRL);
    bool L_SHIFT = seat()->keyboard()->isKeyCodePressed(KEY_LEFTSHIFT);
    bool mods = seat()->keyboard()->isKeyCodePressed(KEY_LEFTALT) && L_CTRL;

    if (event.state() == LKeyboardKeyEvent::Released)
    {
        // Terminates client connection
        if (L_CTRL && seat()->keyboard()->keySymbol(event.keyCode()) == XKB_KEY_q)
        {
            if (seat()->keyboard()->focus())
                seat()->keyboard()->focus()->client()->destroy();
        }

        // Minimizes currently focused surface
        else if (L_CTRL && seat()->keyboard()->keySymbol(event.keyCode()) == XKB_KEY_m)
        {
            if (seat()->keyboard()->focus() && seat()->keyboard()->focus()->toplevel() && !seat()->keyboard()->focus()->toplevel()->fullscreen())
                seat()->keyboard()->focus()->toplevel()->setMinimizedRequest();
        }

        // Terminates the compositor
        else if (event.keyCode() == KEY_ESC && L_CTRL && L_SHIFT)
            compositor()->finish();

        // Screenshot
        else if (L_CTRL && L_SHIFT && event.keyCode() == KEY_3)
        {
            if (cursor() && cursor()->output()->bufferTexture(0))
            {
                const char *user = getenv("HOME");

                if (!user)
                    return;

                char path[128];
                char timeString[32];

                time_t currentTime;
                struct tm *timeInfo;

                time(&currentTime);
                timeInfo = localtime(&currentTime);
                strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", timeInfo);

                sprintf(path, "%s/Desktop/Louvre_Screenshoot_%s.png", user, timeString);

                cursor()->output()->bufferTexture(0)->save(path);
            }
        }

        else if (L_CTRL && !L_SHIFT)
            seat()->dndManager()->setPreferredAction(LDNDManager::Copy);
        else if (!L_CTRL && L_SHIFT)
            seat()->dndManager()->setPreferredAction(LDNDManager::Move);
        else if (!L_CTRL && !L_SHIFT)
            seat()->dndManager()->setPreferredAction(LDNDManager::NoAction);
    }

    // Key press
    else
    {
        // Launches weston-terminal
        if (event.keyCode() == KEY_F1 && !mods)
            LLauncher::launch("weston-terminal");

        // CTRL sets Copy as the preferred action in drag & drop sesión
        if (L_CTRL)
            seat()->dndManager()->setPreferredAction(LDNDManager::Copy);

        // SHIFT sets the Move as the preferred action in drag & drop sesión
        else if (L_SHIFT)
            seat()->dndManager()->setPreferredAction(LDNDManager::Move);
    }
}

LSceneView *LScene::mainView() const
{
    return &imp()->view;
}

LView *LScene::viewAt(const LPoint &pos)
{
    return imp()->viewAt(mainView(), pos);
}
