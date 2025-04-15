#include "easynet.h"
#include "easylogger.h"
#include "easymemory.h"
#include <string.h>

BOOL s_ez_network_initialized = FALSE;
ez_ClientList* s_ez_client_list = NULL; //TODO: integrate these
ez_ServerList* s_ez_server_list = NULL;

EZ_BUFFER* ez_generate_buffer(size_t size) {
	EZ_BUFFER* buffer = EZ_ALLOC(1, sizeof(EZ_BUFFER));
	buffer->bytes = EZ_ALLOC(size, sizeof(EZ_BYTE));
	buffer->max_length = size;
	buffer->current_length = 0;
	return buffer;
}

void ez_clean_buffer(EZ_BUFFER* buffer) {
	if (buffer != NULL) {
		if (buffer->bytes != NULL) {
			EZ_FREE(buffer->bytes);
			EZ_FREE(buffer);
		} else {
			EZ_WARN("Unable to clean empty bytes");
		}
	} else {
		EZ_WARN("Unable to clean a null pointer");
	}
}

void ez_translate_buffer(EZ_BUFFER* buffer, void* destination) {
	memcpy(destination, buffer->bytes, buffer->current_length);
}

BOOL ez_init_network() {
	#ifdef __WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        EZ_ERROR("Failed to initialize Winsock");
    }
	return FALSE;
	#elifndef __linux__
	#error "Unsupported operating system detected"
	#endif
	s_ez_network_initialized = TRUE;
	return TRUE;
}

BOOL ez_clean_network() {
	#ifdef __WIN32
    WSACleanup();
	#elifndef __linux__
	#error "Unsupported operating system detected"
	#endif
	s_ez_network_initialized = FALSE;
	return TRUE;
}

BOOL ez_check_network() {
	if (!s_ez_network_initialized) {
		EZ_WARN("EasyNet was not intialized. Please initialize it using EZ_INIT_NETWORK(). Implicitly intializing now...");
		return ez_init_network();
	}
	return TRUE;
}

ez_Server* ez_generate_server() {
	ez_check_network();
	ez_Server* server = EZ_ALLOC(1, sizeof(ez_Server));
	server->port = 0;
	server->socket = EZ_INVALID_SOCK;
	server->open = FALSE;
	return server;
}

BOOL ez_open_server(ez_Server* server, uint16_t port) {
	ez_check_network();
	if (server->open) {
		EZ_ERROR("Unable to open a server that's already open");
		return FALSE;
	}
	if (port < 1024) {
		EZ_ERROR("Unable to open on reserved port \"%d\"", (int)port);
		return FALSE;
	}
	server->port = port;
	server->socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server->socket == EZ_INVALID_SOCK) {
		EZ_ERROR("Unable to create a new socket");
		return FALSE;
	}
	EZ_OPT_TYPE optval = 1;
    if (setsockopt(server->socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
        EZ_ERROR("Unable to set server socket options");
        EZ_CLOSE_SOCKET(server->socket);
		return FALSE;
    }
    struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons((u_short)(server->port));
    if (bind(server->socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == EZ_INVALID_SOCK) {
        EZ_ERROR("Unable to bind server socket");
        EZ_CLOSE_SOCKET(server->socket);
        return FALSE;
    }
    if (listen(server->socket, (SOMAXCONN) == EZ_INVALID_SOCK)) {
        EZ_ERROR("Unable to listen for connections");
        EZ_CLOSE_SOCKET(server->socket);
        return FALSE;
    }
    server->open = TRUE;
	return TRUE;
}

BOOL ez_close_server(ez_Server* server) {
	ez_check_network();
	if (!server->open) {
		EZ_ERROR("Cannot close a server that isn't already open");
		return FALSE;
	}
	// TODO: close properly by going through tracked servers
	server->open = FALSE;
	return TRUE;
}

BOOL ez_clean_server(ez_Server* server) {
	if (server == NULL) {
		EZ_ERROR("Cannot clean a null pointer");
		return FALSE;
	}
	// TODO: clean properly by going through tracked servers
	return TRUE;
}

BOOL ez_server_ask(ez_Server* server, EZ_BUFFER* buffer) {
	ez_check_network();
	return TRUE;
}

BOOL ez_server_recieve(ez_Server* server, EZ_BUFFER* buffer) {
	ez_check_network();
	return TRUE;
}

BOOL ez_server_send(ez_Server* server, EZ_BUFFER* buffer) {
	ez_check_network();
	return TRUE;
}

ez_Client* ez_generate_client() {
	ez_check_network();
	return NULL;
}

BOOL ez_connect_client(ez_Client* client, Ipv4 address, uint16_t port) {
	ez_check_network();
	return TRUE;
}

BOOL ez_disconnect_client(ez_Client* client) {
	ez_check_network();
	return TRUE;
}

BOOL ez_clean_client(ez_Client* client) {
	return TRUE;
}

BOOL ez_client_ask(ez_Client* client, EZ_BUFFER* buffer) {
	ez_check_network();
	return TRUE;
}

BOOL ez_client_recieve(ez_Client* client, EZ_BUFFER* buffer) {
	ez_check_network();
	return TRUE;
}

BOOL ez_client_send(ez_Client* client, EZ_BUFFER* buffer) {
	ez_check_network();
	return TRUE;
}
