#include "strutils.h"
#include "../CmnBase/log.h"

int main(int argc, char *argv){
    char str[] = " Hello World  ";

    str_trim(str, sizeof(str)/sizeof(str[0]) - 1);
    log_info("Trimmed string: '%s'", str);

    str_reverse(str, sizeof(str)/sizeof(str[0]) - 1);
    log_info("Reverse string: '%s'", str);

    int32_t ret = str_to_int(str, sizeof(str)/sizeof(str[0]) - 1);
    if(ret == INT32_MAX)
        log_error("Conversion error: Non-digit character found.");
    else
        log_info("Converted integer: %d", ret);
    return 0;
}