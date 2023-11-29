#include <protocols/Wayland/private/RDataDevicePrivate.h>
#include <protocols/Wayland/RDataOffer.h>
#include <protocols/Wayland/GSeat.h>
#include <private/LDataDevicePrivate.h>
#include <private/LCompositorPrivate.h>
#include <private/LDataOfferPrivate.h>
#include <private/LDataSourcePrivate.h>
#include <private/LSeatPrivate.h>
#include <private/LDNDManagerPrivate.h>
#include <LTime.h>

using namespace Louvre;

LDataDevice::LDataDevice() : LPRIVATE_INIT_UNIQUE(LDataDevice) {}
LDataDevice::~LDataDevice() {}

LClient *LDataDevice::client() const
{
    return imp()->client;
}

void LDataDevice::sendSelectionEvent()
{
    // Send data device selection first
    if (seat()->dataSelection())
    {
        for (Protocols::Wayland::GSeat *d : client()->seatGlobals())
        {
            if (d->dataDeviceResource())
            {
                Protocols::Wayland::RDataOffer *rDataOffer = new Protocols::Wayland::RDataOffer(d->dataDeviceResource(), 0);
                rDataOffer->dataOffer()->imp()->usedFor = LDataOffer::Selection;
                d->dataDeviceResource()->dataOffer(rDataOffer);

                for (const LDataSource::LSource &s : seat()->dataSelection()->sources())
                    rDataOffer->offer(s.mimeType);

                d->dataDeviceResource()->selection(rDataOffer);
            }
        }
    }
}
