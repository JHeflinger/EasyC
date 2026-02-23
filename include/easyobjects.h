#ifndef EASYOBJECTS_H
#define EASYOBJECTS_H

#include "easylogger.h"
#include "easymemory.h"
#include "easybool.h"
#include <stdint.h>
#include <string.h>

// dynamic structs
#define DECLARE_PAIR(T) \
typedef struct { \
	T value[2]; \
} PAIR_##T;

#define DECLARE_TRIPLET(T) \
typedef struct { \
	T value[3]; \
} TRIPLET_##T;

#define DECLARE_QUAD(T) \
typedef struct { \
	T value[4]; \
} QUAD_##T;

#define DECLARE_ARRLIST(T) \
typedef struct { \
	size_t size; \
	size_t maxsize; \
	T* data; \
} ARRLIST_##T; \
\
void ARRLIST_##T##_zero(ARRLIST_##T* list, size_t size); \
void ARRLIST_##T##_add(ARRLIST_##T* list, T element); \
void ARRLIST_##T##_insert(ARRLIST_##T* list, T element, size_t index); \
int ARRLIST_##T##_has(ARRLIST_##T* list, T element); \
void ARRLIST_##T##_remove(ARRLIST_##T* list, size_t index); \
T ARRLIST_##T##_get(ARRLIST_##T* list, size_t index); \
void ARRLIST_##T##_clear(ARRLIST_##T* list);

#define IMPL_ARRLIST(T) \
void ARRLIST_##T##_zero(ARRLIST_##T* list, size_t size) { \
    ARRLIST_##T##_clear(list); \
    list->maxsize = size; \
    list->size = size; \
    list->data = (T*)EZ_ALLOC(size, sizeof(T)); \
} \
\
void ARRLIST_##T##_add(ARRLIST_##T* list, T element) { \
	if (list->maxsize == 0) { \
		list->data = (T*)EZ_ALLOC(1, sizeof(T)); \
		list->size = 1; \
		list->maxsize = 1; \
		memcpy(list->data, &element, sizeof(T)); \
	} else if (list->size < list->maxsize) { \
		memcpy(&(list->data[list->size]), &element, sizeof(T)); \
		list->size++; \
	} else { \
		list->maxsize *= 2; \
		T* newdata = (T*)EZ_ALLOC(list->maxsize, sizeof(T)); \
		memcpy(newdata, list->data, sizeof(T)*list->size); \
		EZ_FREE(list->data); \
		list->data = newdata; \
		memcpy(&(list->data[list->size]), &element, sizeof(T)); \
		list->size++; \
	} \
} \
\
void ARRLIST_##T##_insert(ARRLIST_##T* list, T element, size_t index) { \
	if (index > list->size) { \
		EZ_FATAL("Invalid arraylist index to insert"); \
	} else if (index == list->size) { \
		ARRLIST_##T##_add(list, element); \
	} else { \
		ARRLIST_##T##_add(list, element); \
		for (size_t i = list->size; i > 0; i--) { \
			size_t ind = i - 1; \
			if (ind == index) { \
				memcpy(&(list->data[ind]), &element, sizeof(T)); \
				break; \
			} else { \
				memcpy(&(list->data[ind]), &(list->data[ind - 1]), sizeof(T)); \
			} \
		} \
	} \
} \
\
int ARRLIST_##T##_has(ARRLIST_##T* list, T element) { \
	for (size_t i = 0; i < list->size; i++) \
		if (memcmp(&element, &(list->data[i]), sizeof(T)) == 0) return TRUE; \
	return FALSE; \
} \
\
void ARRLIST_##T##_remove(ARRLIST_##T* list, size_t index) { \
	if (index >= list->size) \
		EZ_FATAL("Invalid arraylist index to remove"); \
	if (index == list->size - 1)  { \
		list->size--; \
		return; \
	} \
	for (size_t i = index; i < list->size - 1; i++) \
		list->data[i] = list->data[i + 1]; \
	list->size--; \
} \
\
T ARRLIST_##T##_get(ARRLIST_##T* list, size_t index) { \
	if (index >= list->size) \
		EZ_FATAL("Invalid arraylist index to get"); \
	return list->data[index]; \
} \
\
void ARRLIST_##T##_clear(ARRLIST_##T* list) { \
	if (list->data != NULL) \
		EZ_FREE(list->data); \
	list->data = NULL; \
	list->size = 0; \
	list->maxsize = 0; \
}

