#include "exception.h"

void warning_format_log(const char* function_name, const char* file_name, int line_number, const char* format, ...)
{
	va_list marker;
	va_start(marker, format);

	const char* fs = "warning. function:[%s] file:[%s] line:[%d] (%s)\n";

	char* format_buffer = new char[strlen(format) + strlen(function_name) + strlen(file_name) + strlen(fs) + 24];

	sprintf(format_buffer, fs, function_name, file_name, line_number, format);

	vprintf(format_buffer, marker);
	va_end(marker);

	delete[] format_buffer;
}