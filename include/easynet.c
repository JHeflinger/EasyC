#include "easynet.h"
#include "easylogger.h"
#include "easymemory.h"
#include <string.h>

BOOL s_ez_network_initialized = FALSE;
ez_ClientList* s_ez_client_list = NULL;
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

BOOL ez_translate_buffer(EZ_BUFFER* buffer, void* destination, size_t destsize) {
	if (buffer->current_length > destsize) {
		EZ_ERROR("Cannot translate buffer to a smaller size destination");
		return FALSE;
	}
	memcpy(destination, buffer->bytes, buffer->current_length);
	return TRUE;
}

BOOL ez_record_buffer(EZ_BUFFER* buffer, void* source, size_t sourcesize) {
	if (buffer->max_length < sourcesize) {
		EZ_ERROR("Cannot translate source to a smaller size buffer");
		return FALSE;
	}
	memcpy(buffer->bytes, source, sourcesize);
	return TRUE;
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
	while (s_ez_server_list != NULL) {
		BOOL res = ez_clean_server(s_ez_server_list->server);
		if (!res) {
			EZ_ERROR("Unable to clean servers");
			return FALSE;
		}
	}
	while (s_ez_client_list != NULL) {
		BOOL res = ez_clean_client(s_ez_client_list->client);
		if (!res) {
			EZ_ERROR("Unable to clean clients");
			return FALSE;
		}
	}
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
	if (s_ez_server_list == NULL) {
		s_ez_server_list = EZ_ALLOC(1, sizeof(ez_ServerList));
		s_ez_server_list->server = server;
		s_ez_server_list->next = NULL;
	} else {
		ez_ServerList* t = s_ez_server_list;
		s_ez_server_list = EZ_ALLOC(1, sizeof(ez_ServerList));
		s_ez_server_list->server = server;
		s_ez_server_list->next = t;
	}
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
	EZ_CLOSE_SOCKET(server->socket);
	server->open = FALSE;
	return TRUE;
}

BOOL ez_clean_server(ez_Server* server) {
	if (server == NULL) {
		EZ_ERROR("Cannot clean a null pointer");
		return FALSE;
	}
	ez_ServerList* tracker = s_ez_server_list;
	if (tracker->server == server) {
		s_ez_server_list = tracker->next;
		if (tracker->server->open) ez_close_server(tracker->server);
		EZ_FREE(tracker->server);
		EZ_FREE(tracker);
	} else {
		while (tracker->next != NULL) {
			if (((ez_ServerList*)tracker->next)->server == server) {
				ez_ServerList* dead = tracker->next;
				tracker->next = ((ez_ServerList*)tracker->next)->next;
				if (dead->server->open) ez_close_server(dead->server);
				EZ_FREE(dead->server);
				EZ_FREE(dead);
				return TRUE;
			}
			tracker = tracker->next;
		}
		EZ_WARN("Unable to clean an untracked server object - please use the dedicated EZ_GENERATE_SERVER() function to create server objects");
		return FALSE;
	}
	return TRUE;
}

BOOL ez_server_ask(ez_Server* server, EZ_BUFFER* buffer) {
	ez_check_network();
	#ifdef _WIN32
    unsigned long l;
    ioctlsocket(server->socket, FIONREAD, &l);
    if (l > 0) {
		int64_t recbytes = recv(server->socket, (char*)buffer->bytes, buffer->max_length, 0);
		if (recbytes < 0) {
			EZ_WARN("An error occured while recieving data");
			buffer->current_length = (size_t)-1;
			return FALSE;
		}
		if (recbytes == 0) return FALSE;
		buffer->current_length = (size_t)recbytes;
		return TRUE;
	}
	return FALSE;
    #elif __linux__	
	ssize_t retval = recv(server->socket, (char*)buffer->bytes, buffer->max_length, MSG_DONTWAIT);
	if (retval < 0) {
		EZ_WARN("An error occured while recieving data");
		buffer->current_length = (size_t)-1;
		return FALSE;
	}
	if (retval == 0) return FALSE;
	buffer->current_length = (size_t)retval;
	return TRUE;
	#else
    #error "Unsupported operating system detected!"
    #endif
}

BOOL ez_server_recieve(ez_Server* server, EZ_BUFFER* buffer) {
	ez_check_network();
	int64_t retval = recv(server->socket, (char*)buffer->bytes, buffer->max_length, 0);
    if (retval < 0) {
        EZ_WARN("An error occured while recieving data");
        return FALSE;
    }
	if (retval == 0) return FALSE;
    buffer->current_length = (size_t)retval;
	return TRUE;
}

BOOL ez_server_send(ez_Server* server, EZ_BUFFER* buffer) {
	ez_check_network();
	#ifdef __linux__
    int64_t sent = send(server->socket, (char*)buffer->bytes, buffer->current_length, MSG_NOSIGNAL);
	#else 
	int64_t sent = send(server->socket, (char*)buffer->bytes, buffer->current_length, 0);
	#endif
    if (sent < 0) {
        EZ_WARN("An error occured while sending data");
        return FALSE;
    }
	return TRUE;
}

