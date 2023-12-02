#include <private/LSeatPrivate.h>
#include <private/LPointerPrivate.h>
#include <private/LKeyboardPrivate.h>
#include <private/LTouchPrivate.h>
#include <private/LDNDManagerPrivate.h>
#include <private/LCompositorPrivate.h>
#include <private/LOutputPrivate.h>
#include <private/LToplevelRolePrivate.h>

#include <protocols/Wayland/GSeat.h>

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <libudev.h>
#include <libinput.h>

#include <xkbcommon/xkbcommon-compat.h>
#include <xkbcommon/xkbcommon-compose.h>
#include <xkbcommon/xkbcommon-keysyms.h>
#include <xkbcommon/xkbcommon-names.h>
#include <sys/mman.h>

#include <sys/poll.h>
#include <sys/eventfd.h>

#include <LCursor.h>
#include <LToplevelRole.h>
#include <LSurface.h>
#include <LTime.h>
#include <LOutput.h>
#include <LPopupRole.h>
#include <LLog.h>
#include <LInputDevice.h>

using namespace Louvre;

LSeat::LSeat(Params *params) : LPRIVATE_INIT_UNIQUE(LSeat)
{
    L_UNUSED(params);
    compositor()->imp()->seat = this;

    LDNDManager::Params dndManagerParams;
    imp()->dndManager = compositor()->createDNDManagerRequest(&dndManagerParams);

    LPointer::Params pointerParams;
    imp()->pointer = compositor()->createPointerRequest(&pointerParams);

    LKeyboard::Params keyboardParams;
    imp()->keyboard = compositor()->createKeyboardRequest(&keyboardParams);

    LTouch::Params touchParams;
    imp()->touch = compositor()->createTouchRequest(&touchParams);

    imp()->enabled = true;
}

LSeat::~LSeat()
{
    if (imp()->libseatHandle)
    {
        libseat_close_seat(imp()->libseatHandle);
        imp()->libseatHandle = nullptr;
    }}

const std::list<LOutput *> &LSeat::outputs() const
{
    return *compositor()->imp()->graphicBackend->getConnectedOutputs();
}

void *LSeat::graphicBackendContextHandle() const
{
    return compositor()->imp()->graphicBackend->getContextHandle();
}

UInt32 LSeat::graphicBackendId() const
{
    return compositor()->imp()->graphicBackend->id();
}

LSeat::InputCapabilitiesFlags LSeat::inputBackendCapabilities() const
{
    return compositor()->imp()->inputBackend->getCapabilities();
}

const char *LSeat::name() const
{
    if (imp()->libseatHandle)
        return libseat_seat_name(imp()->libseatHandle);

    return "seat0";
}

const std::list<LInputDevice *> &LSeat::inputDevices() const
{
    return *compositor()->imp()->inputBackend->getDevices();
}

void *LSeat::inputBackendContextHandle() const
{
    return compositor()->imp()->inputBackend->getContextHandle();
}

UInt32 LSeat::inputBackendId() const
{
    return compositor()->imp()->inputBackend->id();
}

LSeat::InputCapabilitiesFlags LSeat::inputCapabilities() const
{
    return imp()->capabilities;
}

void LSeat::setInputCapabilities(LSeat::InputCapabilitiesFlags capabilitiesFlags)
{
    if (capabilitiesFlags == imp()->capabilities)
        return;

    imp()->capabilities = capabilitiesFlags;

    for (LClient *c : compositor()->clients())
    {
        for (Wayland::GSeat *s : c->seatGlobals())
            s->capabilities(capabilitiesFlags);
    }
}

LToplevelRole *LSeat::activeToplevel() const
{
    return imp()->activeToplevel;
}

LPointer *LSeat::pointer() const
{
    return imp()->pointer;
}

LKeyboard *LSeat::keyboard() const
{
    return imp()->keyboard;
}

LTouch *LSeat::touch() const
{
    return imp()->touch;
}

LDataSource *LSeat::dataSelection() const
{
    return imp()->dataSelection;
}

