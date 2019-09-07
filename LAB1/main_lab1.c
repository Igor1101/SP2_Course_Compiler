#include <stdio.h>
#include <locale.h>
#include <string.h>
#include "utf8.h"
#include "lab1.h"
int
main(){
    char*c = "Ы";
    int j=0;
    printf("могем писать кирилицай");
    printf("char %s, sym %d\n", c, u8_nextchar(c, &j));
	printf("locale: %s\n",setlocale(LC_ALL,""));
	while(1) {
		char str[1000];
		fgets(str, sizeof str, stdin);
		printf("str: %s, size %d\n", str, u8_strlen(str));
		for(int i=0; /*i<u8_strlen(str)*/; ) {
            if(isutf(*str)) {
                printf("UTF-8:");
                char utf8ch [10];
                uint32_t ch = u8_nextchar(str, &i);
                if(ch == 0) {
                    printf("end of seq\n");
                    break;
                }
                u8_toutf8(utf8ch, sizeof utf8ch, &ch, sizeof ch);
                u8_dec(str, &i);
                u8_printf("char %s, sym %d\n", utf8ch,
                        u8_nextchar(str, &i));
            } else {
			    printf("char %c, sym %d\n", str[i], str[i]);
                i++;
            }
		}
	}
}
