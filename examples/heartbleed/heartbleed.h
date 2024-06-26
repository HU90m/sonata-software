#include <stdbool.h>
#include <cdefs.h>

#ifdef __cplusplus
extern "C" {
#endif

void __cheri_libcall putchar(char c);

#ifdef __cplusplus
}
#endif


/*
void __cheri_compartment("heartbleed") entry_point()
{
	auto uart = MMIO_CAPABILITY(OpenTitanUart<>, uart);

	char message[] = "hello there\n";
	if (return_true()) {
		for (char c : message) uart->blocking_write(c);
	}

	while (true) {}
}
*/
