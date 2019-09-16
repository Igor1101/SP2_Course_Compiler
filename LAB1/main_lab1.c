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
    char * strs[] = { "Вакуоль", "Вакула", "Византия",
    		"BakerStreet", "Baku", "Baobab", "binocular", "Banana", "Ваниль",
			"1223","END"};
    str_array_remove();
    int i;
    for(i=0; ; i++) {
    	if(!strcmp(strs[i], "END"))
    		break;
    	str_add(strs[i]);
    }
    assert(i == str_array.amount);
}

void printall(void)
{
    for(int i=0; i<str_array.amount; i++) {
    	printf("%d %s \t\tcoinc: %d\n", i, str_get_inst(i), str_get(i)->ch_coincidence);
    }
}

void printres(bool binary)
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
	if(binary)
		str_get_max_bin(callback);
	else
		str_get_max(callback);
}

void rmall(void)
{
	str_array_remove();
	str_free(key.inst);
}

int main()
{
    /* tests */
	pr_info("locale: %s\n",setlocale(LC_ALL,""));
    tests();
	while(1) {
        printf("\n-> ");
		if(fgets(cli_line, sizeof cli_line, stdin) == NULL) {
			rmall();
			exit(0);
		}
		cli_get_words();
		pr_debug("w0:%s, w1:%s", ARG_0, ARG_1);
		if(CLI_IS("key")) { key_set(ARG_1); printf("key=%s", key.inst); str_max_update();}
		if(CLI_IS("all")) printall();
		if(CLI_IS("res")) printres(false);
		if(CLI_IS("add")) str_add(ARG_1);
		if(CLI_IS("insert")) str_insert(strtol(ARG_1, NULL, 10), ARG_2);
		if(CLI_IS("del")) str_del(strtol(ARG_1, NULL, 10));
		if(CLI_IS("modify")) str_modify(strtol(ARG_1, NULL, 10), ARG_2);
		if(CLI_IS("reset")) rmall();
		if(CLI_IS("exit")) { rmall(); exit(0); }
		if(CLI_IS("sort")) { str_sort(); }
		if(CLI_IS("resbin")) printres(true);
		if(CLI_IS("delres")) str_del_max();
		str_max_update();
	}
}
