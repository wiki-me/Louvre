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

#include <unistd.h>

using LVS = LView::LViewPrivate::LViewState;

LScene::LScene() : LPRIVATE_INIT_UNIQUE(LScene)
{
    imp()->view.setPos(0);
    LView *baseView = &imp()->view;
    baseView->imp()->scene = this;
}

LScene::~LScene() {}

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

LView *LScene::handlePointerMoveEvent(const LPointerMoveEvent &event, LPointF *outLocalPos)
{
    // Prevent recursive calls
    if (imp()->handlingPointerMove)
        return nullptr;

    LSurface *surface = nullptr;
    LPointF localPos;
    imp()->currentPointerMoveEvent = event;

    if (event.isAbsolute())
        cursor()->setPos(imp()->currentPointerMoveEvent.pos());
    else
    {
        cursor()->move(imp()->currentPointerMoveEvent.pos());
        imp()->currentPointerMoveEvent.setIsAbsolute(true);
        imp()->currentPointerMoveEvent.setPos(cursor()->pos());
    }

    imp()->listChanged = false;
    imp()->pointerIsBlocked = false;
    imp()->pointerMoveEventFirstView = nullptr;

    imp()->handlingPointerMove = true;
    LView::LViewPrivate::removeFlagWithChildren(mainView(), LVS::PointerMoveDone);
    imp()->handlePointerMove(mainView());
    imp()->handlingPointerMove = false;

    if (imp()->pointerMoveEventFirstView)
    {
        localPos = imp()->viewLocalPos(imp()->pointerMoveEventFirstView, cursor()->pos());

        if (outLocalPos)
            *outLocalPos = localPos;

        if (imp()->pointerMoveEventFirstView->type() == LView::Surface)
        {
            LSurfaceView *surfaceView = (LSurfaceView*)imp()->pointerMoveEventFirstView;
            surface = surfaceView->surface();
        }
    }

    if (!handleWaylandPointerEventsEnabled())
        return imp()->pointerMoveEventFirstView;

    // Repaint cursor outputs if hardware composition is not supported
    cursor()->repaintOutputs(true);

    // Update the drag & drop icon (if there was one)
    if (seat()->dndManager()->icon())
    {
        seat()->dndManager()->icon()->surface()->setPos(cursor()->pos());
        seat()->dndManager()->icon()->surface()->repaintOutputs();
    }

    // Update the toplevel size (if there was one being resized)
    if (seat()->resizingToplevel())
    {
        seat()->updateResizingToplevelSize(cursor()->pos());
        return imp()->pointerMoveEventFirstView;
    }

    // Update the toplevel pos (if there was one being moved interactively)
    if (seat()->movingToplevel())
    {
        seat()->updateMovingToplevelPos(cursor()->pos());

        seat()->movingToplevel()->surface()->repaintOutputs();

        if (seat()->movingToplevel()->maximized())
            seat()->movingToplevel()->configure(seat()->movingToplevel()->pendingState() &~ LToplevelRole::Maximized);

        return imp()->pointerMoveEventFirstView;
    }

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

        return imp()->pointerMoveEventFirstView;
    }

    if (!surface)
        seat()->pointer()->setFocus(nullptr);
    else
    {
        surface->imp()->lastPointerEventView = (LSurfaceView*)imp()->pointerMoveEventFirstView;

        if (seat()->pointer()->focus() == surface)
        {
            imp()->currentPointerMoveEvent.localPos = localPos;
            seat()->pointer()->sendMoveEvent(imp()->currentPointerMoveEvent);
        }
        else
            seat()->pointer()->setFocus(surface, localPos);
    }

    return imp()->pointerMoveEventFirstView;
}

void LScene::handlePointerButtonEvent(const LPointerButtonEvent &event)
{
    // Prevent recursive calls
    if (imp()->handlingPointerButton)
        return;

    imp()->currentPointerButtonEvent = event;
    imp()->listChanged = false;
    imp()->handlingPointerButton = true;
    LView::LViewPrivate::removeFlagWithChildren(mainView(), LVS::PointerButtonDone);
    imp()->handlePointerButton(mainView());
    imp()->handlingPointerButton = false;

    if (!handleWaylandPointerEventsEnabled())
        return;

    if (imp()->currentPointerButtonEvent.button() == LPointerButtonEvent::Left &&
        imp()->currentPointerButtonEvent.state() == LPointerButtonEvent::Released)
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
            seat()->pointer()->sendButtonEvent(imp()->currentPointerButtonEvent);
        }
        // If no surface under the cursor
        else
        {
            seat()->keyboard()->setGrabbingSurface(nullptr, nullptr);
            seat()->pointer()->dismissPopups();
        }

        return;
    }

    seat()->pointer()->sendButtonEvent(imp()->currentPointerButtonEvent);

    if (imp()->currentPointerButtonEvent.button() != LPointerButtonEvent::Left)
        return;

    // Left button pressed
    if (imp()->currentPointerButtonEvent.state() == LPointerButtonEvent::Pressed)
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
        seat()->stopResizingToplevel();
        seat()->stopMovingToplevel();

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

