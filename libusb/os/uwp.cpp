
#include <winrt/Windows.Devices.Enumeration.h>
#include <winrt/Windows.Devices.Usb.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/base.h>

#include <chrono>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <thread>
extern "C"
{
#include "libusbi.h"
}

#define TODO() throw std::logic_error("Not implemented");

namespace winrtWDU = winrt::Windows::Devices::Usb;
namespace winrtWDE = winrt::Windows::Devices::Enumeration;

struct UwpContext
{
    libusb_context* ctx;
    std::unordered_map<winrt::hstring, uint16_t> fakeAddresses;

    static UwpContext& Get(libusb_context* ctx)
    {
        return *reinterpret_cast<UwpContext*>(usbi_get_context_priv(ctx));
    }
    static void Init(libusb_context* ctx)
    {
        // Inplace constructor
        auto ictx = new(usbi_get_context_priv(ctx)) UwpContext();
        ictx->ctx = ctx;
    }
    static void Exit(libusb_context* ctx)
    {
        // Inplace destructor
        reinterpret_cast<UwpContext*>(usbi_get_context_priv(ctx))->~UwpContext();
    }
};
struct UwpDevice
{
    UwpDevice() = default;

    struct libusb_device* dev{};
    winrtWDU::UsbDevice usbdevice{ nullptr };
    winrtWDE::DeviceInformation info{ nullptr };

    static UwpDevice& Create(UwpContext& ctx, winrtWDU::UsbDevice const& usbdevice, winrtWDE::DeviceInformation const& info)
    {
        auto desc = usbdevice.DeviceDescriptor();
        size_t sessionId = [&]() ->size_t
        {
            auto it = ctx.fakeAddresses.find(info.Id());
            if (it == ctx.fakeAddresses.end())
            {
                auto size = static_cast<uint16_t>(ctx.fakeAddresses.size());
                ctx.fakeAddresses[info.Id()] = size;
                return size;
            }
            return it->second;
        }();
        auto dev = usbi_alloc_device(ctx.ctx, sessionId);
        // Inplace construct
        auto idev = new (usbi_get_device_priv(dev)) UwpDevice();
        idev->dev = dev;
        idev->usbdevice = usbdevice;
        idev->info = info;
        return *idev;
    }

    static void Destroy(struct libusb_device* dev)
    {
        // Inplace destruct
        (reinterpret_cast<UwpDevice*>(usbi_get_device_priv(dev)))->~UwpDevice();
    }

};
struct UwpDeviceHandle
{
};
struct UwpTransfer
{
};


void usbi_get_monotonic_time(struct timespec* tp)
{
    auto now = std::chrono::system_clock::now();
    auto secs = std::chrono::time_point_cast<std::chrono::seconds>(now);
    auto ns = std::chrono::time_point_cast<std::chrono::nanoseconds>(now) - std::chrono::time_point_cast<std::chrono::nanoseconds>(secs);
    tp->tv_sec = secs.time_since_epoch().count();
    tp->tv_nsec = ns.count();
}

static int uwp_init(struct libusb_context* ctx)
{
    UwpContext::Init(ctx);
    return 0;
}
static void uwp_exit(struct libusb_context* ctx)
{
    UwpContext::Exit(ctx);
}

static int uwp_get_device_list(struct libusb_context* ctx, struct discovered_devs** discdevs)
{
    auto aqs = L"System.Devices.InterfaceClassGuid:=\"{A5DCBF10-6530-11D2-901F-00C04FB951ED}\" AND "
        L"System.Devices.InterfaceEnabled:=System.StructuredQueryType.Boolean#False";
    for (auto info : winrtWDE::DeviceInformation::FindAllAsync(aqs).get())
    {
        try
        {
            auto usbdevice = winrtWDU::UsbDevice::FromIdAsync(info.Id()).get();
            if (!usbdevice) continue;

            *discdevs = discovered_devs_append(*discdevs, UwpDevice::Create(UwpContext::Get(ctx), usbdevice, info).dev);
            if (discdevs == nullptr)
            {
                throw std::runtime_error("Cannot Create a libusb device");
            }
        }
        catch (...)
        {
        }
    }
    return 0;
}
static void uwp_destroy_device(struct libusb_device* dev)
{
    UwpDevice::Destroy(dev);
}