void LSeat::startResizingToplevel(LToplevelRole *toplevel, LToplevelRole::ResizeEdge edge, const LPoint &pointerPos, const LSize &minSize, Int32 L, Int32 T, Int32 R, Int32 B)
{
    if (!toplevel)
        return;

    imp()->resizingToplevel = toplevel;

    toplevel->imp()->resizingMinSize = minSize;
    toplevel->imp()->resizingConstraintBounds = LRect(L,T,R,B);
    toplevel->imp()->resizingEdge = edge;
    toplevel->imp()->resizingInitWindowSize = toplevel->windowGeometry().size();
    toplevel->imp()->resizingInitPointerPos = pointerPos;
    toplevel->imp()->resizingCurrentPointerPos = pointerPos;

    if (L != EdgeDisabled && toplevel->surface()->pos().x() < L)
        toplevel->surface()->setX(L);

    if (T != EdgeDisabled && toplevel->surface()->pos().y() < T)
        toplevel->surface()->setY(T);

    toplevel->imp()->resizingInitPos = toplevel->surface()->pos();

    resizingToplevel()->configure(LToplevelRole::Activated | LToplevelRole::Resizing);
}

void LSeat::updateResizingToplevelSize(const LPoint &pointerPos)
{
    if (resizingToplevel())
    {
        resizingToplevel()->imp()->resizingCurrentPointerPos = pointerPos;
        LSize newSize = resizingToplevel()->calculateResizeSize(resizingToplevel()->imp()->resizingInitPointerPos - pointerPos,
                                                                resizingToplevel()->imp()->resizingInitWindowSize,
                                                                resizingToplevel()->imp()->resizingEdge);
        // Con restricciones
        LToplevelRole::ResizeEdge edge =  resizingToplevel()->imp()->resizingEdge;
        LPoint pos = resizingToplevel()->surface()->pos();
        LRect bounds = resizingToplevel()->imp()->resizingConstraintBounds;
        LSize size = resizingToplevel()->windowGeometry().size();

        // Top
        if (bounds.y() != EdgeDisabled && (edge ==  LToplevelRole::Top || edge ==  LToplevelRole::TopLeft || edge ==  LToplevelRole::TopRight))
        {
            if (pos.y() - (newSize.y() - size.y()) < bounds.y())
                newSize.setH(pos.y() + size.h() - bounds.y());
        }
        // Bottom
        else if (bounds.h() != EdgeDisabled && (edge ==  LToplevelRole::Bottom || edge ==  LToplevelRole::BottomLeft || edge ==  LToplevelRole::BottomRight))
        {
            if (pos.y() + newSize.h() > bounds.h())
                newSize.setH(bounds.h() - pos.y());
        }

        // Left
        if ( bounds.x() != EdgeDisabled && (edge ==  LToplevelRole::Left || edge ==  LToplevelRole::TopLeft || edge ==  LToplevelRole::BottomLeft))
        {
            if (pos.x() - (newSize.x() - size.x()) < bounds.x())
                newSize.setW(pos.x() + size.w() - bounds.x());
        }
        // Right
        else if ( bounds.w() != EdgeDisabled && (edge ==  LToplevelRole::Right || edge ==  LToplevelRole::TopRight || edge ==  LToplevelRole::BottomRight))
        {
            if (pos.x() + newSize.w() > bounds.w())
                newSize.setW(bounds.w() - pos.x());
        }

        if (newSize.w() < resizingToplevel()->imp()->resizingMinSize.w())
            newSize.setW(resizingToplevel()->imp()->resizingMinSize.w());

        if (newSize.h() < resizingToplevel()->imp()->resizingMinSize.h())
            newSize.setH(resizingToplevel()->imp()->resizingMinSize.h());

        resizingToplevel()->configure(newSize, LToplevelRole::Activated | LToplevelRole::Resizing);
    }
}

void LSeat::updateResizingToplevelPos()
{
    if (resizingToplevel())
        resizingToplevel()->updateResizingPos();
}