#define DECLARE_ARRLIST_NAMED(name, T) \
typedef struct { \
	size_t size; \
	size_t maxsize; \
	T* data; \
} ARRLIST_##name; \
\
void ARRLIST_##name##_zero(ARRLIST_##name* list, size_t size); \
void ARRLIST_##name##_add(ARRLIST_##name* list, T element); \
void ARRLIST_##name##_insert(ARRLIST_##name* list, T element, size_t index); \
int ARRLIST_##name##_has(ARRLIST_##name* list, T element); \
void ARRLIST_##name##_remove(ARRLIST_##name* list, size_t index); \
T ARRLIST_##name##_get(ARRLIST_##name* list, size_t index); \
void ARRLIST_##name##_clear(ARRLIST_##name* list);

#define IMPL_ARRLIST_NAMED(name, T) \
void ARRLIST_##name##_zero(ARRLIST_##name* list, size_t size) { \
    ARRLIST_##name##_clear(list); \
    list->maxsize = size; \
    list->size = size; \
    list->data = (T*)EZ_ALLOC(size, sizeof(T)); \
} \
\
void ARRLIST_##name##_add(ARRLIST_##name* list, T element) { \
	if (list->maxsize == 0) { \
		list->data = (T*)EZ_ALLOC(1, sizeof(T)); \
		list->size = 1; \
		list->maxsize = 1; \
		memcpy(list->data, &element, sizeof(T)); \
	} else if (list->size < list->maxsize) { \
		memcpy(&(list->data[list->size]), &element, sizeof(T)); \
		list->size++; \
	} else { \
		list->maxsize *= 2; \
		T* newdata = (T*)EZ_ALLOC(list->maxsize, sizeof(T)); \
		memcpy(newdata, list->data, sizeof(T)*list->size); \
		EZ_FREE(list->data); \
		list->data = newdata; \
		memcpy(&(list->data[list->size]), &element, sizeof(T)); \
		list->size++; \
	} \
} \
\
void ARRLIST_##name##_insert(ARRLIST_##name* list, T element, size_t index) { \
	if (index > list->size) { \
		EZ_FATAL("Invalid arraylist index to insert"); \
	} else if (index == list->size) { \
		ARRLIST_##name##_add(list, element); \
	} else { \
		ARRLIST_##name##_add(list, element); \
		for (size_t i = list->size; i > 0; i--) { \
			size_t ind = i - 1; \
			if (ind == index) { \
				memcpy(&(list->data[ind]), &element, sizeof(T)); \
				break; \
			} else { \
				memcpy(&(list->data[ind]), &(list->data[ind - 1]), sizeof(T)); \
			} \
		} \
	} \
} \
\
int ARRLIST_##name##_has(ARRLIST_##name* list, T element) { \
	for (size_t i = 0; i < list->size; i++) \
		if (memcmp(&element, &(list->data[i]), sizeof(T)) == 0) return TRUE; \
	return FALSE; \
} \
\
void ARRLIST_##name##_remove(ARRLIST_##name* list, size_t index) { \
	if (index >= list->size) \
		EZ_FATAL("Invalid arraylist index to remove"); \
	if (index == list->size - 1)  { \
		list->size--; \
		return; \
	} \
	for (size_t i = index; i < list->size - 1; i++) \
		list->data[i] = list->data[i + 1]; \
	list->size--; \
} \
\
T ARRLIST_##name##_get(ARRLIST_##name* list, size_t index) { \
	if (index >= list->size) \
		EZ_FATAL("Invalid arraylist index to get"); \
	return list->data[index]; \
} \
\
void ARRLIST_##name##_clear(ARRLIST_##name* list) { \
	if (list->data != NULL) \
		EZ_FREE(list->data); \
	list->data = NULL; \
	list->size = 0; \
	list->maxsize = 0; \
}