static int uwp_open(struct libusb_device_handle* dev_handle)
{
    TODO();
}

static void uwp_close(struct libusb_device_handle* dev_handle)
{
    TODO();
}
static int uwp_get_active_config_descriptor(struct libusb_device* device, void* buffer, size_t len)
{
    TODO();
}
static int uwp_get_config_descriptor(struct libusb_device* device, uint8_t config_index, void* buffer, size_t len)
{
    TODO();
}
static int uwp_get_config_descriptor_by_value(struct libusb_device* device, uint8_t bConfigurationValue, void** buffer)
{
    TODO();
}
static int uwp_get_configuration(struct libusb_device_handle* dev_handle, uint8_t* config)
{
    TODO();
}
static int uwp_set_configuration(struct libusb_device_handle* dev_handle, uint8_t config)
{
    TODO();
}
static int uwp_claim_interface(struct libusb_device_handle* dev_handle, uint8_t interface_number)
{
    TODO();
}
static int uwp_release_interface(struct libusb_device_handle* dev_handle, uint8_t interface_number)
{
    TODO();
}
static int uwp_set_interface_altsetting(struct libusb_device_handle* dev_handle, uint8_t interface_number, uint8_t altsetting)
{
    TODO();
}
static int uwp_clear_halt(struct libusb_device_handle* dev_handle, unsigned char endpoint)
{
    TODO();
}
static int uwp_reset_device(struct libusb_device_handle* dev_handle)
{
    TODO();
}

static int uwp_submit_transfer(struct usbi_transfer* itransfer)
{
    TODO();
}
static int uwp_cancel_transfer(struct usbi_transfer* itransfer)
{
    TODO();
}
static void uwp_clear_transfer_priv(struct usbi_transfer* itransfer)
{
    TODO();
}
static int uwp_set_option(struct libusb_context* ctx, enum libusb_option option, va_list ap)
{
    TODO();
}

static int uwp_set_configuration(struct libusb_device_handle* dev_handle, int config)
{
    TODO();
}

static int uwp_handle_transfer_completion(struct usbi_transfer* itransfer)
{
    TODO();
}

const struct usbi_os_backend usbi_backend = {
    "uwp",
    USBI_CAP_HAS_HID_ACCESS,
    uwp_init,
    uwp_exit,
    uwp_set_option,
    uwp_get_device_list,
    NULL, /* hotplug_poll */
    NULL, /* wrap_sys_device */
    uwp_open,
    uwp_close,
    uwp_get_active_config_descriptor,
    uwp_get_config_descriptor,
    uwp_get_config_descriptor_by_value,
    uwp_get_configuration,
    uwp_set_configuration,
    uwp_claim_interface,
    uwp_release_interface,
    uwp_set_interface_altsetting,
    uwp_clear_halt,
    uwp_reset_device,
    NULL, /* alloc_streams */
    NULL, /* free_streams */
    NULL, /* dev_mem_alloc */
    NULL, /* dev_mem_free */
    NULL, /* kernel_driver_active */
    NULL, /* detach_kernel_driver */
    NULL, /* attach_kernel_driver */
    uwp_destroy_device,
    uwp_submit_transfer,
    uwp_cancel_transfer,
    NULL, /* clear_transfer_priv */
    NULL, /* handle_events */
    uwp_handle_transfer_completion,
    sizeof(UwpContext),
    sizeof(UwpDevice),
    sizeof(UwpDeviceHandle),
    sizeof(UwpTransfer),
};
