#include <private/LCompositorPrivate.h>
#include <private/LSeatPrivate.h>
#include <private/LKeyboardPrivate.h>
#include <LInputBackend.h>
#include <LInputDevice.h>
#include <LPointerMoveEvent.h>
#include <LPointerButtonEvent.h>
#include <LPointerAxisEvent.h>
#include <LLog.h>
#include <unordered_map>
#include <cstring>
#include <libinput.h>
#include <fcntl.h>

using namespace Louvre;

struct DEVICE_FD_ID
{
    int fd;
    int id;
};

struct BACKEND_DATA
{
    libinput *li = nullptr;
    udev *ud = nullptr;
    libinput_interface libinputInterface;
    LSeat *seat;
    std::list<DEVICE_FD_ID> devices;
    std::list<LInputDevice*> inputDevices;


    // Recycled events
    LPointerMoveEvent pointerMoveEvent;
    LPointerButtonEvent pointerButtonEvent;
    LPointerAxisEvent pointerAxisEvent;
};

// Libseat devices
static bool libseatEnabled = false;
static wl_event_source *eventSource = nullptr;

// Event common
static libinput_device *dev;
static libinput_event *ev;
static libinput_event_type eventType;

// Keyboard related
static libinput_event_keyboard *keyEvent;
static libinput_key_state keyState;
static Int32 keyCode;

static libinput_event_pointer *pointerEvent;
static Float32 x = 0.f, y = 0.f;

// For any axis event
static Float32 axisX = 0.f, axisY = 0.f;

// For discrete scroll events
static Float32 discreteX = 0.f, discreteY = 0.f;

// For 120 scroll events
static Float32 d120X = 0.f, d120Y = 0.f;

static LInputDevice *inputDevice;

static Int32 openRestricted(const char *path, int flags, void *data)
{
    BACKEND_DATA *bknd = (BACKEND_DATA*)data;

    if (libseatEnabled)
    {
        DEVICE_FD_ID dev;
        dev.id = bknd->seat->openDevice(path, &dev.fd);

        if (dev.id == -1)
            return -1;

        bknd->devices.push_back(dev);
        return dev.fd;
    }
    else
        return open(path, flags);
}

static void closeRestricted(int fd, void *data)
{
    BACKEND_DATA *bknd = (BACKEND_DATA*)data;

    if (libseatEnabled)
    {
        DEVICE_FD_ID dev = {-1, -1};

        for (std::list<DEVICE_FD_ID>::iterator it = bknd->devices.begin(); it != bknd->devices.end(); it++)
        {
            if ((*it).fd == fd)
            {
                dev = (*it);
                bknd->devices.erase(it);
                break;
            }
        }

        if (dev.fd == -1)
            return;

        bknd->seat->closeDevice(dev.id);
    }

    close(fd);
}

/****************** DEVICE INTERFACE ******************/

static const char *deviceName(const LInputDevice *device)
{
    libinput_device *dev = (libinput_device*)device->backendData();
    return libinput_device_get_name(dev);
}

static UInt32 deviceVendorId(const LInputDevice *device)
{
    libinput_device *dev = (libinput_device*)device->backendData();
    return libinput_device_get_id_vendor(dev);
}

static UInt32 deviceProductId(const LInputDevice *device)
{
    libinput_device *dev = (libinput_device*)device->backendData();
    return libinput_device_get_id_product(dev);
}

static LInputDevice::Interface deviceInterface
{
    .name = &deviceName,
    .vendorId = &deviceVendorId,
    .productId = &deviceProductId
};

