// NAME: Konner Macias
// EMAIL: konnermacias@g.ucla.edu
// ID: 004603916

#include <string.h>
#include <sched.h>
#include "SortedList.h"

void SortedList_insert(SortedList_t *list, SortedListElement_t *element) {
	SortedListElement_t *temp = list->next;

	while (temp != list && strcmp(element->key, temp->key) > 0) {
		if (opt_yield & INSERT_YIELD){
			sched_yield();
		}
		temp = temp->next;
	}

	element->next = temp;
	element->prev = temp->prev;
	temp->prev->next = element;
	temp->prev = element;
}

int SortedList_delete(SortedListElement_t *element) {
	if (element->next->prev != element || element->prev->next != element){
		return 1;
	}

	if (opt_yield & DELETE_YIELD){
		sched_yield();
	}

	element->prev->next = element->next;
	element->next->prev = element->prev;
	return 0;
}

SortedListElement_t *SortedList_lookup(SortedList_t *list, const char *key) {
	if (list == NULL || key == NULL){
		return NULL;
	}
	SortedListElement_t *temp = list->next;

	if (opt_yield & LOOKUP_YIELD){
		sched_yield();
	}

	while (temp != list){
		if(strcmp(temp->key,key) > 0){
			return NULL;
		}
		else if (strcmp(temp->key, key) == 0){
			return temp;
		}
		temp = temp->next;
	}	
	return NULL;
}

int SortedList_length(SortedList_t *list) {
	int length = 0;
	SortedListElement_t *temp = list->next;

	if (opt_yield & LOOKUP_YIELD){
		sched_yield();
	}

	while (temp != list){
		if (temp->prev->next != temp || temp->next->prev != temp){
			return -1;
		}
		length = length + 1;
		temp = temp->next;
	}
	return length;
}
