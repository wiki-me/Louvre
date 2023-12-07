#ifndef RPRESENTATIONFEEDBACKPRIVATE_H
#define RPRESENTATIONFEEDBACKPRIVATE_H

#include <protocols/PresentationTime/RPresentationFeedback.h>

using namespace Louvre::Protocols::PresentationTime;
using namespace std;

LPRIVATE_CLASS(RPresentationFeedback)
    static void resource_destroy(wl_resource *resource);

    LSurface *lSurface = nullptr;
    list<RPresentationFeedback*>::iterator surfaceLink;
};

#endif // RPRESENTATIONFEEDBACKPRIVATE_H
