#include <stdio.h>
#include <stdarg.h>

/*
* @brief Adds a new data point to a serial monitor window.
* @param title The title of the serial monitor window.
* @param data_label The label for the data point.
* @param format The datatype format string for the data point. (e.g. "%d" "%f" "%s" etc.)
* @param ... The data point to be added. This can be a number, string, etc. The datatype must match the format string.
*
* @note Example usage: serial_monitor("Test Panel", "First Num", "%d", 95);
*/
void serial_monitor(const char *title, const char *data_label,
		    const char *format, ...);