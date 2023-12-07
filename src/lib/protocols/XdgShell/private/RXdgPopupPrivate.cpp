#include <protocols/XdgShell/private/RXdgPopupPrivate.h>
#include <protocols/XdgShell/private/RXdgSurfacePrivate.h>
#include <protocols/XdgShell/private/RXdgPositionerPrivate.h>
#include <protocols/Wayland/private/GSeatPrivate.h>
#include <protocols/Wayland/RPointer.h>
#include <protocols/Wayland/RKeyboard.h>
#include <protocols/XdgShell/xdg-shell.h>
#include <private/LPopupRolePrivate.h>
#include <private/LSurfacePrivate.h>
#include <private/LPositionerPrivate.h>
#include <LCompositor.h>

void RXdgPopup::RXdgPopupPrivate::destroy_resource(wl_resource *resource)
{
    RXdgPopup *rXdgPopup = (RXdgPopup*)wl_resource_get_user_data(resource);
    delete rXdgPopup;
}

void RXdgPopup::RXdgPopupPrivate::destroy(wl_client *client, wl_resource *resource)
{
    L_UNUSED(client);

    RXdgPopup *rXdgPopup = (RXdgPopup*)wl_resource_get_user_data(resource);

    if (rXdgPopup->popupRole()->surface())
    {
        for (LSurface *child : rXdgPopup->popupRole()->surface()->children())
        {
            if (child->popup() && child->mapped())
            {
                wl_resource_post_error(
                    rXdgPopup->xdgSurfaceResource()->resource(),
                    XDG_WM_BASE_ERROR_NOT_THE_TOPMOST_POPUP,
                    "The client tried to map or destroy a non-topmost popup.");
            }
        }

        if (rXdgPopup->popupRole()->surface()->parent() && rXdgPopup->popupRole()->surface()->parent()->popup())
            rXdgPopup->popupRole()->surface()->imp()->setKeyboardGrabToParent();
    }

    wl_resource_destroy(resource);
}

void RXdgPopup::RXdgPopupPrivate::grab(wl_client *client, wl_resource *resource, wl_resource *seat, UInt32 serial)
{
    L_UNUSED(client);
    L_UNUSED(seat);

    RXdgPopup *rXdgPopup = (RXdgPopup*)wl_resource_get_user_data(resource);
    Wayland::GSeat *gSeat = (Wayland::GSeat*)wl_resource_get_user_data(seat);
    LEvent *triggeringEvent = gSeat->findSerialEventMatch(serial);

    /* TODO: Pass event to the request */
    if (triggeringEvent)
    {
        rXdgPopup->popupRole()->grabSeatRequest(gSeat);

        if (compositor()->seat()->keyboard()->grabbingSurface() != rXdgPopup->popupRole()->surface())
            rXdgPopup->popupRole()->dismiss();
    }
    else
        rXdgPopup->popupRole()->dismiss();
}

#if LOUVRE_XDG_WM_BASE_VERSION >= 3
void RXdgPopup::RXdgPopupPrivate::reposition(wl_client *client, wl_resource *resource, wl_resource *positioner, UInt32 token)
{
    L_UNUSED(client);
    RXdgPopup *rXdgPopup = (RXdgPopup*)wl_resource_get_user_data(resource);
    RXdgPositioner *rXdgPositioner = (RXdgPositioner*)wl_resource_get_user_data(positioner);
    rXdgPopup->popupRole()->imp()->positioner.imp()->data = rXdgPositioner->positioner().imp()->data;
    rXdgPopup->popupRole()->repositionRequest(token);
}
#endif
