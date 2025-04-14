#ifndef EASYOBJECTS_H
#define EASYOBJECTS_H

#include "easylogger.h"
#include "easymemory.h"
#include <stdint.h>
#include <string.h>

#ifndef TRUE
#define TRUE 1
#endif 

#ifndef FALSE
#define FALSE 0
#endif

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
void ARRLIST_##T##_add(ARRLIST_##T* list, T element); \
int ARRLIST_##T##_has(ARRLIST_##T* list, T element); \
void ARRLIST_##T##_remove(ARRLIST_##T* list, size_t index); \
T ARRLIST_##T##_get(ARRLIST_##T* list, size_t index); \
void ARRLIST_##T##_clear(ARRLIST_##T* list);

#define IMPL_ARRLIST(T) \
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
void ARRLIST_##name##_add(ARRLIST_##name* list, T element); \
int ARRLIST_##name##_has(ARRLIST_##name* list, T element); \
void ARRLIST_##name##_remove(ARRLIST_##name* list, size_t index); \
T ARRLIST_##name##_get(ARRLIST_##name* list, size_t index); \
void ARRLIST_##name##_clear(ARRLIST_##name* list);

#define IMPL_ARRLIST_NAMED(name, T) \
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

#endif
