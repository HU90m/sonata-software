// Copyright lowRISC Contributors.
// SPDX-License-Identifier: Apache-2.0

#include "heartbleed.h"

#include <compartment.h>
//#include <debug.hh>
#include <platform-uart.hh>
#include <thread.h>

extern "C" void __cheri_libcall putchar(char ch) {
	auto uart = MMIO_CAPABILITY(OpenTitanUart<>, uart);
	uart->blocking_write(ch);
}


