/* ATTRIBUTION:
 * This file used ideas and concepts from the svec file on Nat Tuck's lecture notes website.
 * It was all hand written and nothing was copied over.
 * If you would like to grill me for this, go ahead.
 * (I don't really see how this would ever really come out differently than the way he did it on the website, but whatever)
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "arrlist.h"

/**
 * Creates a new arraylist, a variable length array.
 * @return The pointer to the created arraylist
 */
arrlist* make_arrlist() {
	arrlist* ar = malloc(sizeof(arrlist));
	ar->size = 0;
	ar->cap = 4; // starting capacity = 4 strings
	ar->data = malloc(4 * sizeof(char*));  
	memset(ar->data, 0, 4 * sizeof(char*)); // fills initial arraylist with 0s  
	return ar;
}

/**
 * Frees up the memory used by the given arraylist.
 * @param ar the arraylist to free
 */
void free_arrlist(arrlist* ar) {
	// free all the data in ar
	for (int i = 0; i < ar->size; ++i) {
		if (ar->data[i] != 0) {
			free(ar->data[i]);
		}
	}
	free(ar->data); // free the array
	free(ar); // free the arraylist itself
}

/**
 * Does the given arraylist contain s?
 * @param  ar the arraylist
 * @param  s  the string to check
 * @return    -1 if not, the first index of s if yes 
 */
int arrlist_contains(arrlist* ar, char* s) {
	int contains = -1;

	if (ar->size == 0) {
		return contains;
	}

	for (int i = 0; i < ar->size; ++i) {
		if (strcmp(ar->data[i], s) == 0) {
			contains = i;
			break;
		}
	}

	return contains;
}

/**
 * Gets the item at the given index from the given arraylist.
 * @param  ar    the arraylist to use
 * @param  index the index to get from
 * @return       the item at index
 */
char* arrlist_get(arrlist* ar, int index) {
	return ar->data[index]; // return the data at the given index
}

/**
 * Puts the given char* into the arraylist at the given index.
 * @param ar     the arraylist to use
 * @param index  the index to place at
 * @param to_put the char* to put
 */
void arrlist_put(arrlist* ar, int index, char* to_put) {
	if (index >= 0) {
		ar->data[index] = strdup(to_put); // strdup the string and put it in the index
	}
}

/**
 * Adds an item to the back of a given arraylist, updating size and capacity as needed.
 * @param ar     the arraylist to use
 * @param to_add the char* to add
 */
void arrlist_add(arrlist* ar, char* to_add) {
	int size = ar->size;

	if (size >= ar->cap) {
		ar->cap *= 2; // increase capacity (2x)
		ar->data = (char**) realloc(ar->data, ar->cap * sizeof(char*));
	}

	ar->size = size + 1;
	arrlist_put(ar, size, to_add);
}

/**
 * Prints the contents of an arraylist.
 * @param ar the arraylist to print
 */
void arrlist_print(arrlist* ar) {
	if (ar->size == 0) {
		return;
	} else {
		for (int i = 0; i < ar->size; ++i) {
			printf("%s ", arrlist_get(ar, i));
		}
	}
}

/**
 * Copies the given array into a new array and returns it
 * @param  ar the array to copy
 * @param  i  start location
 * @param  j  end location
 * @return    the copied array
 */
arrlist* arrlist_copy(arrlist* ar, int i, int j) {
	arrlist* new_ar = make_arrlist();

	int finish = j;

	for (int index = i; index < finish; ++index) {
		char to_add[strlen(ar->data[index])];
		memcpy(to_add, ar->data[index], strlen(ar->data[index]));
		to_add[strlen(ar->data[index])] = 0;
		arrlist_add(new_ar, to_add);
	}

	return new_ar;
}
