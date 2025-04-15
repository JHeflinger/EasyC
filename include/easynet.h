#ifndef EASYNET_H
#define EASYNET_H

#include "easybool.h"
#include <stddef.h>
#include <stdint.h>

typedef uint8_t EZ_BYTE;

typedef struct {
	EZ_BYTE* bytes;
	size_t max_length;
	size_t current_length;
} EZ_BUFFER;

EZ_BUFFER* ez_generate_buffer(size_t size);
void ez_clean_buffer(EZ_BUFFER* buffer);
void ez_translate_buffer(EZ_BUFFER* buffer, void* destination);

#define EZ_GENERATE_BUFFER(size) ez_generate_buffer(size)
#define EZ_CLEAN_BUFFER(buffer) ez_clean_buffer(buffer)
#define EZ_TRANSLATE_BUFFER(buffer, destination) ez_translate_buffer(buffer, (void*)destination)

#ifdef __linux__

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>

typedef int EZ_SOCKET;

#define EZ_INVALID_SOCK -1
#define EZ_OPT_TYPE int
#define EZ_CLOSE_SOCKET(...) close(__VA_ARGS__)

#elif _WIN32

// basic cleanup to avoid windows lib bloat
#define WIN32_LEAN_AND_MEAN
#define NOGDICAPMASKS     // CC_*, LC_*, PC_*, CP_*, TC_*, RC_
#define NOVIRTUALKEYCODES // VK_*
#define NOWINMESSAGES     // WM_*, EM_*, LB_*, CB_*
#define NOWINSTYLES       // WS_*, CS_*, ES_*, LBS_*, SBS_*, CBS_*
#define NOSYSMETRICS      // SM_*
#define NOMENUS           // MF_*
#define NOICONS           // IDI_*
#define NOKEYSTATES       // MK_*
#define NOSYSCOMMANDS     // SC_*
#define NORASTEROPS       // Binary and Tertiary raster ops
#define NOSHOWWINDOW      // SW_*
#define OEMRESOURCE       // OEM Resource values
#define NOATOM            // Atom Manager routines
#define NOCLIPBOARD       // Clipboard routines
#define NOCOLOR           // Screen colors
#define NOCTLMGR          // Control and Dialog routines
#define NODRAWTEXT        // DrawText() and DT_*
#define NOGDI             // All GDI defines and routines
#define NOKERNEL          // All KERNEL defines and routines
#define NOUSER            // All USER defines and routines
#define NOMB              // MB_* and MessageBox()
#define NOMEMMGR          // GMEM_*, LMEM_*, GHND, LHND, associated routines
#define NOMETAFILE        // typedef METAFILEPICT
#define NOMSG             // typedef MSG and associated routines
#define NOOPENFILE        // OpenFile(), OemToAnsi, AnsiToOem, and OF_*
#define NOSCROLL          // SB_* and scrolling routines
#define NOSERVICE         // All Service Controller routines, SERVICE_ equates, etc.
#define NOSOUND           // Sound driver routines
#define NOTEXTMETRIC      // typedef TEXTMETRIC and associated routines
#define NOWH              // SetWindowsHook and WH_*
#define NOWINOFFSETS      // GWL_*, GCL_*, associated routines
#define NOCOMM            // COMM driver routines
#define NOKANJI           // Kanji support stuff.
#define NOHELP            // Help engine interface.
#define NOPROFILER        // Profiler interface.
#define NODEFERWINDOWPOS  // DeferWindowPos routines
#define NOMCX             // Modem Configuration Extensions

#include <stdlib.h>
#include <winsock2.h>

typedef SOCKET EZ_SOCKET;

#define EZ_INVALID_SOCK INVALID_SOCKET
#define EZ_OPT_TYPE char
#define EZ_CLOSE_SOCKET(...) closesocket(__VA_ARGS__)

#else
#error Unsupported operating system detected!
#endif

BOOL ez_init_network();
BOOL ez_clean_network();
BOOL ez_check_network();

#define EZ_INIT_NETWORK() ez_init_network()
#define EZ_CLEAN_NETWORK() ez_clean_network()
#define EZ_CHECK_NETWORK() ez_check_network()

typedef struct {
	uint16_t port;
	EZ_SOCKET socket;
	BOOL open;
} ez_Server;

ez_Server* ez_generate_server();
BOOL ez_open_server(ez_Server* server, uint16_t port);
BOOL ez_close_server(ez_Server* server);
BOOL ez_clean_server(ez_Server* server);
BOOL ez_server_ask(ez_Server* server, EZ_BUFFER* buffer);
BOOL ez_server_recieve(ez_Server* server, EZ_BUFFER* buffer);
BOOL ez_server_send(ez_Server* server, EZ_BUFFER* buffer);

#define EZ_GENERATE_SERVER() ez_generate_server()
#define EZ_OPEN_SERVER(server, port) ez_open_server(server, port)
#define EZ_CLOSE_SERVER(server) ez_close_server(server)
#define EZ_CLEAN_SERVER(server) ez_clean_server(server)
#define EZ_SERVER_ASK(server, buffer) ez_server_ask(server, buffer)
#define EZ_SERVER_RECIEVE(server, buffer) ez_server_recieve(server, buffer)
#define EZ_SERVER_SEND(server, buffer) ez_server_send(server, buffer)

typedef struct {
	uint8_t address[4];
} Ipv4;

typedef struct {
	Ipv4 address;
	uint16_t port;
	EZ_SOCKET socket;
	BOOL open;
} ez_Client;

ez_Client* ez_generate_client();
BOOL ez_connect_client(ez_Client* client, Ipv4 address, uint16_t port);
BOOL ez_disconnect_client(ez_Client* client);
BOOL ez_clean_client(ez_Client* client);
BOOL ez_client_ask(ez_Client* client, EZ_BUFFER* buffer);
BOOL ez_client_recieve(ez_Client* client, EZ_BUFFER* buffer);
BOOL ez_client_send(ez_Client* client, EZ_BUFFER* buffer);

#define EZ_GENERATE_CLIENT() ez_generate_client()
#define EZ_CONNECT_CLIENT(client, address, port) ez_connect_client(client, address, port)
#define EZ_DISCONNECT_CLIENT(client) ez_disconnect_client(client)
#define EZ_CLEAN_CLIENT(client) ez_clean_client(client)
#define EZ_CLIENT_ASK(client, buffer) ez_client_ask(client, buffer)
#define EZ_CLIENT_RECIEVE(client, buffer) ez_client_recieve(client, buffer)
#define EZ_CLIENT_SEND(client, buffer) ez_client_send(client, buffer)

typedef struct {
	ez_Client* client;
	void* next;
} ez_ClientList;

typedef struct {
	ez_Server* server;
	void* next;
} ez_ServerList;

#endif
