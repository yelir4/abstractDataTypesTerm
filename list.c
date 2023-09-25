#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<assert.h>
#include<stdbool.h>
#include<math.h>
#include "list.h"

// coen 12 term project, list.c
// 11/30/21

typedef struct node NODE;
typedef struct list LIST;

/* node struct
 * contains data using a circular array
 * previous and next node pointers for doubly-linked implementation
 */
struct node {
	int count;
	int length;
	int key; // index of first filled slot in circular array
	void **data;
	struct node *prev; // previous pointer provides implicit tail pointer
	struct node *next;
};

/* list struct,
 * utilizes nodes in which each node contains circular array
 */
struct list {
	int count;		// total number of elements in list
	int length;		// total number of nodes in list
	int nextLength;		// length of next node to be added to list, doubled each time so memory is not wasted, allocation not often
	struct node *head;	// pointer to first node (dummy)
};

/* *createList function
 * allocates memory for new list and head pointer
 * @return pointer to list lp
 * @notation O(1)
 */ 
LIST *createList(void) {
	LIST *lp = malloc(sizeof(LIST));
	assert(lp != NULL);

	lp->count = 0;
	lp->length = 0;
	lp->nextLength = 10; // first node has length 10, length doubles with successive nodes

	NODE *np = malloc(sizeof(NODE)); // head is a dummy/sentinel node
	assert(np != NULL);
	np->count = 0;
	np->length = 0; // sentinel is "full", cannot insert elements
	np->key = 0;
	np->data = NULL;
	np->prev = np;
	np->next = np;
	lp->head = np;
	return lp;
}

/* destroyList function
 * @param lp list pointer
 * deallocate memory associated with lp
 * @notation O(m) where m is number of nodes in lp (lp->length)
 */
void destroyList(LIST *lp) {
	assert(lp != NULL);

	NODE *nCur;
	while(lp->length != 0) { // nodes present in lp
		nCur = lp->head->next;
		lp->head->next = nCur->next;
		nCur->next->prev = lp->head;
		free(nCur->data);
		free(nCur);
		lp->length--;
	}
	free(lp->head); // head's data was not allocated
	free(lp);
}

/* numItems function
 * @return number of elements in lp (lp->count)
 * @notation O(1)
 */
int numItems(LIST *lp) {
	assert(lp != NULL);
	return lp->count;
}

/* addFirst function
 * add @param item to first slot in lp
 * allocate new node if necessary (memory cost (void*)*lp->nextLength)
 * @notation O(1)
 */
void addFirst(LIST *lp, void *item) {
	assert(lp != NULL);
	assert(item != NULL);

	if(lp->head->next->count == lp->head->next->length) { // first node in list is full, allocate new node
		NODE *nNew = malloc(sizeof(NODE));
		assert(nNew != NULL);
		nNew->count = 0;
		nNew->length = lp->nextLength;
		lp->length++;
		lp->nextLength *= 2; // double next node's array length, should go 10, 20, 40 etc.
		nNew->key = 0;
		nNew->data = malloc(sizeof(void*)*nNew->length);
		assert(nNew->data != NULL);
		nNew->prev = lp->head;
		nNew->next = lp->head->next;
		lp->head->next->prev = nNew;
		lp->head->next = nNew;
	}

	if(lp->head->next->count > 0) { // there are elements in the first node of lp, key must be decremented for new element
		lp->head->next->key += lp->head->next->length - 1;
		lp->head->next->key %= lp->head->next->length;
	}
	lp->head->next->data[lp->head->next->key] = item;
	lp->head->next->count++;
	lp->count++;
}

/* addLast function
 * add item to last slot in lp
 * allocate new node if necessary (memory cost (void*)*lp->nextLength)
 * @notation O(1)
 */
void addLast(LIST *lp, void *item) {
	assert(lp != NULL);
	assert(item != NULL);

	if(lp->head->prev->count == lp->head->prev->length) { // last node in lp is full
		NODE *nNew = malloc(sizeof(NODE));
		assert(nNew != NULL);
		nNew->count = 0;
		nNew->length = lp->nextLength;
		lp->length++;
		lp->nextLength *= 2; // double next node's array length
		nNew->key = 0;
		nNew->data = malloc(sizeof(void*)*nNew->length);
		assert(nNew->data != NULL);
		nNew->prev = lp->head->prev;
		nNew->next = lp->head;
		lp->head->prev->next = nNew;
		lp->head->prev = nNew;
	}
	int i = (lp->head->prev->key + lp->head->prev->count++) % lp->head->prev->length; // get "last" index to place new element
	lp->head->prev->data[i] = item;
	lp->count++;
}

/* *removeFirst function
 * @precondition lp must not be empty
* deallocate memory of first node if empty
 * @return first item in lp after removing it
 * @notation O(1)
 */
void *removeFirst(LIST *lp) {
	assert(lp != NULL);
	assert(lp->count > 0);

	NODE *nCur;
	if(lp->head->next->count == 0) { // first node in lp is empty, deallocate
		nCur = lp->head->next;
		lp->head->next = nCur->next;
		nCur->next->prev = lp->head;
		free(nCur->data);
		free(nCur);
		lp->length--;
		lp->nextLength /= 2; // half next node's array length
	}
	void *copy = lp->head->next->data[lp->head->next->key++]; // increment array's first filled index
	lp->head->next->key %= lp->head->next->length;
	lp->head->next->count--;
	lp->count--;
	return copy;
}

/* *removeLast function
 * @precondition lp is not empty
 * deallocate memory of "last" node in lp if empty
 * @return last item in lp after removing it
 * @notation O(1)
 */
void *removeLast(LIST *lp) {
	assert(lp != NULL);
	assert(lp->count > 0);

	NODE *nCur;
	if(lp->head->prev->count == 0) { // last node in lp is empty
		nCur = lp->head->prev;
		lp->head->prev = nCur->prev;
		nCur->prev->next = lp->head;
		free(nCur->data);
		free(nCur);
		lp->length--;
		lp->nextLength /= 2;
	}
	void *copy = lp->head->prev->data[(lp->head->prev->key + lp->head->prev->count - 1) % lp->head->prev->length];
	lp->head->prev->count--;
	lp->count--;
	return copy;
}

/* *getItem function
 * @return item at @param index in lp
 * @precondition index must be within range (less than lp->count)
 * @notation O(m) where m is nodes in list (lp->length)
 */
void *getItem(LIST *lp, int index) {
	assert(lp != NULL);
	assert(lp->count > index);

	NODE *nCur = lp->head->next;
	while(index >= nCur->count) {
		index -= nCur->count;
		nCur = nCur->next;
	}
	return(nCur->data[(nCur->key+index) % nCur->length]); 
}

/* setItem function
 * update item at index in lp
 * @precondition index must be within range
 * @notation O(m) where m is nodes in list (lp->length)
 */
void setItem(LIST *lp, int index, void *item) {
	assert(lp != NULL);
	assert(lp->count > index);
	assert(item != NULL);

	NODE *nCur = lp->head->next;
	while(index >= nCur->count) {
		index -= nCur->count;
		nCur = nCur->next;
	}
	nCur->data[(nCur->key+index) % nCur->length] = item;
}
