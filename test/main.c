#include <easyc.h>

int passed_tests = 0;
int total_tests = 0;

#define EZTEST(condition, name) { \
	total_tests++; \
	if (condition) { \
		printf("%d. %s - %s[PASSED]%s\n", (int)total_tests, name, EZ_GREEN, EZ_RESET); \
		passed_tests++; \
	} else { \
		printf("%d. %s - %s[FAILED]%s\n", (int)total_tests, name, EZ_RED, EZ_RESET); \
	} \
}

DECLARE_ARRLIST(int);
IMPL_ARRLIST(int);
DECLARE_ARRLIST_NAMED(intPtr, int*);
IMPL_ARRLIST_NAMED(intPtr, int*);

typedef struct {
	int* sum;
	int a;
	int b;
	EZ_MUTEX mutex;
} Params;

void* thread_function(void* params) {
	Params* p = (Params*)params;
	*(p->sum) = p->a + p->b;
	return NULL;
}

void* unsafe_function(void* params) {
	Params* p = (Params*)params;
	EZ_LOCK_MUTEX(p->mutex);
	*(p->sum) = *(p->sum) + 1;
	EZ_RELEASE_MUTEX(p->mutex);
	return NULL;
}

int main() {
	// easybool tests
	EZTEST(TRUE == 1, "Truth bool");
	EZTEST(FALSE == 0, "False bool");
	EZTEST(sizeof(BOOL) == sizeof(int), "Bool size");
	
	// easymemory tests
	void* obj = EZ_ALLOC(1, 100);
	EZTEST(obj != NULL, "Successful allocation");
	EZTEST(EZ_ALLOCATED() == 100 + sizeof(size_t), "Allocated size");
	EZ_FREE(obj);
	EZTEST(EZ_ALLOCATED() == 0, "Free memory");
	obj = EZ_ALLOC(2, 100);
	EZTEST(EZ_ALLOCATED() == 200 + sizeof(size_t), "Multiplicative allocation");
	void* obj2 = EZ_ALLOC(2, 100);
	memset(obj2, 0, 200);
	EZTEST(memcmp(obj, obj2, 200) == 0, "Preset to zero");
	EZ_FREE(obj);
	EZ_FREE(obj2);

	// easyobjects tests
	size_t before_eo_tests = EZ_ALLOCATED();
	ARRLIST_int list = { 0 };
	EZTEST(list.size == 0, "Empty list");
	EZTEST(list.maxsize == 0, "Empty list capacity");
	ARRLIST_int_add(&list, 2);
	ARRLIST_int_add(&list, 4);
	ARRLIST_int_add(&list, 6);
	ARRLIST_int_add(&list, 8);
	ARRLIST_int_add(&list, 10);
	EZTEST(list.size == 5, "Filled list");
	EZTEST(list.maxsize == 8, "List capacity");
	EZTEST(ARRLIST_int_has(&list, 10), "List contains");
	EZTEST(!ARRLIST_int_has(&list, 5), "List does not contains");
	ARRLIST_int_remove(&list, 4);
	EZTEST(!ARRLIST_int_has(&list, 10), "List removal");
	EZTEST(list.size == 4, "List removed size");
	EZTEST(list.maxsize == 8, "List maintained capacity");
	EZTEST(ARRLIST_int_get(&list, 2) == 6, "List get");
	ARRLIST_int_clear(&list);
	EZTEST(list.size == 0, "List clear size");
	EZTEST(list.maxsize == 0, "List clear capacity");
	ARRLIST_intPtr nlist = { 0 };
	EZTEST(nlist.size == 0, "Empty named list");
	EZTEST(nlist.maxsize == 0, "Empty named list capacity");
	int i = 1;
	int j = 3;
	int k = 5;
	int z = 1000;
	ARRLIST_intPtr_add(&nlist, &i);
	ARRLIST_intPtr_add(&nlist, &j);
	ARRLIST_intPtr_add(&nlist, &k);
	EZTEST(nlist.size == 3, "Filled named list");
	EZTEST(nlist.maxsize == 4, "Named list capacity");
	EZTEST(ARRLIST_intPtr_has(&nlist, &j), "Named list contains");
	EZTEST(!ARRLIST_intPtr_has(&nlist, &z), "Named list does not contains");
	ARRLIST_intPtr_remove(&nlist, 1);
	EZTEST(!ARRLIST_intPtr_has(&nlist, &j), "Named list removal");
	EZTEST(nlist.size == 2, "Named list removed size");
	EZTEST(nlist.maxsize == 4, "Named list maintained capacity");
	EZTEST(ARRLIST_intPtr_get(&nlist, 1) == &k, "Named list get");
	ARRLIST_intPtr_clear(&nlist);
	EZTEST(nlist.size == 0, "Named list clear size");
	EZTEST(nlist.maxsize == 0, "Named list clear capacity");
	EZTEST(before_eo_tests == EZ_ALLOCATED(), "EasyObjects memory leak");
	
	// easythreads tests
	int sum = 0;
	EZ_MUTEX mutex;
	EZ_CREATE_MUTEX(mutex);
	Params par = { &sum, 10, 59, mutex };
	EZ_THREAD safe_thread;
	EZ_CREATE_THREAD(safe_thread, thread_function, &par);
	EZ_WAIT_THREAD(safe_thread);
	EZTEST(sum == 69, "Safe thread");
	sum = 0;
	EZ_THREAD unsafe_threads[1234];
	for (int i = 0; i < 1234; i++) {
		EZ_CREATE_THREAD(unsafe_threads[i], unsafe_function, &par);
	}
	for (int i = 0; i < 1234; i++) {
		EZ_WAIT_THREAD(unsafe_threads[i]);
	}
	EZTEST(sum == 1234, "Unsafe threads");
	
	// easynet tests
	// TODO:

	printf("%s%d%s/%d Tests passed\n", 
		passed_tests == total_tests ? EZ_GREEN : EZ_RED, passed_tests, EZ_RESET, total_tests);
	return 0;
}
