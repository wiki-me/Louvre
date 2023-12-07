#ifndef LCLIENTPRIVATE_H
#define LCLIENTPRIVATE_H

#include <LClient.h>
#include <LDataDevice.h>

using namespace Louvre;
using namespace Louvre::Protocols;

struct LClient::Params
{
    wl_client *client;
};

LPRIVATE_CLASS(LClient)

    wl_client *client = nullptr;
    std::list<LClient*>::iterator compositorLink;
    LDataDevice dataDevice;
    std::list<LSurface*> surfaces;

    // Globals
    std::list<Wayland::GCompositor*> compositorGlobals;
    std::list<Wayland::GOutput*> outputGlobals;
    std::list<Wayland::GSeat*> seatGlobals;
    std::list<Wayland::GSubcompositor*> subcompositorGlobals;
    std::list<XdgShell::GXdgWmBase*> xdgWmBaseGlobals;
    std::list<XdgDecoration::GXdgDecorationManager*> xdgDecorationManagerGlobals;
    std::list<PresentationTime::GPresentation*> presentationTimeGlobals;
    std::list<LinuxDMABuf::GLinuxDMABuf*> linuxDMABufGlobals;
    std::list<PointerGestures::GPointerGestures*> pointerGesturesGlobals;
    std::list<RelativePointer::GRelativePointerManager*> relativePointerManagerGlobals;

    // Singleton Globals
    Wayland::GDataDeviceManager *dataDeviceManagerGlobal = nullptr;
};

#endif // LCLIENTPRIVATE_H
