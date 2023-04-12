#pragma once
#include <winsock.h>

typedef struct {
	char text_message[2000];
	char nickname[150];
} ClientSocketMessage;

typedef struct {
	struct sockaddr_in client_addr;
	char nickname[150];
} SocketClient;

typedef struct {
	SOCKET socket_desc;
	struct sockaddr* server_addr;
	int server_struct_length;
} ReadMessageInput;