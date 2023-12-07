#include <protocols/PresentationTime/private/RPresentationFeedbackPrivate.h>
#include <protocols/PresentationTime/presentation-time.h>

/* Currently has no interface (no requests) */

void RPresentationFeedback::RPresentationFeedbackPrivate::resource_destroy(wl_resource *resource)
{
    RPresentationFeedback *rPresentationFeedback = (RPresentationFeedback*)wl_resource_get_user_data(resource);
    delete rPresentationFeedback;
}
