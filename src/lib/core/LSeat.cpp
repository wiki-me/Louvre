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

const std::list<LToplevelResizeSession *> &LSeat::resizeSessions() const
{
    return imp()->resizeSessions;
}

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
    }
}

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

        if (imp()->movingToplevelConstraintBounds.w() != LToplevelRole::EdgeDisabled && newPos.x() > imp()->movingToplevelConstraintBounds.w())
            newPos.setX(imp()->movingToplevelConstraintBounds.w());

        if (imp()->movingToplevelConstraintBounds.x() != LToplevelRole::EdgeDisabled && newPos.x() < imp()->movingToplevelConstraintBounds.x())
            newPos.setX(imp()->movingToplevelConstraintBounds.x());

        if (imp()->movingToplevelConstraintBounds.h() != LToplevelRole::EdgeDisabled && newPos.y() > imp()->movingToplevelConstraintBounds.h())
            newPos.setY(imp()->movingToplevelConstraintBounds.h());

        if (imp()->movingToplevelConstraintBounds.y() != LToplevelRole::EdgeDisabled && newPos.y() < imp()->movingToplevelConstraintBounds.y())
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
