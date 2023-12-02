#include <private/LCompositorPrivate.h>
#include <LInputEvent.h>
#include <LTime.h>

using namespace Louvre;

void LInputEvent::setDevice(LInputDevice *device)
{
    if (device)
        m_device = device;
    else
        m_device = &compositor()->imp()->fakeDevice;
}

LInputEvent::LInputEvent(Type type, Subtype subtype, UInt32 serial, UInt32 time, LInputDevice *device) :
    LEvent(type, subtype, serial, time)
{
    setDevice(device);
}