void LScene::handlePointerScrollEvent(const LPointerScrollEvent &event)
{
    // Prevent recursive calls
    if (imp()->handlingPointerScrollEvent)
        return;

    imp()->currentPointerScrollEvent = event;
    imp()->listChanged = false;
    imp()->handlingPointerScrollEvent = true;
    LView::LViewPrivate::removeFlagWithChildren(mainView(), LVS::PointerScrollDone);
    imp()->handlePointerScrollEvent(mainView());
    imp()->handlingPointerScrollEvent = false;

    if (!handleWaylandPointerEventsEnabled())
        return;

    seat()->pointer()->sendScrollEvent(imp()->currentPointerScrollEvent);
}

bool LScene::handleWaylandPointerEventsEnabled() const
{
    return imp()->handleWaylandPointerEvents;
}

void LScene::enableHandleWaylandPointerEvents(bool enabled)
{
    imp()->handleWaylandPointerEvents = enabled;
}

void LScene::handleKeyEvent(const LKeyboardKeyEvent &event)
{
    // Prevent recursive calls
    if (imp()->handlingKeyEvent)
        return;

    imp()->currentKeyboardKeyEvent = event;
    imp()->listChanged = false;
    imp()->handlingKeyEvent = true;
    LView::LViewPrivate::removeFlagWithChildren(mainView(), LVS::KeyDone);
    imp()->handleKeyEvent(mainView());
    imp()->handlingKeyEvent = false;

    if (handleWaylandKeyboardEventsEnabled())
        seat()->keyboard()->sendKeyEvent(imp()->currentKeyboardKeyEvent);

    if (!auxKeyboardImplementationEnabled())
        return;

    bool L_CTRL = seat()->keyboard()->isKeyCodePressed(KEY_LEFTCTRL);
    bool L_SHIFT = seat()->keyboard()->isKeyCodePressed(KEY_LEFTSHIFT);
    bool mods = seat()->keyboard()->isKeyCodePressed(KEY_LEFTALT) && L_CTRL;

    if (imp()->currentKeyboardKeyEvent.state() == LKeyboardKeyEvent::Released)
    {
        // Terminates client connection
        if (L_CTRL && seat()->keyboard()->keySymbol(imp()->currentKeyboardKeyEvent.keyCode()) == XKB_KEY_q)
        {
            if (seat()->keyboard()->focus())
                seat()->keyboard()->focus()->client()->destroy();
        }

        // Minimizes currently focused surface
        else if (L_CTRL && seat()->keyboard()->keySymbol(imp()->currentKeyboardKeyEvent.keyCode()) == XKB_KEY_m)
        {
            if (seat()->keyboard()->focus() && seat()->keyboard()->focus()->toplevel() && !seat()->keyboard()->focus()->toplevel()->fullscreen())
                seat()->keyboard()->focus()->toplevel()->setMinimizedRequest();
        }

        // Terminates the compositor
        else if (imp()->currentKeyboardKeyEvent.keyCode() == KEY_ESC && L_CTRL && L_SHIFT)
            compositor()->finish();

        // Screenshot
        else if (L_CTRL && L_SHIFT && imp()->currentKeyboardKeyEvent.keyCode() == KEY_3)
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
        if (imp()->currentKeyboardKeyEvent.keyCode() == KEY_F1 && !mods)
            if (fork() == 0)
                exit(system("weston-terminal"));

        // CTRL sets Copy as the preferred action in drag & drop sesión
        if (L_CTRL)
            seat()->dndManager()->setPreferredAction(LDNDManager::Copy);

        // SHIFT sets the Move as the preferred action in drag & drop sesión
        else if (L_SHIFT)
            seat()->dndManager()->setPreferredAction(LDNDManager::Move);
    }
}

bool LScene::handleWaylandKeyboardEventsEnabled() const
{
    return imp()->handleWaylandKeyboardEvents;
}

void LScene::enableHandleWaylandKeyboardEvents(bool enabled)
{
    imp()->handleWaylandKeyboardEvents = enabled;
}

bool LScene::auxKeyboardImplementationEnabled() const
{
    return imp()->auxKeyboardImplementationEnabled;
}

void LScene::enableAuxKeyboardImplementation(bool enabled)
{
    imp()->auxKeyboardImplementationEnabled = enabled;
}

LSceneView *LScene::mainView() const
{
    return &imp()->view;
}

LView *LScene::viewAt(const LPoint &pos)
{
    return imp()->viewAt(mainView(), pos);
}
