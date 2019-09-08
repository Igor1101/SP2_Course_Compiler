#include <stdio.h>
#include <locale.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <defs.h>
#include "utf8.h"
#include "lab1.h"
void tests(void)
{
    char A_ascii = 'A';
    char * A_utf = "А";
    assert(ch_num(A_utf) == 1040);
    assert(ch_is_same(ch_num(A_utf), A_ascii));
    char * res = ch_utf_diff_alloc("123456", "1234556");
    assert(strlen(res) == 5);
}

int
main(){
    /* tests */
    tests();
    char*c = "Ы";
    int j=0;
    pr_debug("могем писать кирилицай");
    pr_info("char %s, sym %d", c, u8_nextchar(c, &j));
	pr_info("locale: %s\n",setlocale(LC_ALL,""));
    char * key = "Вакуоль";
    char str[1000];
	while(1) {
        printf("\nkey-> ");
		fgets(str, sizeof str, stdin);
		pr_info("str: %s key: %s, size %d", str,key, u8_strlen(str));
        char* same = ch_utf_diff_alloc(str, key);
        pr_info("res:" COLOR_RED "%s" COLOR_DEF "%s", 
                same,
                &key[ch_utf_memcmp(key, str)]
                );
        free(same);
	}
}
