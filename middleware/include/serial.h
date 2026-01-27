#include <stdio.h>
#include <stdarg.h>

/*
* @brief Adds a new data point to a serial monitor window.
* @param title The title of the serial monitor window.
* @param data_label The label for the data point.
* @param format The datatype format for the data point. (e.g. "%d" "%f" "%s" etc.)
* @param ... The data point to be added. This can be a number, string, etc. The datatype must match the format string.
*
* @note Example usage: serial_monitor("Test Panel", "Test Val", "%d", test_val);
*/
void serial_monitor(const char *title, const char *data_label,
		    const char *format, ...);

/*
* @brief Adds a new data point to a serial graph window.
* @param title The title of the serial graph window.
* @param data_label The label for the data point.
* @param format The datatype format for the data point. (e.g. "%d" "%f" "%u" etc.). This MUST be a plotable value (i.e. a number).
* @param ... The data point to be added. The datatype must match the format string.
*
* @note Example usage: serial_graph("Test Graph", "Test Val", "%d", test_val);
*/
void serial_graph(const char *title, const char *data_label, const char *format,
		  ...);