Int32 LInputBackend::processInput(int, unsigned int, void *userData)
{
    LSeat *seat = (LSeat*)userData;
    BACKEND_DATA *data = (BACKEND_DATA*)seat->imp()->inputBackendData;

    int ret = libinput_dispatch(data->li);

    if (ret != 0)
    {
        LLog::error("[Libinput Backend] Failed to dispatch libinput %s.", strerror(-ret));
        return 0;
    }

    while ((ev = libinput_get_event(data->li)) != NULL)
    {
        eventType = libinput_event_get_type(ev);

        switch (eventType)
        {
        case LIBINPUT_EVENT_POINTER_MOTION:
            dev = libinput_event_get_device(ev);
            inputDevice = (LInputDevice*)libinput_device_get_user_data(dev);
            pointerEvent = libinput_event_get_pointer_event(ev);
            data->pointerMoveEvent.setDevice(inputDevice);
            data->pointerMoveEvent.setX(libinput_event_pointer_get_dx(pointerEvent));
            data->pointerMoveEvent.setY(libinput_event_pointer_get_dy(pointerEvent));
            data->pointerMoveEvent.setTime(libinput_event_pointer_get_time(pointerEvent));
            data->pointerMoveEvent.notify();
            break;
        case LIBINPUT_EVENT_POINTER_SCROLL_FINGER:
            dev = libinput_event_get_device(ev);
            inputDevice = (LInputDevice*)libinput_device_get_user_data(dev);
            pointerEvent = libinput_event_get_pointer_event(ev);
            data->pointerAxisEvent.setDevice(inputDevice);

            if (libinput_event_pointer_has_axis(pointerEvent, LIBINPUT_POINTER_AXIS_SCROLL_HORIZONTAL))
                data->pointerAxisEvent.setAxisX(libinput_event_pointer_get_scroll_value(pointerEvent, LIBINPUT_POINTER_AXIS_SCROLL_HORIZONTAL));

            if (libinput_event_pointer_has_axis(pointerEvent, LIBINPUT_POINTER_AXIS_SCROLL_VERTICAL))
                data->pointerAxisEvent.setAxisY(libinput_event_pointer_get_scroll_value(pointerEvent, LIBINPUT_POINTER_AXIS_SCROLL_VERTICAL));

            data->pointerAxisEvent.setAxis120X(0.f);
            data->pointerAxisEvent.setAxis120Y(0.f);
            data->pointerAxisEvent.setSource(LPointer::AxisSource::Finger);
            data->pointerAxisEvent.setTime(libinput_event_pointer_get_time(pointerEvent));
            data->pointerAxisEvent.notify();
            break;
        case LIBINPUT_EVENT_POINTER_SCROLL_CONTINUOUS:
            dev = libinput_event_get_device(ev);
            inputDevice = (LInputDevice*)libinput_device_get_user_data(dev);
            pointerEvent = libinput_event_get_pointer_event(ev);
            data->pointerAxisEvent.setDevice(inputDevice);

            if (libinput_event_pointer_has_axis(pointerEvent, LIBINPUT_POINTER_AXIS_SCROLL_HORIZONTAL))
                data->pointerAxisEvent.setAxisX(libinput_event_pointer_get_scroll_value(pointerEvent, LIBINPUT_POINTER_AXIS_SCROLL_HORIZONTAL));

            if (libinput_event_pointer_has_axis(pointerEvent, LIBINPUT_POINTER_AXIS_SCROLL_VERTICAL))
                data->pointerAxisEvent.setAxisY(libinput_event_pointer_get_scroll_value(pointerEvent, LIBINPUT_POINTER_AXIS_SCROLL_VERTICAL));

            data->pointerAxisEvent.setAxis120X(0.f);
            data->pointerAxisEvent.setAxis120Y(0.f);
            data->pointerAxisEvent.setSource(LPointer::AxisSource::Continuous);
            data->pointerAxisEvent.setTime(libinput_event_pointer_get_time(pointerEvent));
            data->pointerAxisEvent.notify();
            break;
        case LIBINPUT_EVENT_POINTER_SCROLL_WHEEL:
            dev = libinput_event_get_device(ev);
            inputDevice = (LInputDevice*)libinput_device_get_user_data(dev);
            pointerEvent = libinput_event_get_pointer_event(ev);
            data->pointerAxisEvent.setDevice(inputDevice);

            if (libinput_event_pointer_has_axis(pointerEvent, LIBINPUT_POINTER_AXIS_SCROLL_HORIZONTAL))
            {
                data->pointerAxisEvent.setAxisX(libinput_event_pointer_get_scroll_value(pointerEvent, LIBINPUT_POINTER_AXIS_SCROLL_HORIZONTAL));
                data->pointerAxisEvent.setAxis120X(libinput_event_pointer_get_scroll_value_v120(pointerEvent, LIBINPUT_POINTER_AXIS_SCROLL_HORIZONTAL));
            }

            if (libinput_event_pointer_has_axis(pointerEvent, LIBINPUT_POINTER_AXIS_SCROLL_VERTICAL))
            {
                data->pointerAxisEvent.setAxisY(libinput_event_pointer_get_scroll_value(pointerEvent, LIBINPUT_POINTER_AXIS_SCROLL_VERTICAL));
                data->pointerAxisEvent.setAxis120Y(libinput_event_pointer_get_scroll_value_v120(pointerEvent, LIBINPUT_POINTER_AXIS_SCROLL_VERTICAL));
            }

            data->pointerAxisEvent.setSource(LPointer::AxisSource::Wheel);
            data->pointerAxisEvent.setTime(libinput_event_pointer_get_time(pointerEvent));
            data->pointerAxisEvent.notify();
            break;
        case LIBINPUT_EVENT_POINTER_BUTTON:
            dev = libinput_event_get_device(ev);
            inputDevice = (LInputDevice*)libinput_device_get_user_data(dev);
            pointerEvent = libinput_event_get_pointer_event(ev);
            data->pointerButtonEvent.setDevice(inputDevice);
            data->pointerButtonEvent.setButton((LPointer::Button)libinput_event_pointer_get_button(pointerEvent));
            data->pointerButtonEvent.setState((LPointer::ButtonState)libinput_event_pointer_get_button_state(pointerEvent));
            data->pointerButtonEvent.setTime(libinput_event_pointer_get_time(pointerEvent));
            data->pointerButtonEvent.notify();
            break;
        case LIBINPUT_EVENT_KEYBOARD_KEY:
            keyEvent = libinput_event_get_keyboard_event(ev);
            keyState = libinput_event_keyboard_get_key_state(keyEvent);
            keyCode = libinput_event_keyboard_get_key(keyEvent);
            seat->keyboard()->imp()->backendKeyEvent(keyCode, (LKeyboard::KeyState)keyState);
            break;
        case LIBINPUT_EVENT_DEVICE_ADDED:
            dev = libinput_event_get_device(ev);
            inputDevice = new LInputDevice(deviceInterface, dev);
            libinput_device_set_user_data(dev, inputDevice);
            data->inputDevices.push_back(inputDevice);
            seat->inputDevicePlugged(inputDevice);
            break;
        case LIBINPUT_EVENT_DEVICE_REMOVED:
            dev = libinput_event_get_device(ev);
            inputDevice = (LInputDevice*)libinput_device_get_user_data(dev);
            data->inputDevices.remove(inputDevice);
            seat->inputDeviceUnplugged(inputDevice);
            delete inputDevice;
            break;
        default:
            break;
        }

        seat->nativeInputEvent(ev);
        libinput_event_destroy(ev);
    }

    return 0;
}

