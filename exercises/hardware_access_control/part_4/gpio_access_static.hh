// Copyright lowRISC Contributors.
// SPDX-License-Identifier: Apache-2.0

#include <compartment.h>
#include <optional>

/**
 * A handle showing ownership of the LED at the held index.
 */
struct LedHandle
{
	uint8_t index;
};

DECLARE_AND_DEFINE_STATIC_SEALED_VALUE(LedHandle,
                                       gpio_access_static,
                                       LedHandleType,
                                       led_handle_0,
                                       0);

__cheri_compartment("gpio_access_static") bool toggle_led(LedHandle *);
