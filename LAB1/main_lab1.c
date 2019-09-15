#include <stdio.h>
#include <locale.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <defs.h>
#include <LAB1/tables.h>
#include "ch_utf8.h"
#include "utf8.h"
#include <LAB1/cli.h>
void tests(void)
{
    char A_ascii = 'A';
    char * A_utf = "А";
    assert(ch_num(A_utf) == 1040);
    assert(ch_is_same(ch_num(A_utf), A_ascii));
    char * res = ch_utf_diff_alloc("123456", "1234556");
    assert(strlen(res) == 5);
    free(res);
}
char * strs[] = { "Вакуоль", "Вакула", "Византия", "BakerStreet", "Baku", "Baobab", "binocular", "END"};

void printall(void)
{
	printf("all:\n");
    for(int i=0; i<str_array.amount; i++) {
    	printf("str:%d %s\n", str_get(i)->ch_coincidence, str_get_inst(i));
    }
}

void printres(void)
{
	void callback(int ind)
	{
		if(key.inst == NULL)
			return;
        char* diff = ch_utf_diff_alloc(str_get_inst(ind), key.inst);
            printf(" " COLOR_RED "%s" COLOR_DEF "%s",
                diff,
                &str_get_inst(ind)[ch_utf_memcmp(str_get_inst(ind), key.inst)]
                );
            free(diff);

	}
	str_max_update();
	str_get_max(callback);
}
int main()
{
    /* tests */
    tests();
    str_add("Вакуоль");
    str_add("Воронежь");
    str_add("Воронежь");
		fgets(cli_line, sizeof cli_line, stdin);
		cli_get_words();
	pr_info("locale: %s\n",setlocale(LC_ALL,""));
	while(1) {
        printf("\n-> ");
		fgets(cli_line, sizeof cli_line, stdin);
		cli_get_words();
		pr_debug("w0:%s, w1:%s", ARG_0, ARG_1);
		if(CLI_IS("key")) { key_set(ARG_1); printf("key=%s", key.inst);}
		if(CLI_IS("all")) printall();
		if(CLI_IS("res")) printres();
		//fgets(key, sizeof key, stdin);
        int i;
        /*
        for(i=0; ; i++) {
            if(!strcmp(strs[i], "END"))
                break;
            char* same = ch_utf_diff_alloc(strs[i], key);
            printf(" " COLOR_RED "%s" COLOR_DEF "%s", 
                same,
                &strs[i][ch_utf_memcmp(strs[i], key)]
                );
            free(same);
        }*/
	}
}
