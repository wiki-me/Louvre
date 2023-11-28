#ifndef LINPUTDEVICE_H
#define LINPUTDEVICE_H

#include <LSeat.h>
#include <LObject.h>
#include <libinput.h>

/**
 * @brief Input Device
 *
 * This class represents an input device, offering methods for querying device information and configuring its properties.
 * The complete list of available devices can be accessed through LSeat::inputDevices() or any of the LInputEvent subtypes to identify the originating device.
 *
 * ### Capabilities
 *
 * Devices aren't categorized by type, instead by the events they generate, such as pointer, keyboard, touch events, or a combination of these.
 * To retrieve device capabilities, use the capabilities() method.
 *
 * ### Configuration
 *
 * Configuration methods for device parameters may not be available for all devices. You can ascertain this by checking their return values,
 * which can be one of three outcomes: Success, Unsupported, or Invalid.
 */
class Louvre::LInputDevice : public LObject
{
public:

    /**
     * @brief Enumerates the status of configuration operations.
     *
     * This enumeration defines the possible outcomes of device configuration operations.
     * The values correspond to those provided by the LIBINPUT_CONFIG_STATUS enumeration.
     */
    enum ConfigurationStatus
    {
        /// The configuration operation was successful.
        Success = LIBINPUT_CONFIG_STATUS_SUCCESS,

        /// The configuration operation is not supported on the device.
        Unsupported = LIBINPUT_CONFIG_STATUS_UNSUPPORTED,

        /// The configuration provided is invalid or not allowed.
        Invalid = LIBINPUT_CONFIG_STATUS_INVALID
    };

    struct Interface
    {
        LSeat::InputCapabilitiesFlags (*capabilities)(const LInputDevice *);
        const char * (*name)(const LInputDevice *);
        UInt32 (*vendorId)(const LInputDevice *);
        UInt32 (*productId)(const LInputDevice *);
    };

    /// @cond OMIT
    LInputDevice(const Interface &interface, void *backendData);
    LCLASS_NO_COPY(LInputDevice)
    ~LInputDevice();
    /// @endcond

    /**
     * @brief Get the capabilities of the input device.
     *
     * This method returns the capabilities of the input device, represented as a combination of flags.
     */
    LSeat::InputCapabilitiesFlags capabilities() const;

    /**
     * @brief Get the name of the input device.
     */
    const char *name() const;

    /**
     * @brief Get the product ID of the input device.
     */
    UInt32 productId() const;

    /**
     * @brief Get the vendor ID of the input device.
     */
    UInt32 vendorId() const;

    void *backendData() const;

private:
    friend class LInputBackend;
    Interface m_interface;
    void *m_backendData;
    void notifyPlugged();
    void notifyUnplugged();
};

#endif // LINPUTDEVICE_H
