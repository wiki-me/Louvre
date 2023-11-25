#ifndef RCALLBACK_H
#define RCALLBACK_H

#include <LResource.h>

class Louvre::Protocols::Wayland::RCallback : public LResource
{
public:
    RCallback(wl_client *client, UInt32 id, std::list<RCallback*>*list);
    LCLASS_NO_COPY(RCallback)
    ~RCallback();

    bool commited = false;
    bool done(UInt32 data);

    LPRIVATE_IMP_UNIQUE(RCallback)
};

#endif // RCALLBACK_H
