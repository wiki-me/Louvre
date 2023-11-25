#include <private/LCompositorPrivate.h>
#include <LInputEvent.h>
#include <LTime.h>

using namespace Louvre;

LInputEvent::LInputEvent()
{
    m_time = LTime::ms();
    m_device = &compositor()->imp()->fakeDevice;

}
LInputEvent::~LInputEvent() {}
