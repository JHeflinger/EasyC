#include <easyc.h>
#include <math.h>

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

DECLARE_ARRLIST_NAMED(intPtr, int*);
IMPL_ARRLIST_NAMED(intPtr, int*);

typedef struct {
	int* sum;
	int a;
	int b;
	EZ_MUTEX mutex;
} Params;

typedef struct {
	int32_t a;
	char b;
	float c;
	uint32_t d;
} Packet;

EZ_THREAD_RETURN_TYPE thread_function(EZ_THREAD_PARAMETER_TYPE params) {
	Params* p = (Params*)params;
	*(p->sum) = p->a + p->b;
	return 0;
}

EZ_THREAD_RETURN_TYPE unsafe_function(EZ_THREAD_PARAMETER_TYPE params) {
	Params* p = (Params*)params;
	EZ_LOCK_MUTEX(p->mutex);
	*(p->sum) = *(p->sum) + 1;
	EZ_RELEASE_MUTEX(p->mutex);
	return 0;
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
	ARRLIST_int_add(&list, 6);
	ARRLIST_int_add(&list, 10);
	ARRLIST_int_insert(&list, 4, 1);
	ARRLIST_int_insert(&list, 8, 3);
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
	size_t en_allocated = EZ_ALLOCATED();
	ez_Buffer* buf = EZ_GENERATE_BUFFER(1024);
	Packet packet = { -123, 'z', 69.0f, UINT32_MAX };
	EZTEST(buf->max_length == 1024, "Generate buffer max size");
	EZTEST(buf->current_length == 0, "Empty buffer");
	EZTEST(EZ_RECORD_BUFFER(buf, &packet), "Record buffer success");
	EZTEST(buf->current_length == sizeof(Packet), "Record buffer size");
	int count = 1023;
	while (buf->bytes[count] == 0) {
		count--;
	}
	EZTEST(count + 1 == sizeof(Packet), "Real buffer size");
	Packet affirm = { 0 };
	EZTEST(EZ_TRANSLATE_BUFFER(buf, &affirm), "Translate buffer success");
	EZTEST(affirm.a == -123 && affirm.b == 'z' && affirm.c == 69.0f && affirm.d == UINT32_MAX, "Translate buffer data");
	EZTEST(EZ_INIT_NETWORK(), "Initialize network");
	ez_Server* server = EZ_GENERATE_SERVER();
	ez_Client* client = EZ_GENERATE_CLIENT();
	Ipv4 address = {{127, 0, 0, 1}};
	ez_Buffer* retbuf = EZ_GENERATE_BUFFER(1024);
	EZTEST(EZ_OPEN_SERVER(server, 55000), "Open server");
	EZTEST(EZ_CONNECT_CLIENT(client, address, 55000), "Connect client");
	ez_Connection* connection = EZ_SERVER_ACCEPT(server);
	EZTEST(connection != NULL, "Accepted client");
	EZTEST(!EZ_SERVER_ASK(connection, retbuf), "Non blocking server recieve failure");
	EZTEST(EZ_CLIENT_SEND(client, buf), "Client send");
	EZTEST(EZ_SERVER_ASK(connection, retbuf), "Non blocking server recieve success");
	EZTEST(memcmp(buf->bytes, retbuf->bytes, buf->max_length) == 0, "Successful byte transfer 1");
	memset(retbuf->bytes, 0, retbuf->max_length);
	EZ_CLIENT_SEND(client, buf);
	EZTEST(EZ_SERVER_RECIEVE(connection, retbuf), "Blocking server recieve");
	EZTEST(memcmp(buf->bytes, retbuf->bytes, buf->max_length) == 0, "Successful byte transfer 2");
	memset(retbuf->bytes, 0, retbuf->max_length);
	EZTEST(!EZ_CLIENT_ASK(client, retbuf), "Non blocking client recieve failure");
	EZTEST(EZ_SERVER_SEND(connection, buf), "Server send");
	EZTEST(EZ_CLIENT_ASK(client, retbuf), "Non blocking client recieve success");
	EZTEST(memcmp(buf->bytes, retbuf->bytes, buf->max_length) == 0, "Successful byte transfer 3");
	memset(retbuf->bytes, 0, retbuf->max_length);
	EZ_SERVER_SEND(connection, buf);
	EZTEST(EZ_CLIENT_RECIEVE(client, retbuf), "Blocking client recieve");
	EZTEST(memcmp(buf->bytes, retbuf->bytes, buf->max_length) == 0, "Successful byte transfer 4");
	EZTEST(EZ_CLOSE_CONNECTION(connection), "Close connection");
	EZTEST(EZ_DISCONNECT_CLIENT(client), "Disconnect client");
	EZTEST(EZ_CLOSE_SERVER(server), "Close server");
	EZTEST(EZ_CLEAN_NETWORK(), "Clean network");
	EZ_CLEAN_BUFFER(buf);
	EZ_CLEAN_BUFFER(retbuf);
	EZTEST(en_allocated == EZ_ALLOCATED(), "EasyNet memory leak");

	// easymath tests
	EZTEST(EZ_CLAMP(-99, -60, 100) == -60, "EasyMath clamp to min");
	EZTEST(EZ_CLAMP(999.0, 0.0, 100.0) == 100.0, "EasyMath clamp to max");
	EZTEST(EZ_CLAMP(100.0f, 0.0f, 200.0f) == 100.0f, "EasyMath clamp to none");
	EZTEST(EZ_DISTANCE(0, 25, 0, 0) == 25.0f, "EasyMath distance 1D");
	EZTEST(EZ_DISTANCE(3, 3, -3, -3) == (float)sqrt(72), "EasyMath distance square");
	EZTEST(EZ_DISTANCE(1, 10, 3, 4) == (float)sqrt(40), "EasyMath distance 2D");

	printf("\nTest suite results: %s%d%s/%d tests passed\n", 
		passed_tests == total_tests ? EZ_GREEN : EZ_RED, passed_tests, EZ_RESET, total_tests);
	return 0;
}
