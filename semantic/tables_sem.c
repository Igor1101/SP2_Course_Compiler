/*
 * tables.c
 *
 *  Created on: Sep 14, 2019
 *      Author: igor
 */
#include "tables_sem.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <defs.h>
#include <lexems/ch_utf8.h>
/*
keys_t key = {
		.inst = NULL,
		.max_coincidence = 0
};*/

ident_array_t ident_array = {
		.instcs = NULL,
		.amount = 0
};

int ident_add(char*str, lexem_t lt)
{
	int strindex = ident_array.amount;
	if(ident_array.instcs == NULL) {
		ident_array.instcs = (ident_t*)calloc(1,sizeof(ident_t));
		ident_array.amount = 1;
	} else {
		ident_array.amount++;
		ident_array.instcs =
				reallocarray(ident_array.instcs, ident_array.amount, sizeof(ident_t));
		if(ident_array.instcs == NULL) {
			pr_err("array alloc failed");
			exit(-1);
		}
	}
	ident_array.instcs[strindex].inst = ident_alloc(str);
	ident_array.instcs[strindex].lext = lt;
	/* return index */
	return strindex;
}

char* ident_alloc(char* str)
{
	size_t sz = strlen(str);
	char* alloc = (char*)calloc(sz, 1);
	if(alloc == NULL) {
		pr_err("out of memory");
		exit(-1);
	}
	strncpy(alloc, str, sz);
	return alloc;
}

void ident_free(char*str)
{
	if(str == NULL)
		return;
	free(str);
	str = NULL;
}

char* ident_get_inst(int index)
{
	if(ident_array.amount <= index) {
		pr_err("ident_get: out of bounds");
		return NULL;
	}
	return ident_array.instcs[index].inst;
}

ident_t* ident_get(int index)
{
	if(ident_array.amount <= index) {
		pr_err("ident_get: out of bounds");
		static ident_t str = {
				.inst = NULL,
				.level = 0,
				.lext = L_NOTDEFINED,
				.synt = 0
		};
		return &str;
	}
	return &ident_array.instcs[index];
}

void ident_array_remove(void)
{
	if(ident_array.instcs == NULL)
		return;
	for(int i=0; i<ident_array.amount; i++) {
		ident_free(ident_array.instcs[i].inst);
	}
	free(ident_array.instcs);
	ident_array.instcs = NULL;
	ident_array.amount = 0;
}

void ident_coincidence(int index)
{
	if(ident_array.instcs == NULL) {
		return;
	}
	if(key.inst == NULL) {
		ident_array.instcs[index].level = 0;
		return;
	}
	ident_array.instcs[index].level = ch_utf_strcmp(ident_array.instcs[index].inst, key.inst);
	if(ident_array.instcs[index].level > key.max_coincidence) {
		key.max_coincidence = ident_array.instcs[index].level;
	}
}

void ident_max_update(void)
{
	if(ident_array.instcs == NULL)
		return;
	key.max_coincidence = 0;
	for(int i=0; i<ident_array.amount; i++)
		ident_coincidence(i);
	pr_debug("key max coincidence=%d", key.max_coincidence);
}

void ident_get_max(void(*func)(int))
{
	if(func == NULL)
		return;
	/* find max here */
	for(int i=0; i<ident_array.amount; i++) {
		if(key.max_coincidence == ident_array.instcs[i].level)
			func(i);
	}
}

// Function to swap two pointers
static void swap(ident_t *a, ident_t *b)
{
    ident_t temp = *a;
    *a = *b;
    *b = temp;
}
// Function to run quicksort on an array of integers
// l is the leftmost starting index, which begins at 0
// r is the rightmost starting index, which begins at array length - 1
static void quicksort(ident_t arr[], int l, int r)
{
    // Base case: No need to sort arrays of length <= 1
    if (l >= r)
    {
        return;
    }
    // Choose pivot to be the last element in the subarray
    ident_t pivot = arr[r];
    // Index indicating the "split" between elements smaller than pivot and
    // elements greater than pivot
    int cnt = l;
    // Traverse through array from l to r
    for (int i = l; i <= r; i++)
    {
        // If an element less than or equal to the pivot is found...
        if (arr[i].level <= pivot.level)
        {
            // Then swap arr[cnt] and arr[i] so that the smaller element arr[i]
            // is to the left of all elements greater than pivot
            swap(&arr[cnt], &arr[i]);
            // Make sure to increment cnt so we can keep track of what to swap
            // arr[i] with
            cnt++;
        }
    }

    // NOTE: cnt is currently at one plus the pivot's index
    // (Hence, the cnt-2 when recursively sorting the left side of pivot)
    quicksort(arr, l, cnt-2); // Recursively sort the left side of pivot
    quicksort(arr, cnt, r);   // Recursively sort the right side of pivot
}

