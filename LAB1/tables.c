/*
 * tables.c
 *
 *  Created on: Sep 14, 2019
 *      Author: igor
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <defs.h>
#include <LAB1/ch_utf8.h>
#include "tables.h"

keys_t key = {
		.inst = NULL,
		.max_coincidence = 0
};

str_array_t str_array = {
		.instcs = NULL,
		.amount = 0
};

int str_add(char*str)
{
	int strindex = str_array.amount;
	if(str_array.instcs == NULL) {
		str_array.instcs = (str_t*)calloc(1,sizeof(str_t));
		str_array.amount = 1;
	} else {
		str_array.amount++;
		str_array.instcs =
				reallocarray(str_array.instcs, str_array.amount, sizeof(str_t));
		if(str_array.instcs == NULL) {
			pr_err("array alloc failed");
			exit(-1);
		}
	}
	str_array.instcs[strindex].inst = str_alloc(str);
	/* return index */
	return strindex;
}

char* str_alloc(char* str)
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

void str_free(char*str)
{
	if(str == NULL)
		return;
	free(str);
	str = NULL;
}

char* str_get_inst(int index)
{
	if(str_array.amount <= index) {
		pr_err("str_get: out of bounds");
		return NULL;
	}
	return str_array.instcs[index].inst;
}

str_t* str_get(int index)
{
	if(str_array.amount <= index) {
		pr_err("str_get: out of bounds");
		return NULL;
	}
	return &str_array.instcs[index];
}

void str_array_remove(void)
{
	if(str_array.instcs == NULL)
		return;
	for(int i=0; i<str_array.amount; i++) {
		str_free(str_array.instcs[i].inst);
	}
	free(str_array.instcs);
	str_array.instcs = NULL;
	str_array.amount = 0;
}

void str_coincidence(int index)
{
	if(str_array.instcs == NULL) {
		return;
	}
	if(key.inst == NULL) {
		str_array.instcs[index].ch_coincidence = 0;
		return;
	}
	str_array.instcs[index].ch_coincidence = ch_utf_strcmp(str_array.instcs[index].inst, key.inst);
}

void str_max_update(void)
{
	if(str_array.instcs == NULL)
		return;
	int max = 0;
	for(int i=0; i<str_array.amount; i++)
		str_coincidence(i);
	/* find max here */
	for(int i=0; i<str_array.amount; i++) {
		if(str_get(i)->ch_coincidence > max)
			max = str_array.instcs[i].ch_coincidence;
	}
	key.max_coincidence = max;
	pr_debug("key max coincidence=%d", key.max_coincidence);
}

void str_get_max(void(*func)(int))
{
	if(func == NULL)
		return;
	/* find max here */
	for(int i=0; i<str_array.amount; i++) {
		if(key.max_coincidence == str_array.instcs[i].ch_coincidence)
			func(i);
	}
}

// Function to swap two pointers
static void swap(str_t *a, str_t *b)
{
    str_t temp = *a;
    *a = *b;
    *b = temp;
}
// Function to run quicksort on an array of integers
// l is the leftmost starting index, which begins at 0
// r is the rightmost starting index, which begins at array length - 1
static void quicksort(str_t arr[], int l, int r)
{
    // Base case: No need to sort arrays of length <= 1
    if (l >= r)
    {
        return;
    }
    // Choose pivot to be the last element in the subarray
    str_t pivot = arr[r];
    // Index indicating the "split" between elements smaller than pivot and
    // elements greater than pivot
    int cnt = l;
    // Traverse through array from l to r
    for (int i = l; i <= r; i++)
    {
        // If an element less than or equal to the pivot is found...
        if (arr[i].ch_coincidence <= pivot.ch_coincidence)
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

void str_sort(void)
{
	quicksort(str_array.instcs, 0, str_array.amount - 1);
}

void key_set(char* str)
{
	str_free(key.inst);
	key.inst = str_alloc(str);
}

void str_del(int index)
{
	str_free(str_get_inst(index));
	if(str_array.amount == 0)
		return;
	if(str_array.amount <= index)
		return;
	for(int i=index+1; i<str_array.amount; i++) {
		str_array.instcs[i-1] = str_array.instcs[i];
	}
	str_array.amount--;
	str_array.instcs = reallocarray(str_array.instcs, str_array.amount, sizeof(str_t));
}

void str_modify(int index, char* new)
{
	if(str_array.amount == 0)
		return;
	if(str_array.amount <= index)
		return;
	str_free(str_get_inst(index));
	str_get(index)->inst = str_alloc(new);
}

/* Find first occurrence index of key in array
 * Returns: an index in range [0, n-1] if key belongs
 *          to array, -1 if key doesn't belong to array
 */
static int bin_first(str_t arr[], int low, int high, int key)
{
    int ans = -1;

    while (low <= high) {
        int mid = low + (high - low + 1) / 2;
        int midVal = arr[mid].ch_coincidence;

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

/* Find last occurrence index of key in array
 * Returns: an index in range [0, n-1] if key
             belongs to array,
 *          -1 if key doesn't belong to array
 */
static int bin_last(str_t arr[], int low, int high, int key)
{
    int ans = -1;

    while (low <= high) {
        int mid = low + (high - low + 1) / 2;
        int midVal = arr[mid].ch_coincidence;

        if (midVal < key) {

            // if mid is less than key, then all elements
            // in range [low, mid - 1] are also less
            // so we now search in [mid + 1, high]
            low = mid + 1;
        }
        else if (midVal > key) {

            // if mid is greater than key, then all
            // elements in range [mid + 1, high] are
            // also greater so we now search in
            // [low, mid - 1]
            high = mid - 1;
        }
        else if (midVal == key) {

            // if mid is equal to key, we note down
            // the last found index then we search
            // for more in right side of mid
            // so we now search in [mid + 1, high]
            ans = mid;
            low = mid + 1;
        }
    }

    return ans;
}

void str_get_max_bin(void(*func)(int))
{
	if(func == NULL)
		return;
	/* find max here */
	int lowest = bin_first(str_array.instcs, 0, str_array.amount-1, key.max_coincidence);
	int highest = bin_last(str_array.instcs, 0, str_array.amount-1, key.max_coincidence);
	if(lowest == -1 || highest == -1) {
		pr_err("Array not sorted");
		return;
	}
	for(int i=lowest; i<=highest; i++) {
		func(i);
	}
}

void str_insert(int index, char* str)
{
	if(str_array.instcs == NULL) {
		pr_err("cant insert str into empty array");
		return;
	}
	if(index >= str_array.amount) {
		pr_err("index out of bounds");
		return;
	}
	/* realloc array */
	str_array.amount++;
	str_array.instcs =
			reallocarray(str_array.instcs, str_array.amount, sizeof(str_t));
	if(str_array.instcs == NULL) {
		pr_err("array alloc failed");
		exit(-1);
	}
	/* shift all elements higher index to the right */
	for(int i=str_array.amount - 1; i>index; i--)
		str_array.instcs[i] = str_array.instcs[i-1];
	str_array.instcs[index].inst = str_alloc(str);
}