UInt32 LInputBackend::id()
{
    return LInputBackendLibinput;
}

bool LInputBackend::initialize()
{
    int fd;
    LSeat *seat = LCompositor::compositor()->seat();
    libseatEnabled = seat->imp()->initLibseat();

    BACKEND_DATA *data = new BACKEND_DATA;
    data->seat = (LSeat*)seat;
    seat->imp()->inputBackendData = data;
    data->ud = udev_new();

    data->pointerMoveEvent.setIsAbsolute(false);

    if (!data->ud)
        goto fail;

    data->libinputInterface.open_restricted = &openRestricted;
    data->libinputInterface.close_restricted = &closeRestricted;
    data->li = libinput_udev_create_context(&data->libinputInterface, data, data->ud);

    if (!data->li)
        goto fail;

    if (libseatEnabled )
        libinput_udev_assign_seat(data->li, libseat_seat_name(seat->libseatHandle()));
    else
        libinput_udev_assign_seat(data->li, "seat0");

    fd = libinput_get_fd(data->li);
    fcntl(fd, F_SETFD, FD_CLOEXEC);

    eventSource = LCompositor::addFdListener(fd, (LSeat*)seat, &processInput);
    return true;

    fail:
    uninitialize();
    return false;
}

UInt32 LInputBackend::getCapabilities()
{
    return LSeat::InputCapabilities::Pointer | LSeat::InputCapabilities::Keyboard;
}

