#include <protocols/XdgDecoration/private/RXdgToplevelDecorationPrivate.h>
#include <protocols/XdgShell/private/RXdgSurfacePrivate.h>
#include <protocols/XdgShell/RXdgToplevel.h>
#include <private/LToplevelMoveSessionPrivate.h>
#include <private/LToplevelResizeSessionPrivate.h>
#include <private/LToplevelRolePrivate.h>
#include <private/LBaseSurfaceRolePrivate.h>
#include <private/LSurfacePrivate.h>
#include <private/LSeatPrivate.h>
#include <protocols/XdgShell/xdg-shell.h>
#include <LCompositor.h>
#include <LOutput.h>
#include <LPoint.h>
#include <LCursor.h>
#include <LPointer.h>
#include <LKeyboard.h>
#include <LTime.h>
#include <string.h>
#include <ctype.h>

#undef None

using namespace Louvre;

LToplevelRole::LToplevelRole(Louvre::LToplevelRole::Params *params) :
    LBaseSurfaceRole(params->toplevel, params->surface, LSurface::Role::Toplevel),
    LPRIVATE_INIT_UNIQUE(LToplevelRole)
{
    imp()->currentConf.commited = true;
    imp()->toplevel = this;
}

LToplevelRole::~LToplevelRole()
{
    if (surface())
        surface()->imp()->setMapped(false);

    // Remove focus
    if (resizeSession())
        resizeSession()->imp()->destroy();

    if (moveSession())
        moveSession()->stop();

    if (seat()->activeToplevel() == this)
        seat()->imp()->activeToplevel = nullptr;

    if (imp()->xdgDecoration)
        imp()->xdgDecoration->imp()->lToplevelRole = nullptr;
}

bool LToplevelRole::maximized() const
{
    return imp()->currentConf.flags & LToplevelRole::Maximized;
}

bool LToplevelRole::fullscreen() const
{
    return imp()->currentConf.flags & LToplevelRole::Fullscreen;
}

bool LToplevelRole::activated() const
{
    return imp()->currentConf.flags & LToplevelRole::Activated;
}

UInt32 LToplevelRole::currentState() const
{
    return imp()->currentConf.flags;
}

LToplevelRole::States LToplevelRole::pendingState() const
{
    return imp()->pendingSendConf.flags;
}

void LToplevelRole::setDecorationMode(DecorationMode mode)
{
    if (decorationMode() == mode)
        return;

    if (mode != DecorationMode::ClientSide && mode != DecorationMode::ServerSide)
        return;

    imp()->pendingDecorationMode = mode;
}

LToplevelRole::DecorationMode LToplevelRole::decorationMode() const
{
    return imp()->decorationMode;
}

LToplevelRole::DecorationMode LToplevelRole::preferredDecorationMode() const
{
    return imp()->preferredDecorationMode;
}

RXdgToplevel *LToplevelRole::xdgToplevelResource() const
{
    return (RXdgToplevel*)resource();
}

RXdgSurface *LToplevelRole::xdgSurfaceResource() const
{
    return xdgToplevelResource()->xdgSurfaceResource();
}

