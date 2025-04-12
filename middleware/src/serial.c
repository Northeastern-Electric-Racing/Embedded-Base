#include "serial.h"

void serial_monitor(const char *title, const char *data_label,
		    const char *format, ...)
{
	printf("m/%s/%s/", title,
	       data_label); // Prints the formatted title and data label

	// Prints the data
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
	printf("\r\n");
}