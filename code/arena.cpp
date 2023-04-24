#include "arena.h"

Region*
region_new(usize cap)
{
	usize s = sizeof(Region) + sizeof(uintptr_t)*cap;
	Region* r = (Region*)malloc(s);
	assert(r && "Region NOT allocated");
	r->next = NULL;
	r->used = 0;
	r->capacity = cap;
	return(r);
}

void
region_free(Region* r)
{
	free(r);
}

void*
arena_alloc(Arena* a, usize size)
{
	if(a->end == NULL) {
		assert(a->begin == NULL);
		usize cap = DEFAULT_REGION_CAPACITY;
		if(cap < size) cap = size;
		a->end = region_new(cap);
		a->begin = a->end;
	}

	while(a->end->used + size > a->end->capacity && a->end->next != NULL) {
		a->end = a->end->next;
	}

	if(a->end->used + size > a->end->capacity) {
		assert(a->end->next == NULL && "Arena->End->Next NOT null");
		usize cap = DEFAULT_REGION_CAPACITY;
		if(cap < size) cap = size;
		a->end->next = region_new(cap);
		a->end = a->end->next;
	}

	void* result = &a->end->base[a->end->used];
	a->end->used += size;
	return(result);
}

void*
arena_realloc(Arena* a, void* oldptr, usize oldsize, usize newsize)
{
	if(newsize <= oldsize) return(oldptr);
	void* newptr = arena_alloc(a, newsize);
	char* newptr_char = (char*)newptr;
	char* oldptr_char = (char*)oldptr;
	for(usize i = 0; i < oldsize; i++) {
		newptr_char[i] = oldptr_char[i];
	}
	return(newptr);
}

void
arena_reset(Arena* a)
{
	for(Region* r = a->begin; r != NULL; r = r->next) {
		r->used = 0;
	}
	a->end = a->begin;
}

void
arena_free(Arena* a)
{
	Region* r = a->begin;
	while(r) {
		Region* r0 = r;
		r = r->next;
		region_free(r0);
	}
	a->begin = 0;
	a->end = 0;
}