void LToplevelRole::handleSurfaceCommit(Protocols::Wayland::RSurface::CommitOrigin origin)
{
    L_UNUSED(origin);

    // Apply pending role
    if (surface()->imp()->pending.role)
    {
        if (imp()->hasPendingMaxSize)
        {
            imp()->hasPendingMaxSize = false;
            imp()->currentMaxSize = imp()->pendingMaxSize;
        }

        if (imp()->hasPendingMinSize)
        {
            imp()->hasPendingMinSize = false;
            imp()->currentMinSize = imp()->pendingMinSize;
        }

        if (xdgSurfaceResource()->imp()->hasPendingWindowGeometry)
        {
            xdgSurfaceResource()->imp()->hasPendingWindowGeometry = false;
            xdgSurfaceResource()->imp()->currentWindowGeometry = xdgSurfaceResource()->imp()->pendingWindowGeometry;
        }
        // Si nunca ha asignado la geometría, usa el tamaño de la superficie
        else if (!xdgSurfaceResource()->imp()->windowGeometrySet &&
                 xdgSurfaceResource()->imp()->currentWindowGeometry.size() != surface()->size())
        {
            xdgSurfaceResource()->imp()->currentWindowGeometry = LRect(0, surface()->size());
        }

        if (surface()->buffer())
        {
            wl_resource_post_error(resource()->resource(), XDG_SURFACE_ERROR_ALREADY_CONSTRUCTED, "Given wl_surface already has a buffer attached.");
            return;
        }

        surface()->imp()->applyPendingRole();
        configureRequest();

        // Fake set maximized or fullscreen request if called before the role was applied
        if (imp()->prevRoleRequest == LToplevelRole::Maximized)
            setMaximizedRequest();
        else if (imp()->prevRoleRequest == LToplevelRole::Fullscreen)
        {
            // Validate the output is still initialized
            if (imp()->prevRoleFullscreenRequestOutput)
            {
                for (LOutput *output : compositor()->outputs())
                {
                    if (output == imp()->prevRoleFullscreenRequestOutput)
                        goto skipUnsetOutput;
                }

                imp()->prevRoleFullscreenRequestOutput = nullptr;
            }

            skipUnsetOutput:
            setFullscreenRequest(imp()->prevRoleFullscreenRequestOutput);
        }

        imp()->prevRoleRequest = 0;
        imp()->prevRoleFullscreenRequestOutput = nullptr;

        return;
    }

    /* First apply all changes */

    if (imp()->hasPendingMaxSize)
        imp()->currentMaxSize = imp()->pendingMaxSize;

    if (imp()->hasPendingMinSize)
        imp()->currentMinSize = imp()->pendingMinSize;

    if (xdgSurfaceResource()->imp()->hasPendingWindowGeometry)
        xdgSurfaceResource()->imp()->currentWindowGeometry = xdgSurfaceResource()->imp()->pendingWindowGeometry;
    // If never assigned, use the surface size
    else if (!xdgSurfaceResource()->imp()->windowGeometrySet &&
             xdgSurfaceResource()->imp()->currentWindowGeometry.size() != surface()->size())
    {
        xdgSurfaceResource()->imp()->hasPendingWindowGeometry = true;
        xdgSurfaceResource()->imp()->currentWindowGeometry = LRect(0, surface()->size());
    }

    /* Then notify all changes */

    imp()->applyPendingChanges();

    if (imp()->hasPendingMinSize)
    {
        imp()->hasPendingMinSize = false;
        minSizeChanged();
    }

    if (imp()->hasPendingMaxSize)
    {
        imp()->hasPendingMaxSize = false;
        maxSizeChanged();
    }

    if (xdgSurfaceResource()->imp()->hasPendingWindowGeometry)
    {
        if (resizeSession())
            resizeSession()->imp()->handleGeometryChange();

        xdgSurfaceResource()->imp()->hasPendingWindowGeometry = false;
        geometryChanged();
    }

    // Request configure
    if (!surface()->mapped() && !surface()->buffer())
    {
        configureRequest();
        return;
    }

    // Request unmap
    if (surface()->mapped() && !surface()->buffer())
    {
        surface()->imp()->setMapped(false);

        // If a surface becomes unmapped, its children's parent is set to the parent of the now-unmapped surface
        while (!surface()->children().empty())
        {
            if (surface()->children().front()->subsurface())
                surface()->children().front()->imp()->setMapped(false);

            surface()->children().front()->imp()->setParent(surface()->parent());
        }

        surface()->imp()->setParent(nullptr);

        if (moveSession())
            moveSession()->stop();

        if (resizeSession())
            resizeSession()->imp()->destroy();

        if (seat()->activeToplevel() == this)
            seat()->imp()->activeToplevel = nullptr;

        // Clean Up
        imp()->setAppId("");
        imp()->setTitle("");
        imp()->currentConf.commited = false;
        imp()->currentConf.size = LSize();
        imp()->currentConf.flags = NoState;
        imp()->currentConf.serial = 0;
        imp()->pendingSendConf = imp()->currentConf;
        imp()->sentConfs.clear();
        imp()->hasPendingMinSize = false;
        imp()->hasPendingMaxSize = false;
        imp()->currentMinSize = LSize();
        imp()->pendingMinSize = LSize();
        imp()->currentMaxSize = LSize();
        imp()->pendingMaxSize = LSize();

        XdgShell::RXdgToplevel *rXdgToplevel = (XdgShell::RXdgToplevel*)resource();
        rXdgToplevel->xdgSurfaceResource()->imp()->hasPendingWindowGeometry = false;
        rXdgToplevel->xdgSurfaceResource()->imp()->windowGeometrySet = false;
        rXdgToplevel->xdgSurfaceResource()->imp()->pendingWindowGeometry = LRect();
        rXdgToplevel->xdgSurfaceResource()->imp()->currentWindowGeometry = LRect();
        return;
    }

    // Request map
    if (!surface()->mapped() && surface()->buffer())
    {
        surface()->imp()->setMapped(true);
        return;
    }
}

const std::string &LToplevelRole::appId() const
{
    return imp()->appId;
}

const std::string &LToplevelRole::title() const
{
    return imp()->title;
}

const LSize &LToplevelRole::maxSize() const
{
    return imp()->currentMaxSize;
}

bool LToplevelRole::resizing() const
{
    return imp()->currentConf.flags & LToplevelRole::Resizing;
}

const LSize &LToplevelRole::minSize() const
{
    return imp()->currentMinSize;
}

void LToplevelRole::configure(Int32 width, Int32 height, UInt32 stateFlags)
{
    imp()->configure(width, height, stateFlags);
}

void LToplevelRole::close() const
{
    XdgShell::RXdgToplevel *res = (XdgShell::RXdgToplevel*)resource();
    res->close();
}

const LRect &LToplevelRole::windowGeometry() const
{
    return xdgSurfaceResource()->imp()->currentWindowGeometry;
}

