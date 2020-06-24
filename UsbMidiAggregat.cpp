#include "UsbMidiAggregat.h"

UsbMidiAggregat::UsbMidiAggregat(bool connect_blocking, uint16_t vendor_id, uint16_t product_id, uint16_t product_release)
    : USBMIDI(connect_blocking, vendor_id, product_id, product_release)
{

}

UsbMidiAggregat::UsbMidiAggregat(USBPhy *phy, uint16_t vendor_id, uint16_t product_id, uint16_t product_release)
    : USBMIDI(phy, vendor_id, product_id, product_release)
{

}

const uint8_t *UsbMidiAggregat::string_iinterface_desc()
{
    static const uint8_t string_iinterface_descriptor[] = {
        0x0c,                           //bLength
        STRING_DESCRIPTOR,              //bDescriptorType 0x03
        'A', 0, 'u', 0, 'd', 0, 'i', 0, 'o', 0 //bString iInterface - Audio
    };
    return string_iinterface_descriptor;
}

const uint8_t *UsbMidiAggregat::string_iproduct_desc()
{
    static const uint8_t string_iproduct_descriptor[] = {
        42,                                                       //bLength
        STRING_DESCRIPTOR,                                          //bDescriptorType 0x03
        'A', 0, 'g', 0, 'g', 0, 'r', 0, 'e', 0, 'g', 0, 'a', 0, 't', 0, '-', 0, '0', 0, '2', 0 //bString iProduct - Aggregat-02
    };
    return string_iproduct_descriptor;
}