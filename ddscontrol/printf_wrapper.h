#ifndef __PRINTF_WRAPPER_H__
#define __PRINTF_WRAPPER_H__


#include <stdarg.h>

// printf to Arduino serial terminal
void serial_printf(char *fmt, ... ){
        char tmp[128]; // resulting string limited to 128 chars
        va_list args;
        va_start (args, fmt );
        vsnprintf(tmp, 128, fmt, args);
        va_end (args);
        Serial.print(tmp);
}


#endif // __PRINTF_WRAPPER_H__
