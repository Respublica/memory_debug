#include "base.hpp"

#define MAX_BUFFER_LEN 1024

std::string format(const char *fmt, ...)
{
    char textString[MAX_BUFFER_LEN] = {'\0'};

    // -- empty the buffer properly to ensure no leaks.
    memset(textString, '\0', sizeof(textString));

    va_list args;
    va_start(args, fmt);
    vsnprintf(textString, MAX_BUFFER_LEN, fmt, args);
    va_end(args);
    std::string retStr = textString;
    return retStr;
}