bool LToplevelRole::startMoveSession(const LEvent &triggerigEvent, const LPoint &movePointPos, Int32 L, Int32 T, Int32 R, Int32 B)
{
    if (moveSession())
        return false;

    imp()->moveSession = new LToplevelMoveSession();
    imp()->moveSession->imp()->toplevel = this;
    imp()->moveSession->imp()->triggeringEvent = triggerigEvent.copy();
    imp()->moveSession->imp()->initMovePointPos = movePointPos;
    imp()->moveSession->imp()->initToplevelPos = surface()->pos();
    imp()->moveSession->imp()->bounds = {L, T, R, B};
    seat()->imp()->moveSessions.push_back(imp()->moveSession);
    imp()->moveSession->imp()->link = std::prev(seat()->imp()->moveSessions.end());
    return true;
}

LToplevelMoveSession *LToplevelRole::moveSession() const
{
    return imp()->moveSession;
}

void LToplevelRole::configure(const LSize &size, UInt32 stateFlags)
{
    imp()->configure(size.w(), size.h(), stateFlags);
}

void LToplevelRole::configure(UInt32 stateFlags)
{
    imp()->configure(imp()->pendingSendConf.size.w(),
                     imp()->pendingSendConf.size.h(),
                     stateFlags);
}

LSize LToplevelRole::calculateResizeSize(const LPoint &cursorPosDelta, const LSize &initialSize, ResizeEdge edge)
{
    LSize newSize = initialSize;
    switch(edge)
    {
        case LToplevelRole::NoEdge:
        {

        }break;
        case LToplevelRole::Bottom:
        {
            newSize.setH(initialSize.h() - cursorPosDelta.y());
        }break;
        case LToplevelRole::Right:
        {
            newSize.setW(initialSize.w() - cursorPosDelta.x());
        }break;
        case LToplevelRole::BottomRight:
        {
            newSize.setH(initialSize.h() - cursorPosDelta.y());
            newSize.setW(initialSize.w() - cursorPosDelta.x());
        }break;
        case LToplevelRole::Top:
        {
            newSize.setH(initialSize.h() + cursorPosDelta.y());
        }break;
        case LToplevelRole::Left:
        {
            newSize.setW(initialSize.w() + cursorPosDelta.x());
        }break;
        case LToplevelRole::TopLeft:
        {
            newSize.setH(initialSize.h() + cursorPosDelta.y());
            newSize.setW(initialSize.w() + cursorPosDelta.x());
        }break;
        case LToplevelRole::BottomLeft:
        {
            newSize.setH(initialSize.h() - cursorPosDelta.y());
            newSize.setW(initialSize.w() + cursorPosDelta.x());
        }break;
        case LToplevelRole::TopRight:
        {
            newSize.setH(initialSize.h() + cursorPosDelta.y());
            newSize.setW(initialSize.w() - cursorPosDelta.x());
        }break;
    }

        return newSize;
}

bool LToplevelRole::startResizeSession(const LEvent &triggeringEvent, ResizeEdge edge, const LPoint &resizePointPos, const LSize &minSize, Int32 L, Int32 T, Int32 R, Int32 B)
{
    if (resizeSession())
    {
        if (resizeSession()->imp()->stopped)
            resizeSession()->imp()->destroy();
        else
            return false;
    }

    imp()->resizeSession = new LToplevelResizeSession();
    imp()->resizeSession->imp()->toplevel = this;
    imp()->resizeSession->imp()->triggeringEvent = triggeringEvent.copy();
    imp()->resizeSession->imp()->toplevelMinSize = minSize;
    imp()->resizeSession->imp()->bounds = {L,T,R,B};
    imp()->resizeSession->imp()->edge = edge;
    imp()->resizeSession->imp()->initToplevelSize = windowGeometry().size();
    imp()->resizeSession->imp()->initResizePointPos = resizePointPos;
    imp()->resizeSession->imp()->currentResizePointPos = resizePointPos;

    if (L != EdgeDisabled && surface()->pos().x() < L)
        surface()->setX(L);

    if (T != EdgeDisabled && surface()->pos().y() < T)
        surface()->setY(T);

    imp()->resizeSession->imp()->initToplevelPos = surface()->pos();

    configure(LToplevelRole::Activated | LToplevelRole::Resizing);
    seat()->imp()->resizeSessions.push_back(imp()->resizeSession);
    imp()->resizeSession->imp()->link = std::prev(seat()->imp()->resizeSessions.end());
    return true;
}

LToplevelResizeSession *LToplevelRole::resizeSession() const
{
    return imp()->resizeSession;
}

static char *trim(char *s)
{
    char *ptr;
    if (!s)
            return NULL;   // handle NULL string
    if (!*s)
            return s;      // handle empty string
    for (ptr = s + strlen(s) - 1; (ptr >= s) && isspace(*ptr); --ptr);
    ptr[1] = '\0';
    return s;
}
void LToplevelRole::LToplevelRolePrivate::setAppId(const char *newAppId)
{
    std::string newStr = trim((char*)newAppId);

    if (appId == newStr)
        return;

    appId = newStr;
    toplevel->appIdChanged();
}

void LToplevelRole::LToplevelRolePrivate::setTitle(const char *newTitle)
{
    std::string newStr = trim((char*)newTitle);

    if (title == newStr)
        return;

    title = newStr;
    toplevel->titleChanged();
}
