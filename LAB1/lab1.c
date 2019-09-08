#include <stdio.h>
#include <locale.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "utf8.h"
#include "lab1.h"

ch_t ch_num(char* ch)
{
    int j = 0;
    return u8_nextchar(ch, &j);
}

bool ch_is_same(ch_t ch1, ch_t ch2)
{
    if(ch1 == ch2) {
        return true;
    }
    /* cyrrilic */
    ch_t ch_ascii;
    ch_t ch_utf;
    if(ch1 < 255 && ch2 > 255) {
        /* ch1 - ascii, ch2 - unicode */
        ch_ascii = ch1;
        ch_utf = ch2;
    } else if(ch1 > 255 && ch2 < 255) {
        /* ch2 - ascii, ch1 - unicode */
        ch_ascii = ch2;
        ch_utf = ch1;
    } else {
        return false;
    }
    if(ch_num("а") == ch_utf && ch_ascii == 'a')
        return true;
    if(ch_num("с") == ch_utf && ch_ascii == 'c')
        return true;
    if(ch_num("е") == ch_utf && ch_ascii == 'e')
        return true;
    if(ch_num("і") == ch_utf && ch_ascii == 'i')
        return true;
    if(ch_num("к") == ch_utf && ch_ascii == 'k')
        return true;
    if(ch_num("п") == ch_utf && ch_ascii == 'n')
        return true;
    if(ch_num("о") == ch_utf && ch_ascii == 'o')
        return true;
    if(ch_num("г") == ch_utf && ch_ascii == 'r')
        return true;
    if(ch_num("х") == ch_utf && ch_ascii == 'x')
        return true;
    if(ch_num("у") == ch_utf && ch_ascii == 'y')
        return true;
    if(ch_num("А") == ch_utf && ch_ascii == 'A')
        return true;
    if(ch_num("В") == ch_utf && ch_ascii == 'B')
        return true;
    if(ch_num("С") == ch_utf && ch_ascii == 'C')
        return true;
    if(ch_num("Е") == ch_utf && ch_ascii == 'E')
        return true;
    if(ch_num("Н") == ch_utf && ch_ascii == 'H')
        return true;
    if(ch_num("К") == ch_utf && ch_ascii == 'K')
        return true;
    if(ch_num("М") == ch_utf && ch_ascii == 'M')
        return true;
    if(ch_num("О") == ch_utf && ch_ascii == 'O')
        return true;
    if(ch_num("Р") == ch_utf && ch_ascii == 'P')
        return true;
    if(ch_num("Х") == ch_utf && ch_ascii == 'X')
        return true;
    if(ch_num("У") == ch_utf && ch_ascii == 'Y')
        return true;
    return false;
}

int ch_utf_strcmp(char* str1, char* str2)
{
    int i;
    int j;
    int count=0;
    for(i=0, j=0; ;) {
        ch_t ch1 = u8_nextchar(str1, &i);
        ch_t ch2 = u8_nextchar(str2, &j);
        if(ch1 == 0 || ch2 == 0)
            break;
        if(!ch_is_same(ch1, ch2)) {
            break;
        }
        count++;
    }
    return count;
}
int ch_utf_memcmp(char* str1, char* str2)
{
    int i;
    int j;
    for(i=0, j=0; ;) {
        ch_t ch1 = u8_nextchar(str1, &i);
        ch_t ch2 = u8_nextchar(str2, &j);
        if(ch1 == 0 || ch2 == 0)
            break;
        if(!ch_is_same(ch1, ch2)) {
            break;
        }
    }
    u8_dec(str1,&i);
    u8_dec(str1,&j);
    return i;
}
char* ch_utf_diff_alloc(char*str1, char*str2)
{
    int sz = ch_utf_memcmp(str1, str2);
    char* diff = (char*)malloc(strlen(str1)+strlen(str2));
    memcpy(diff, str1, sz);
}
