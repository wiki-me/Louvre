#include <protocols/Wayland/private/GSeatPrivate.h>
#include <protocols/Wayland/private/RDataDevicePrivate.h>
#include <private/LDNDManagerPrivate.h>
#include <LSurface.h>
#include <LClient.h>

using namespace Louvre::Protocols::Wayland;

void LDNDManager::LDNDManagerPrivate::clear()
{
    focus = nullptr;
    source = nullptr;
    origin = nullptr;
    icon = nullptr;
    srcDataDevice = nullptr;
    dstClient = nullptr;
    matchedMimeType = false;
    eventSource = InputEventSource::Unknown;
    serial = 0;
    touchId = -1;
}

void LDNDManager::LDNDManagerPrivate::sendLeaveEvent(LSurface *surface)
{
    matchedMimeType = false;
    focus = nullptr;

    if (!surface)
        return;

    for (GSeat *gSeat : surface->client()->seatGlobals())
        if (gSeat->dataDeviceResource())
            gSeat->dataDeviceResource()->leave();
}
