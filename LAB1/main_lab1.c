#include <stdio.h>
#include <locale.h>
#include <string.h>
#include <assert.h>
#include "utf8.h"
#include "lab1.h"
void tests(void)
{
    char A_ascii = 'A';
    char * A_utf = "А";
    assert(ch_num(A_utf) == 1040);
    assert(ch_is_same(ch_num(A_utf), A_ascii));
}
int
main(){
    /* tests */
    tests();
    char*c = "Ы";
    int j=0;
    printf("могем писать кирилицай");
    printf("char %s, sym %d\n", c, u8_nextchar(c, &j));
	printf("locale: %s\n",setlocale(LC_ALL,""));
    const char * key = "1234568iaA";
    char str[1000];
	while(1) {
        printf("\nstr->");
		fgets(str, sizeof str, stdin);
		printf("str: %s key: %s, size %d\n", str,key, u8_strlen(str));
        printf("res: %s", ch_utf_diff_alloc(str, key));
	}
}