void ident_sort(void)
{
	quicksort(ident_array.instcs, 0, ident_array.amount - 1);
}

/*
void key_set(char* str)
{
	ident_free(key.inst);
	key.inst = ident_alloc(str);
}*/

void ident_del(int index)
{
	ident_free(ident_get_inst(index));
	if(ident_array.amount == 0)
		return;
	if(ident_array.amount <= index)
		return;
	for(int i=index+1; i<ident_array.amount; i++) {
		ident_array.instcs[i-1] = ident_array.instcs[i];
	}
	ident_array.amount--;
	ident_array.instcs = reallocarray(ident_array.instcs, ident_array.amount, sizeof(ident_t));
}

void ident_modify(int index, char* new)
{
	if(ident_array.amount == 0)
		return;
	if(ident_array.amount <= index)
		return;
	ident_free(ident_get_inst(index));
	ident_get(index)->inst = ident_alloc(new);
}

void ident_modify_max(char* new)
{
	if(ident_array.amount == 0)
		return;
	void callback(int i)
	{
		ident_modify(i, new);
	}
	ident_get_max(callback);
}

/* Find first occurrence index of key in array
 * Returns: an index in range [0, n-1] if key belongs
 *          to array, -1 if key doesn't belong to array
 */
static int bin_first(ident_t arr[], int low, int high, int key)
{
    int ans = -1;

    while (low <= high) {
        int mid = low + (high - low + 1) / 2;
        int midVal = arr[mid].level;

        if (midVal < key) {

            // if mid is less than key, all elements
            // in range [low, mid] are also less
            // so we now search in [mid + 1, high]
            low = mid + 1;
        }
        else if (midVal > key) {

            // if mid is greater than key, all elements
            // in range [mid + 1, high] are also greater
            // so we now search in [low, mid - 1]
            high = mid - 1;
        }
        else if (midVal == key) {

            // if mid is equal to key, we note down
            //  the last found index then we search
            // for more in left side of mid
            // so we now search in [low, mid - 1]
            ans = mid;
            high = mid - 1;
        }
    }

    return ans;
}

void ident_get_max_bin(void(*func)(int))
{
	if(func == NULL)
		return;
	/* find max here */
	int lowest = bin_first(ident_array.instcs, 0, ident_array.amount-1, key.max_coincidence);
	if(lowest == -1 ) {
		pr_err("Array not sorted");
		return;
	}
	for(int i=lowest; i<ident_array.amount; i++) {
		if(ident_get(i)->level != key.max_coincidence)
			break;
		func(i);
	}
}

void ident_insert(int index, char* str)
{
	if(ident_array.instcs == NULL) {
		pr_err("cant insert str into empty array");
		return;
	}
	if(index >= ident_array.amount) {
		pr_err("index out of bounds");
		return;
	}
	/* realloc array */
	ident_array.amount++;
	ident_array.instcs =
			reallocarray(ident_array.instcs, ident_array.amount, sizeof(ident_t));
	if(ident_array.instcs == NULL) {
		pr_err("array alloc failed");
		exit(-1);
	}
	/* shift all elements higher index to the right */
	for(int i=ident_array.amount - 1; i>index; i--)
		ident_array.instcs[i] = ident_array.instcs[i-1];
	ident_array.instcs[index].inst = ident_alloc(str);
}

void ident_del_max(void)
{
	printf("removing...");
	try_removing_anothertime: for(int i=0; i<ident_array.amount; i++) {
		if(key.max_coincidence == ident_array.instcs[i].level) {
			printf("%s ", ident_get_inst(i));
			ident_del(i);
			goto try_removing_anothertime;
		}
	}
	puts("");
}
