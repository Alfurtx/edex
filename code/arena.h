#ifndef ARENAH
#define ARENAH

#include "common.h"

#define DEFAULT_REGION_CAPACITY MB(4)

struct Region {
	Region* next;
	usize used;
	usize capacity;
	u8* base;
};

struct Arena {
	Region* begin;
	Region* end;
};

struct TempArena {
	Arena* arena;
	usize lastpos;
	usize pos;
};

Region* region_new(usize cap);
void region_free(Region* r);

void* arena_alloc(Arena* a, usize size);
void* arena_realloc(Arena* a, void* oldptr, usize oldisize, usize newsize);

void arena_reset(Arena* a);
void arena_free(Arena* a);

#endif // ARENAH