void *LInputBackend::getContextHandle()
{
    LSeat *seat = LCompositor::compositor()->seat();
    BACKEND_DATA *data = (BACKEND_DATA*)seat->imp()->inputBackendData;
    return data->li;
}

void LInputBackend::suspend()
{
    LSeat *seat = LCompositor::compositor()->seat();
    BACKEND_DATA *data = (BACKEND_DATA*)seat->imp()->inputBackendData;
    LCompositor::removeFdListener(eventSource);
    libinput_suspend(data->li);
}

void LInputBackend::forceUpdate()
{
    LSeat *seat = LCompositor::compositor()->seat();
    processInput(0, 0, (LSeat*)seat);
}

void LInputBackend::resume()
{
    LSeat *seat = LCompositor::compositor()->seat();
    BACKEND_DATA *data = (BACKEND_DATA*)seat->imp()->inputBackendData;

    int fd;

    libinput_dispatch(data->li);

    if (libinput_resume(data->li) == -1)
    {
        LLog::error("[Libinput Backend] Failed to resume libinput.");
        return;
    }

    // Handle queued events during session switching

    forceUpdate();

    /* For some reason libinput crashes if a device is added while suspended.
     * As a temp fix we destroy and recreate the context again. */
    
    if (data->li)
        libinput_unref(data->li);

    if (data->ud)
        udev_unref(data->ud);

    data->ud = udev_new();
    data->libinputInterface.open_restricted = &openRestricted;
    data->libinputInterface.close_restricted = &closeRestricted;
    data->li = libinput_udev_create_context(&data->libinputInterface, data, data->ud);

    if (libseatEnabled )
        libinput_udev_assign_seat(data->li, libseat_seat_name(seat->libseatHandle()));
    else
        libinput_udev_assign_seat(data->li, "seat0");

    fd = libinput_get_fd(data->li);
    fcntl(fd, F_SETFD, FD_CLOEXEC);
    eventSource = LCompositor::addFdListener(fd, (LSeat*)seat, &processInput);
}

void LInputBackend::uninitialize()
{
    LSeat *seat = LCompositor::compositor()->seat();
    BACKEND_DATA *data = (BACKEND_DATA*)seat->imp()->inputBackendData;

    if (!data)
        return;

    if (eventSource)
    {
        LCompositor::removeFdListener(eventSource);
        eventSource = nullptr;
    }

    // Only delete devices, do not notify
    while (!data->inputDevices.empty())
    {
        delete data->inputDevices.back();
        data->inputDevices.pop_back();
    }

    if (data->li)
        libinput_unref(data->li);

    if (data->ud)
        udev_unref(data->ud);

    delete data;
    seat->imp()->inputBackendData = nullptr;
}

std::list<LInputDevice *> *LInputBackend::getDevices()
{
    LSeat *seat = LCompositor::compositor()->seat();
    BACKEND_DATA *data = (BACKEND_DATA*)seat->imp()->inputBackendData;
    return &data->inputDevices;
}

LInputBackendInterface API;

extern "C" LInputBackendInterface *getAPI()
{
    API.id = &LInputBackend::id;
    API.initialize = &LInputBackend::initialize;
    API.uninitialize = &LInputBackend::uninitialize;
    API.getCapabilities = &LInputBackend::getCapabilities;
    API.getContextHandle = &LInputBackend::getContextHandle;
    API.suspend = &LInputBackend::suspend;
    API.forceUpdate = &LInputBackend::forceUpdate;
    API.resume = &LInputBackend::resume;
    API.getDevices = &LInputBackend::getDevices;

    return &API;
}
