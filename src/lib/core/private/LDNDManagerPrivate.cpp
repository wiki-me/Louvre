#include <protocols/Wayland/private/GSeatPrivate.h>
#include <protocols/Wayland/private/RDataDevicePrivate.h>
#include <private/LDNDManagerPrivate.h>
#include <LSurface.h>
#include <LClient.h>
#include <LEvent.h>

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

    if (startDragEvent)
    {
        delete startDragEvent;
        startDragEvent = nullptr;
    }
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
