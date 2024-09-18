// Copyright lowRISC Contributors.
// SPDX-License-Identifier: Apache-2.0

#include "gpio_access_static.hh"
#include <debug.hh>
#include <thread.h>
#include <token.h>
#include <vector>

using Debug = ConditionalDebug<true, "Led Walk Static">;

/// Thread entry point.
void __cheri_compartment("led_walk_static") start_walking()
{
	Debug::log("hello");

	Sealed<LedHandle> value{STATIC_SEALED_VALUE(led_handle_0)};

	while (true)
		thread_millisecond_wait(500);
}