void LSeat::stopResizingToplevel()
{
    if(resizingToplevel())
    {
        updateResizingToplevelSize(cursor()->pos());
        updateResizingToplevelPos();
        resizingToplevel()->configure(0, resizingToplevel()->pendingState() &~ LToplevelRole::Resizing);
        imp()->resizingToplevel = nullptr;
    }
}

LToplevelRole *LSeat::resizingToplevel() const
{
    return imp()->resizingToplevel;
}

void LSeat::startMovingToplevel(LToplevelRole *toplevel, const LPoint &pointerPos, Int32 L, Int32 T, Int32 R, Int32 B)
{
    imp()->movingToplevelConstraintBounds = LRect(L,T,B,R);
    imp()->movingToplevelInitPos = toplevel->surface()->pos();
    imp()->movingToplevelInitPointerPos = pointerPos;
    imp()->movingToplevel = toplevel;
}

void LSeat::updateMovingToplevelPos(const LPoint &pointerPos)
{
    if (movingToplevel())
    {
        LPoint newPos = movingToplevelInitPos() - movingToplevelInitPointerPos() + pointerPos;

        if (imp()->movingToplevelConstraintBounds.w() != EdgeDisabled && newPos.x() > imp()->movingToplevelConstraintBounds.w())
            newPos.setX(imp()->movingToplevelConstraintBounds.w());

        if (imp()->movingToplevelConstraintBounds.x() != EdgeDisabled && newPos.x() < imp()->movingToplevelConstraintBounds.x())
            newPos.setX(imp()->movingToplevelConstraintBounds.x());

        if (imp()->movingToplevelConstraintBounds.h() != EdgeDisabled && newPos.y() > imp()->movingToplevelConstraintBounds.h())
            newPos.setY(imp()->movingToplevelConstraintBounds.h());

        if (imp()->movingToplevelConstraintBounds.y() != EdgeDisabled && newPos.y() < imp()->movingToplevelConstraintBounds.y())
            newPos.setY(imp()->movingToplevelConstraintBounds.y());

        movingToplevel()->surface()->setPos(newPos);
    }
}

void LSeat::stopMovingToplevel()
{
    imp()->movingToplevel = nullptr;
}

LToplevelRole *LSeat::movingToplevel() const
{
    return imp()->movingToplevel;
}

const LPoint &LSeat::movingToplevelInitPos() const
{
    return imp()->movingToplevelInitPos;
}

const LPoint &LSeat::movingToplevelInitPointerPos() const
{
    return imp()->movingToplevelInitPointerPos;
}

LDNDManager *LSeat::dndManager() const
{
    return imp()->dndManager;
}

void LSeat::setTTY(UInt32 tty)
{
    if (imp()->libseatHandle)
        imp()->ttyNumber = tty;
}

Int32 LSeat::openDevice(const char *path, Int32 *fd)
{
    if (!imp()->libseatHandle)
        return -1;

    Int32 id = libseat_open_device(libseatHandle(), path, fd);

    if (id == -1)
        LLog::error("[LSeat::openDevice] Failed to open device %s, id %d, %fd.", path, id, fd);
    else
        fcntl(*fd, F_SETFD, FD_CLOEXEC);

    return id;
}

Int32 LSeat::closeDevice(Int32 id)
{
    if (!imp()->libseatHandle)
        return -1;

    Int32 ret = libseat_close_device(libseatHandle(), id);

    if (ret == -1)
        LLog::error("[LSeat::closeDevice] Failed to close device %d.", id);

    return ret;
}

libseat *LSeat::libseatHandle() const
{
    return imp()->libseatHandle;
}

bool LSeat::enabled() const
{
    return imp()->enabled;
}

LPopupRole *LSeat::topmostPopup() const
{
    for (std::list<LSurface*>::const_reverse_iterator it = compositor()->surfaces().crbegin();
         it != compositor()->surfaces().crend(); it++)
    {
        if ((*it)->mapped() && (*it)->popup())
            return (*it)->popup();
    }

    return nullptr;
}