ez_Client* ez_generate_client() {
	ez_check_network();
	ez_Client* client = EZ_ALLOC(1, sizeof(ez_Client));
	client->open = FALSE;
	if (s_ez_client_list == NULL) {
		s_ez_client_list = EZ_ALLOC(1, sizeof(ez_ClientList));
		s_ez_client_list->client = client;
		s_ez_client_list->next = NULL;
	} else {
		ez_ClientList* t = s_ez_client_list;
		s_ez_client_list = EZ_ALLOC(1, sizeof(ez_ClientList));
		s_ez_client_list->client = client;
		s_ez_client_list->next = t;
	}
	return client;
}

BOOL ez_connect_client(ez_Client* client, Ipv4 address, uint16_t port) {
	ez_check_network();
	if (client->open) {
		EZ_ERROR("Unable to connect a client that's already connected");
		return FALSE;
	}
	if (port < 1024) {
		EZ_ERROR("Unable to connect to reserved port \"%d\"", (int)port);
		return FALSE;
	}
	client->port = port;
	client->socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client->socket == EZ_INVALID_SOCK) {
		EZ_ERROR("Unable to create a new socket");
		return FALSE;
	}
	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons((u_short)(client->port));
	memcpy(&client->address, &address, sizeof(Ipv4));
	char addrstr[64];
	sprintf(addrstr, "%d.%d.%d.%d",
		 client->address.address[0],
		 client->address.address[1],
		 client->address.address[2],
		 client->address.address[3]);
	if (inet_pton(AF_INET, addrstr, &serverAddr.sin_addr) <= 0) {
		EZ_ERROR("Invalid IP address, unable to connect");
		EZ_CLOSE_SOCKET(client->socket);
		return FALSE;
	}
	if (connect(client->socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
		EZ_ERROR("Unable to connect configured client to server");
		EZ_CLOSE_SOCKET(client->socket);
		return FALSE;
	}
	return TRUE;
}

BOOL ez_disconnect_client(ez_Client* client) {
	ez_check_network();
	if (!client->open) {
		EZ_ERROR("Cannot disconnect a client that isn't already connected");
		return FALSE;
	}
	EZ_CLOSE_SOCKET(client->socket);
	client->open = FALSE;
	return TRUE;
}

BOOL ez_clean_client(ez_Client* client) {
	if (client == NULL) {
		EZ_ERROR("Cannot clean a null pointer");
		return FALSE;
	}
	ez_ClientList* tracker = s_ez_client_list;
	if (tracker->client == client) {
		s_ez_client_list = tracker->next;
		if (tracker->client->open) ez_disconnect_client(tracker->client);
		EZ_FREE(tracker->client);
		EZ_FREE(tracker);
	} else {
		while (tracker->next != NULL) {
			if (((ez_ClientList*)tracker->next)->client == client) {
				ez_ClientList* dead = tracker->next;
				tracker->next = ((ez_ClientList*)tracker->next)->next;
				if (dead->client->open) ez_disconnect_client(dead->client);
				EZ_FREE(dead->client);
				EZ_FREE(dead);
				return TRUE;
			}
			tracker = tracker->next;
		}
		EZ_WARN("Unable to clean an untracked client object - please use the dedicated EZ_GENERATE_CLIENT() function to create client objects");
		return FALSE;
	}
	return TRUE;
}

BOOL ez_client_ask(ez_Client* client, EZ_BUFFER* buffer) {
	ez_check_network();
	#ifdef _WIN32
    unsigned long l;
    ioctlsocket(client->socket, FIONREAD, &l);
    if (l > 0) {
		int64_t recbytes = recv(client->socket, (char*)buffer->bytes, buffer->max_length, 0);
		if (recbytes < 0) {
			EZ_WARN("An error occured while recieving data");
			buffer->current_length = (size_t)-1;
			return FALSE;
		}
		if (recbytes == 0) return FALSE;
		buffer->current_length = (size_t)recbytes;
		return TRUE;
	}
	return FALSE;
    #elif __linux__	
	ssize_t retval = recv(client->socket, (char*)buffer->bytes, buffer->max_length, MSG_DONTWAIT);
	if (retval < 0) {
		EZ_WARN("An error occured while recieving data");
		buffer->current_length = (size_t)-1;
		return FALSE;
	}
	if (retval == 0) return FALSE;
	buffer->current_length = (size_t)retval;
	return TRUE;
	#else
    #error "Unsupported operating system detected!"
    #endif
}

BOOL ez_client_recieve(ez_Client* client, EZ_BUFFER* buffer) {
	ez_check_network();
	int64_t retval = recv(client->socket, (char*)buffer->bytes, buffer->max_length, 0);
    if (retval < 0) {
        EZ_WARN("An error occured while recieving data");
        return FALSE;
    }
	if (retval == 0) return FALSE;
    buffer->current_length = (size_t)retval;
	return TRUE;
}

BOOL ez_client_send(ez_Client* client, EZ_BUFFER* buffer) {
	ez_check_network();
	#ifdef __linux__
    int64_t sent = send(client->socket, (char*)buffer->bytes, buffer->current_length, MSG_NOSIGNAL);
	#else 
	int64_t sent = send(client->socket, (char*)buffer->bytes, buffer->current_length, 0);
	#endif
    if (sent < 0) {
        EZ_WARN("An error occured while sending data");
        return FALSE;
    }
	return TRUE;
}
