#include "easymemory.h"
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

size_t g_ezn_allocated_bytes = 0;

void* ez_allocate(size_t amount, size_t size) {
	g_ezn_allocated_bytes += (amount * size) + sizeof(size_t);
	size_t numbytes = (amount * size) + sizeof(size_t);
	void* full_ptr = calloc(numbytes, sizeof(char));
	memcpy(full_ptr, &numbytes, sizeof(numbytes));
	return (void*)((char*)full_ptr + sizeof(size_t));
}

void ez_free(void* ptr) {
	if (ptr == NULL) return;
	size_t ptrsize = 0;
	void* full_ptr = (void*)((char*)ptr - sizeof(size_t));
	memcpy(&ptrsize, full_ptr, sizeof(size_t));
	g_ezn_allocated_bytes -= ptrsize;
	free(full_ptr);
}

size_t ez_allocated_bytes() {
	return g_ezn_allocated_bytes;
}
