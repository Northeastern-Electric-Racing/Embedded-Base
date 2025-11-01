#include "serial.h"

// TLDR: Serial printfs a line with the format: mtr/<Title>/<Data Label>/[Data]
void serial_monitor(const char *title, const char *data_label,
		    const char *format, ...)
{
	printf("\r\n");

	// Print the tag first
	printf("mtr/%s/%s/", title, data_label);

	// Now handle the rest of the format string
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
	printf("\r\n");
}

// TLDR: Serial printfs a line with the format: gph/<Title>/<Data Label>/[HAL_GetTick()]/[Data]
void serial_graph(const char *title, const char *data_label, const char *format,
		  ...)
{
	printf("\r\n");

	// Print the tag first
	printf("gph/%s/%s/%lu/", title, data_label, HAL_GetTick());

	// Now handle the rest of the format string
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
	printf("\r\n");
}