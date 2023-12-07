#ifndef GPRESENTATIONPRIVATE_H
#define GPRESENTATIONPRIVATE_H

#include <protocols/PresentationTime/GPresentation.h>
#include <protocols/PresentationTime/presentation-time.h>

using namespace Louvre::Protocols::PresentationTime;
using namespace std;

LPRIVATE_CLASS(GPresentation)
    static void bind(wl_client *client, void *data, UInt32 version, UInt32 id);
    static void resource_destroy(wl_resource *resource);
    static void destroy(wl_client *client, wl_resource *resource);
    static void feedback(wl_client *client, wl_resource *resource, wl_resource *surface, UInt32 id);

    list<GPresentation*>::iterator clientLink;
};

#endif // GPRESENTATIONPRIVATE_H
