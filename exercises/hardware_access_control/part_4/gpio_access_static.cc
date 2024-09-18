// Copyright lowRISC Contributors.
// SPDX-License-Identifier: Apache-2.0

#include "gpio_access_static.hh"
#include <platform-gpio.hh>
#include <timeout.hh>
#include <token.h>

/// The number of LEDs available.
static constexpr uint8_t NumLeds = 8;
/// A mask of the LEDs that have been aquired.
static uint8_t ledTaken = 0x0;

/**
 * Get a token key for use sealing LedHandles.
 */
static auto key()
{
	static auto key = STATIC_SEALING_TYPE(SealingType);
	return key;
}

/**
 * Get a pointer to the GPIO MMIO region.
 */
static auto gpio()
{
	static auto gpio = MMIO_CAPABILITY(SonataGPIO, gpio);
	return gpio;
}

/**
 * Unseal a handle with our led token key.
 */
static auto unseal_handle(LedHandle *handle) -> std::optional<LedHandle *>
{
	const auto Unsealed = token_unseal(key(), Sealed<LedHandle>{handle});
	if (nullptr == Unsealed)
	{
		return {};
	}
	return Unsealed;
}

/**
 * Toggle the LED of the given handle.
 */
bool toggle_led(LedHandle *handle)
{
	if (auto unsealedHandle = unseal_handle(handle))
	{
		gpio()->led_toggle(unsealedHandle.value()->index);
		return true;
	}

	return false;
};
