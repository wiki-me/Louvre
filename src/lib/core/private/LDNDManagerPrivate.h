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
    LEvent *startDragEvent                                      = nullptr;
    LDNDIconRole *icon                                          = nullptr;
    LSurface *origin                                            = nullptr;
    LSurface *focus                                             = nullptr;
    LDataSource *source                                         = nullptr;
    LClient *dstClient                                          = nullptr;
    Protocols::Wayland::RDataDevice *srcDataDevice              = nullptr;

    bool dropped                                                = false;
    bool matchedMimeType                                        = false;

    void sendLeaveEvent(LSurface *surface);
};

#endif // LDNDMANAGERPRIVATE_H
