// Copyright lowRISC Contributors.
// SPDX-License-Identifier: Apache-2.0

#include "heartbleed.h"

#include "../../libraries/lcd.hh"

#include <string.h>
#include <stdbool.h>

#include <debug.hh>
#include <cheri.hh>


#define CONTENT_TYPE_HEART_BEAT 0x18
#define MESSAGE_TYPE_REQUEST 0x01
#define MESSAGE_TYPE_RESPONSE 0x02
#define PAD_LEN 16
#define GOOD_RECORD_LEN 26
#define BAD_RECORD_LEN 8

const struct {
  uint8_t goodRecord[GOOD_RECORD_LEN];
  uint8_t badRecord[BAD_RECORD_LEN];
  char secrets[23];
} Heap = {
  .goodRecord = {
    CONTENT_TYPE_HEART_BEAT, // Content Type
    0x03, 0x03,              // TLS Version (1.2)
    0x00,   21,              // Record Length
    MESSAGE_TYPE_REQUEST,    // Heartbeat Message Type
    0x00, 0x02,              // Payload Length
    'a', 'b',                // Payload
    'P', 'P', 'P', 'P',      // 16 bytes of padding
    'P', 'P', 'P', 'P',
    'P', 'P', 'P', 'P',
    'P', 'P', 'P', 'P',
  },
  .badRecord = {
    CONTENT_TYPE_HEART_BEAT, // Content Type
    0x03, 0x03,              // TLS Version (1.2)
    0x00, 0x03,              // Record Length
    MESSAGE_TYPE_REQUEST,    // Heartbeat Message Type
    0x00,   23               // Payload length
  },
  .secrets = "Secrets! More secrets."
};

static uint8_t responseBuffer[128];

uint16_t into_uint16(const uint8_t in[2]) {
  return (static_cast<uint16_t>(in[0]) << 8) | in[1];
}

void from_uint16(uint8_t out[2], uint16_t in) {
  out[0] = in >> 8;
  out[1] = in & 0xFF;
}

void get_response_protocol_message(uint8_t response[], const uint8_t request[]) {
  uint16_t payloadLen = into_uint16(request + 6);
  uint16_t recordLen = 1 + 2 + payloadLen + PAD_LEN;

  // Content Type, TLS Version, and record length
  *response++ = CONTENT_TYPE_HEART_BEAT;
  *response++ = 0x03;
  *response++ = 0x03;
  from_uint16(response, recordLen);
  response += 2;

  // Protocol Message starts here
  *response++ = MESSAGE_TYPE_RESPONSE;
  from_uint16(response, payloadLen);
  response += 2;
  memcpy(response, request + 8, payloadLen);
  response += payloadLen;

  // Response Padding
  for (int i = 0; i < PAD_LEN; ++i) {
    *response++ = 'p';
  };
}

void print_payload(uint8_t response[]) {
  uint16_t payloadLen = into_uint16(response + 6);
  for (int i = 0; i < payloadLen; ++i) {
    putchar(response[8 + i]);
  }
  putchar('\n');
};

using namespace sonata::lcd;
SonataLcd lcd;

void lcd_bad() {
	//auto screen   = Rect::from_point_and_size(Point::ORIGIN, lcd.resolution());
	//auto logoRect = screen.centered_subrect({105, 80});
	lcd.draw_str({1, 1}, "Hello world!", Color::White, Color::Black);
	//return lcd;
}


using Debug = ConditionalDebug<true, "HeartBleed">;
using CHERI::Capability;

void __cheri_compartment("heartbleed") entry_point()
{
	Debug::log("rrr");
	lcd = SonataLcd();
	Debug::log("ggg");
	lcd.draw_str({1, 1}, "Hello there!", Color::White, Color::Black);
	Debug::log("wizz");

	auto goodRecord = CHERI::Capability<const uint8_t>(Heap.goodRecord);
	goodRecord.bounds() = GOOD_RECORD_LEN;
	auto badRecord = CHERI::Capability<const uint8_t>(Heap.badRecord);
	badRecord.bounds() = BAD_RECORD_LEN;

	Debug::log("{}", Heap.goodRecord);
	Debug::log("{}", Heap.badRecord);
	Debug::log("{}", badRecord);

	get_response_protocol_message(responseBuffer, goodRecord);
	print_payload(responseBuffer);

	get_response_protocol_message(responseBuffer, badRecord);
	print_payload(responseBuffer);

	while (true) {
		asm volatile("wfi");
	};
}

//extern "C" ErrorRecoveryBehaviour
//compartment_error_handler(ErrorState *frame, size_t mcause, size_t mtval)
//{
//	auto [exceptionCode, registerNumber] = CHERI::extract_cheri_mtval(mtval);
//	if (exceptionCode != CHERI::CauseCode::BoundsViolation)
//	{
//		Debug::log("Encountered an unexpected CHERI violation.");
//		return ErrorRecoveryBehaviour::ForceUnwind;
//	}
//
//	Debug::log("CHERI Bounds violation caught.");
//	Debug::log("{}", exceptionCode);
//	//auto lcd = lcd_bad();
//	//lcd_bad();
//
//	while (true) {
//		asm volatile("wfi");
//	};
//}
