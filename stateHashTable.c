/*
 * stateHashTable.c
 *
 *  Created on: Sep 26, 2014
 *      Author: ryan
 */

#include "freecell.h"
#include "lookup3.h"

hash_table_t *create_hash_table(int size) {
	int i;

	hash_table_t *new_table;
	if(size<1) return NULL;
	if((new_table = malloc(sizeof(hash_table_t)))==NULL)
		return NULL;
	if( (new_table->table = malloc(sizeof(list_t *) * hashsize(size))) == NULL )
		return NULL;

	for(i=0; i<hashsize(size); i++)
		new_table->table[i] = NULL;

	new_table->size = size;

	return new_table;
}

unsigned int hash(hash_table_t *hashtable, State *s) {
	int i;

	char seq[16];
	for(i=0;i<NUMCOLS;i++)
		seq[i] = s->colheight[i];

	for(i=0;i<CELLS; i++)
		seq[i+NUMCOLS] = s->freecell[i];

	for(i=0;i<CELLS; i++)
		seq[i+NUMCOLS+CELLS] = s->stack[i];

	/*
	-------------------------------------------------------------------------------
	hashlittle() -- hash a variable-length key into a 32-bit value
	  k       : the key (the unaligned variable-length array of bytes)
	  length  : the length of the key, counting by bytes
	  initval : can be any 4-byte value
	Returns a 32-bit value.  Every bit of the key affects every bit of
	the return value.  Two keys differing by one or two bits will have
	totally different hash values.

	The best hash table sizes are powers of 2.  There is no need to do
	mod a prime (mod is sooo slow!).  If you need less than 32 bits,
	use a bitmask.  For example, if you need only 10 bits, do
	  h = (h & hashmask(10));
	In which case, the hash table should have hashsize(10) elements.*/
	return (hashlittle(seq, 16, 8) & hashmask(hashtable->size));
}

State *lookup_state(hash_table_t *hashtable, State *s) {
	list_t * list;
	unsigned int hashval = hash(hashtable, s);

	for(list = hashtable->table[hashval]; list != NULL; list=list->next) {
		if(sameState(s, list->state))
			return list->state;
	}

	return NULL;
}

int add_state(hash_table_t *hashtable, State *s) {
	list_t *new_list;
	State *current_list;

	unsigned int hashval = hash(hashtable, s);
	if( (new_list = malloc(sizeof(list_t))) == NULL ) {
		return 1;
	}

	current_list = lookup_state(hashtable, s);
	if(current_list != NULL) {
		free(new_list);
		return 2;
	}

	new_list->state = s;
	new_list->next = hashtable->table[hashval];
	hashtable->table[hashval] = new_list;

	return 0;
}

void free_table(hash_table_t *hashtable) {
	int i;
	list_t *list, *temp;

	if(hashtable == NULL) return;

	for(i=0; i<hashtable->size; i++) {
		list=hashtable->table[i];
		while(list!=NULL) {
			temp = list;
			list = list->next;
			free(temp);
		}
	}
	free(hashtable->table);
	free(hashtable);
}
