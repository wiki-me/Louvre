#include <protocols/Wayland/private/RDataOfferPrivate.h>
#include <protocols/Wayland/private/RDataDevicePrivate.h>
#include <protocols/Wayland/GSeat.h>
#include <protocols/Wayland/RDataDevice.h>
#include <protocols/Wayland/RDataSource.h>
#include <private/LDNDManagerPrivate.h>
#include <private/LDataOfferPrivate.h>
#include <private/LDataDevicePrivate.h>
#include <private/LSurfacePrivate.h>
#include <private/LCompositorPrivate.h>
#include <LDNDIconRole.h>
#include <LClient.h>
#include <LDataSource.h>
#include <LSeat.h>
#include <LPointer.h>
#include <LTimer.h>
#include <LSurface.h>

using namespace Louvre;
using namespace Louvre::Protocols::Wayland;

LDNDManager::LDNDManager(Params *params) : LPRIVATE_INIT_UNIQUE(LDNDManager)
{
    L_UNUSED(params);
}

LDNDManager::~LDNDManager()
{
    if (startDragEvent())
    {
        delete imp()->startDragEvent;
        imp()->startDragEvent = nullptr;
    }
}

void LDNDManager::setFocus(LSurface *surface, const LPointF &localPos)
{
    if (!surface)
    {
        imp()->sendLeaveEvent(focus());
        return;
    }

    if (surface == focus() || seat()->dndManager()->imp()->dropped)
        return;
    else
    {
        imp()->sendLeaveEvent(focus());
        imp()->focus = surface;
    }

    Float24 x = wl_fixed_from_double(localPos.x());
    Float24 y = wl_fixed_from_double(localPos.y());

    if (source())
    {
        UInt32 serial = LTime::nextSerial();

        for (GSeat *gSeat : focus()->client()->seatGlobals())
        {
            if (gSeat->dataDeviceResource())
            {
                RDataOffer *rDataOffer = new RDataOffer(gSeat->dataDeviceResource(), 0);

                rDataOffer->dataOffer()->imp()->usedFor = LDataOffer::DND;
                gSeat->dataDeviceResource()->imp()->dataOffered = rDataOffer->dataOffer();
                gSeat->dataDeviceResource()->dataOffer(rDataOffer);

                for (const LDataSource::LSource &s : source()->sources())
                    rDataOffer->offer(s.mimeType);

                gSeat->dataDeviceResource()->imp()->serials.enter = serial;
                gSeat->dataDeviceResource()->enter(serial,
                                               surface->surfaceResource(),
                                               x,
                                               y,
                                               rDataOffer);

                rDataOffer->sourceActions(source()->dndActions());
            }
        }
    }
    // If source is NULL, enter, leave and motion events are sent only to the client that
    // initiated the drag and the client is expected to handle the data passing internally
    else if (origin() == focus())
    {
        UInt32 serial = LTime::nextSerial();

        for (GSeat *gSeat : focus()->client()->seatGlobals())
        {
            if (gSeat->dataDeviceResource())
            {
                gSeat->dataDeviceResource()->imp()->serials.enter = serial;
                gSeat->dataDeviceResource()->enter(
                    serial,
                    surface->surfaceResource(),
                    x,
                    y,
                    NULL);
            }
        }
    }
}

void LDNDManager::sendMoveEvent(const LPointF &localPos, UInt32 ms)
{
    if (!focus() || seat()->dndManager()->imp()->dropped)
        return;

    Float24 x = wl_fixed_from_double(localPos.x());
    Float24 y = wl_fixed_from_double(localPos.y());

    for (GSeat *gSeat : focus()->client()->seatGlobals())
        if (gSeat->dataDeviceResource())
            gSeat->dataDeviceResource()->motion(ms, x, y);
}

const LEvent *LDNDManager::startDragEvent() const
{
    return imp()->startDragEvent;
}

LDNDIconRole *LDNDManager::icon() const
{
    return imp()->icon;
}

LSurface *LDNDManager::origin() const
{
    return imp()->origin;
}

LSurface *LDNDManager::focus() const
{
    return imp()->focus;
}

LDataSource *LDNDManager::source() const
{
    return imp()->source;
}

Wayland::RDataDevice *LDNDManager::srcDataDevice() const
{
    return imp()->srcDataDevice;
}

LClient *LDNDManager::dstClient() const
{
    return imp()->dstClient;
}

bool LDNDManager::dragging() const
{
    return imp()->origin != nullptr;
}

void LDNDManager::cancel()
{
    imp()->sendLeaveEvent(focus());

    if (source())
    {
        source()->dataSourceResource()->dndFinished();
        source()->dataSourceResource()->cancelled();
    }

    imp()->clear();
    cancelled();
}

void LDNDManager::drop()
{
    if (dragging() && !focus())
    {
        cancel();
        return;
    }

    if (dragging() && !imp()->dropped)
    {
        imp()->dropped = true;

        LTimer::oneShot(100, [this](LTimer *)
        {
            if (source() && imp()->dropped)
                cancel();
        });

        compositor()->imp()->unlockPoll();

        if (icon() && icon()->surface())
            icon()->surface()->imp()->setMapped(false);

        if (imp()->focus)
        {
            for (Wayland::GSeat *s : imp()->focus->client()->seatGlobals())
            {
                if (s->dataDeviceResource())
                {
                    if (!imp()->matchedMimeType && s->dataDeviceResource()->version() >= 3)
                    {
                        cancel();
                        return;
                    }

                    s->dataDeviceResource()->drop();
                }
            }

            if (source())
                source()->dataSourceResource()->dndDropPerformed();

            LSurface *focus = seat()->pointer()->focus();
            seat()->pointer()->setFocus(nullptr);
            seat()->pointer()->setFocus(focus);
        }
        else
        {
            if (source())
                source()->dataSourceResource()->dndDropPerformed();

            cancel();
        }
    }
}

// Since 3

LDNDManager::Action LDNDManager::preferredAction() const
{
    return imp()->preferredAction;
}

void LDNDManager::setPreferredAction(LDNDManager::Action action)
{
    if (imp()->preferredAction == action)
        return;

    imp()->preferredAction = action;

    if (focus())
        for (Wayland::GSeat *s : focus()->client()->seatGlobals())
            if (s->dataDeviceResource() && s->dataDeviceResource()->dataOffered())
                s->dataDeviceResource()->dataOffered()->imp()->updateDNDAction();
}