#define DECLARE_HASHMAP(Tkey, Tval, name) \
typedef struct { \
    Tkey key; \
    Tval value; \
    int used; \
} HASHENTRY_##name; \
\
typedef struct { \
    HASHENTRY_##name* entries; \
    size_t capacity; \
    size_t size; \
} HASHMAP_##name; \
\
void HASHMAP_##name##_set(HASHMAP_##name* map, Tkey key, Tval value); \
BOOL HASHMAP_##name##_has(HASHMAP_##name* map, Tkey key); \
Tval HASHMAP_##name##_get(HASHMAP_##name* map, Tkey key); \
void HASHMAP_##name##_remove(HASHMAP_##name* map, Tkey key); \
void HASHMAP_##name##_clear(HASHMAP_##name* map);

#define IMPL_HASHMAP(Tkey, Tval, name, hashfunc) \
void HASHMAP_##name##_set(HASHMAP_##name* map, Tkey key, Tval value) { \
    if (map->capacity == 0 || ((double)map->size / map->capacity > 0.7)) { \
        size_t old_capacity = map->capacity; \
        HASHENTRY_##name* old_entries = map->entries; \
        map->capacity = map->capacity == 0 ? 2 : map->capacity*2; \
        map->entries = EZ_ALLOC(map->capacity, sizeof(HASHENTRY_##name)); \
        map->size = 0; \
        for (size_t i = 0; i < old_capacity; i++) { \
            if (old_entries[i].used == 1) { \
                HASHMAP_##name##_set(map, old_entries[i].key, old_entries[i].value); \
            } \
        } \
        EZ_FREE(old_entries); \
    } \
    uint64_t hash = hashfunc(key); \
    size_t index = hash & (map->capacity - 1); \
    while (map->entries[index].used == 1 && \
           map->entries[index].key != key) { \
        index = (index + 1) & (map->capacity - 1); \
    } \
    if (map->entries[index].used != 1) map->size++; \
    map->entries[index].key = key; \
    map->entries[index].value = value; \
    map->entries[index].used = 1; \
} \
\
BOOL HASHMAP_##name##_has(HASHMAP_##name* map, Tkey key) { \
    uint64_t hash = hashfunc(key); \
    size_t index = hash & (map->capacity - 1); \
    while (map->entries[index].used != 0) { \
        if (map->entries[index].used == 1 && map->entries[index].key == key) { \
            return TRUE; \
        } \
        index = (index + 1) & (map->capacity - 1); \
    } \
    return FALSE; \
} \
\
Tval HASHMAP_##name##_get(HASHMAP_##name* map, Tkey key) { \
    uint64_t hash = hashfunc(key); \
    size_t index = hash & (map->capacity - 1); \
    while (map->entries[index].used != 0) { \
        if (map->entries[index].used == 1 && map->entries[index].key == key) { \
            return map->entries[index].value; \
        } \
        index = (index + 1) & (map->capacity - 1); \
    } \
    EZ_FATAL("Could not get a value from a key that does not exist in the hashmap"); \
    Tval t; \
    return t; \
} \
\
void HASHMAP_##name##_remove(HASHMAP_##name* map, Tkey key) { \
    uint64_t hash = hashfunc(key); \
    size_t index = hash & (map->capacity - 1); \
    while (map->entries[index].used != 0) { \
        if (map->entries[index].used == 1 && \
            map->entries[index].key == key) { \
            map->entries[index].used = 2; \
            map->size--; \
            return; \
        } \
        index = (index + 1) & (map->capacity - 1); \
    } \
    EZ_FATAL("Could not remove a key that does not exist in the hashmap"); \
} \
\
void HASHMAP_##name##_clear(HASHMAP_##name* map) { \
    EZ_FREE(map->entries); \
    map->entries = NULL; \
    map->size = 0; \
    map->size = 0; \
}

#endif
