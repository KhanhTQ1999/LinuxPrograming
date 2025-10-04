#include "strutils.h"
#include <stdbool.h>

void swap_character(char *x, char *y){
    char temp;
    temp = *x;
    *x = *y;
    *y = temp;
}

bool validate_input(const char *str, uint32_t len){
    return (str != NULL && len > 0);
}

bool is_digit(const char c){
    return c >= '0' && c <= '9';
}

bool is_null(const char c){
    return c == '\0';
}

uint32_t str_reverse(char *str, uint32_t len){
    if(validate_input(str, len) == false) 
        return ERRNO_II;

    //Find the last non-null charactor
    uint32_t tail = len-1;
    for(uint32_t idx = 0; idx < len; ++idx){
        if(is_null(str[idx])){
            tail = idx;
            break;
        }
    }

    for(uint32_t idx = 0; idx < tail / 2; idx++){
        swap_character(&str[idx], &str[tail - idx - 1]);
    }

    return SUCCESS;
}
uint32_t str_trim(char *str, uint32_t len){
    if(validate_input(str, len) == false) 
        return ERRNO_II;

    uint32_t start = 0, end = len - 1;
    //Find the first non-space charactor from start and end
    while(start < len && str[start] == ' ')
        start++;
    //Find the last non-space charactor from end
    while(end > start && str[end] == ' ')
        end--;
    //Get the trimmed string
    for(uint32_t idx = 0; idx < len; ++idx){
        if(idx + start <= end)
            str[idx] = str[idx + start];
        else
            str[idx] = '\0';
    }

    return SUCCESS;
}

int32_t str_to_int(const char *str, uint32_t len){
    if(validate_input(str, len) == false) 
        return ERRNO_II;

    //Check negative sign
    uint32_t idx = str[0] == '-' ? 1 : 0;
    uint32_t ret = 0;
    
    //Convert each charactor to int
    for(; idx < len; idx++){
        if(is_null(str[idx])) //End of string
            break;
        if(is_digit(str[idx]) == false)
            return INT32_MAX; //Return INT32_MAX if the charactor is not a digit
        ret = ret * 10 + (str[idx] - '0');
    }

    //Return negative number if the first charactor is '-'
    return str[0] == '-' ? -ret : ret;
}