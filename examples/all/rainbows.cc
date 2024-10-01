// Copyright lowRISC Contributors.
// SPDX-License-Identifier: Apache-2.0

#include <compartment.h>
#include <ctype.h>
#include <platform-rgbctrl.hh>
#include <thread.h>

[[noreturn]] void __cheri_compartment("rainbows") led_there_be_light()
{
	auto rgbled = MMIO_CAPABILITY(SonataRgbLedController, rgbled);

	constexpr uint8_t MaxLight = 100;

	bool reverse = false;
	uint8_t prox = 0;
	while (true) {
		rgbled->rgb(SonataRgbLed::Led0, prox, MaxLight - prox, 0);
		rgbled->rgb(SonataRgbLed::Led1, 0, MaxLight - prox, prox);
		rgbled->update();
		thread_millisecond_wait(50);

		prox = reverse ? --prox : ++prox;
		if (prox > MaxLight) {
			reverse = true;
		}
	}
}
