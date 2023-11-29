#ifndef LDNDMANAGERPRIVATE_H
#define LDNDMANAGERPRIVATE_H

#include <LDNDManager.h>

using namespace Louvre;

struct LDNDManager::Params
{
    /* Add here any required constructor param */
};

LPRIVATE_CLASS(LDNDManager)

    void clear();

    // Since 3
    Action preferredAction                                      = NoAction;
    InputEventSource eventSource                                = InputEventSource::Unknown;
    UInt32 serial                                               = 0;
    Int32 touchId                                               = -1;
    LDNDIconRole *icon                                          = nullptr;
    LSurface *origin                                            = nullptr;
    LSurface *focus                                             = nullptr;
    LDataSource *source                                         = nullptr;
    LClient *dstClient                                          = nullptr;
    Protocols::Wayland::RDataDevice *srcDataDevice              = nullptr;

    bool dropped                                                = false;
    bool matchedMimeType                                        = false;
    //UInt32 destDidNotRequestReceive                             = 0;


    void sendLeaveEvent(LSurface *surface);
};

#endif // LDNDMANAGERPRIVATE_H
