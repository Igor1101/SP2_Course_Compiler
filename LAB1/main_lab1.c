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
char * strs[] = { "Вакуоль", "Вакула", "Византия", "Baker Street", "Baku", "Baobab", "binocular", "END"};
int
main(){
    /* tests */
    tests();
	pr_info("locale: %s\n",setlocale(LC_ALL,""));
    char key[1000];
	while(1) {
        printf("\nkey-> ");
		fgets(key, sizeof key, stdin);
        int i;
        for(i=0; ; i++) {
            if(!strcmp(strs[i], "END"))
                break;
            char* same = ch_utf_diff_alloc(key, strs[i]);
            printf(" " COLOR_RED "%s" COLOR_DEF "%s", 
                same,
                &strs[i][ch_utf_memcmp(key,strs[i])]
                );
            free(same);
        }
	}
}
