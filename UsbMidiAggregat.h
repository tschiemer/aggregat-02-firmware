/**
  * aggregat controller 02: MIDI based servo controller
  * Copyright (C) 2020  Institute for Computer Music and Sound Technology
  * (ICST), ZHdK Zurich, <https://www.zhdk.ch/forschung/icst>
  * 
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU Lesser General Public
  * License as published by the Free Software Foundation; either
  * version 3 of the License, or (at your option) any later version.
  * 
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  * Lesser General Public License for more details.
  * 
  * You should have received a copy of the GNU Lesser General Public License
  * along with this program; if not, write to the Free Software Foundation,
  * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
  */

#ifndef USBMIDI_AGGREGAT_H
#define USBMIDI_AGGREGAT_H

#include "USBMIDI.h"
#include "PinNames.h"
#include "InterruptIn.h"

class UsbMidiAggregat : public USBMIDI {

    public:

        UsbMidiAggregat(PinName power_pin, bool connect_blocking=true, uint16_t vendor_id=0x0700, uint16_t product_id=0x0101, uint16_t product_release=0x0001);
        UsbMidiAggregat(PinName power_pin, USBPhy *phy, uint16_t vendor_id, uint16_t product_id, uint16_t product_release);

        bool connected();

        bool just_reconnected();

    protected:

        void init_vbus();

        mbed::InterruptIn vbus;

        bool vbus_flag;

        const uint8_t *string_iproduct_desc();

        const uint8_t *string_iinterface_desc();

};

#endif //USBMIDI_AGGREGAT_H