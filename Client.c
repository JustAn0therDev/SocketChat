#include <stdlib.h>
#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <processthreadsapi.h>
#include "Structs.h"

#define MAX_MESSAGE_SIZE 2000

void read_message(LPVOID lpParam);

void start_client(char* nickname) {
	SOCKET socket_desc;
	ClientSocketMessage client_socket_message = { 0 };
	struct sockaddr_in server_addr = { 0 };
	int server_struct_length = sizeof(server_addr);

	WSADATA wsaData = { 0 };

	// Initializes the socket DLL.
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (iResult != 0) {
		wprintf(L"WSAStartup failed: %d\n", iResult);
		return;
	}

	socket_desc = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (socket_desc == INVALID_SOCKET) {
		printf("Error while creating socket\n");
		return;
	}

	printf("Socket created successfully\n");

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(2000);
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	ReadMessageInput rmi = { socket_desc, (struct sockaddr*)&server_addr, server_struct_length };

	DWORD dword;

	HANDLE handle = CreateThread(
	NULL,											// default security attributes
	0,												// use default stack size  
	(LPTHREAD_START_ROUTINE)read_message,           // thread function name
	&rmi,											// argument to thread function 
	0,												// use default creation flags 
	&dword);										// returns the thread identifier

	strcpy(client_socket_message.nickname, nickname);

	while (1) {
		// Is this REALLY the BEST way to read input though?
		fgets(client_socket_message.text_message, MAX_MESSAGE_SIZE, stdin);
		
		// char == 1 byte
		if (sendto(socket_desc, (unsigned char*)&client_socket_message, sizeof(client_socket_message), 0,
			(struct sockaddr*)&server_addr, server_struct_length) < 0) {
			printf("Unable to send message\n");
			return -1;
		}
	}
}

void read_message(LPVOID lpParam) {
	ReadMessageInput* rmi = (ReadMessageInput*)lpParam;

	while (1) {
		char server_message_buffer[MAX_MESSAGE_SIZE] = { 0 };

		// Receive the server's response:
		if (recvfrom(rmi->socket_desc, server_message_buffer, sizeof(server_message_buffer), 0,
			rmi->server_addr, &rmi->server_struct_length) < 0) {
			printf("Error while receiving server's msg: %d\n", WSAGetLastError());
		}

		printf("%s\n", server_message_buffer);
	}
}