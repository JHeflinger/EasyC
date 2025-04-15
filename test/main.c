#include <easyc.h>

int passed_tests = 0;
int total_tests = 0;

#define EZTEST(condition, name) { \
	total_tests++; \
	if (condition) { \
		printf("%d. \"%s\" - %s[PASSED]%s\n", (int)total_tests, name, EZ_GREEN, EZ_RESET); \
		passed_tests++; \
	} else { \
		printf("%d. \"%s\" - %s[FAILED]%s\n", (int)total_tests, name, EZ_RED, EZ_RESET); \
	} \
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
	// TODO:
	
	// easythreads tests
	// TODO:
	
	// easynet tests
	// TODO:

	printf("%s%d%s/%d Tests passed\n", 
		passed_tests == total_tests ? EZ_GREEN : EZ_RED, passed_tests, EZ_RESET, total_tests);
	return 0;
}
