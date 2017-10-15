/* ATTRIBUTION:
 * This file used ideas and concepts from the svec file on Nat Tuck's lecture notes website.
 * It was all hand written and nothing was copied over.
 * If you would like to grill me for this, go ahead.
 * (I don't really see how this would ever really come out differently than the way he did it on the website, but whatever)
 */

#ifndef ARRLIST_H
#define ARRLIST_H

typedef struct arrlist {
	int size; // current size of the array list
	int cap; // max capacity of the array list
	char** data; // the data being held
} arrlist;

arrlist* make_arrlist(); // creates a new array list

void free_arrlist(arrlist* ar); // frees the given arraylist

int arrlist_contains(arrlist* ar, char* s); // does this arrlist contain the token s

char* arrlist_get(arrlist* ar, int index); // gets the item from the given arraylist at index

void arrlist_put(arrlist* ar, int index, char* to_put); // puts to_put in ar at index (overwrites if needed)

void arrlist_add(arrlist* ar, char* to_add); // adds the given item to the front of the arraylist (adds capacity if needed)

void arrlist_print(arrlist* ar); // prints all the contents of the given arrlist

arrlist* arrlist_copy(arrlist* ar, int i, int j); // copies ar from i - (j-1) into a new arraylist 

#endif
