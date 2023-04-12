#include <stdlib.h>
#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include "Structs.h"

#define MAX_MESSAGE_SIZE 2000
#define MAX_CLIENTS 10

int contains(SocketClient* clients, struct sockaddr_in* current_address);
inline int is_same_address(struct sockaddr_in* first, struct sockaddr_in* second);

int start_server() {
	SOCKET socket_desc;
	struct sockaddr_in server_addr = { 0 }, client_addr = { 0 };
	int client_struct_length = sizeof(client_addr);
	SocketClient clients[MAX_CLIENTS] = { 0 };
	int clientsIdx = 0;

	WSADATA wsaData = { 0 };

	// Initializes the socket DLL.
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (iResult != 0) {
		wprintf(L"WSAStartup failed: %d\n", iResult);
		return 1;
	}

	socket_desc = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (socket_desc == INVALID_SOCKET) {
		printf("Error while creating socket: %d\n", WSAGetLastError());
		return -1;
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(2000);
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	if (bind(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
		printf("Couldn't bind to the port\n");
		return -1;
	}

	printf("Binded.\n");

	printf("Listening for incoming messages...\n");

	while (1) {
		ClientSocketMessage client_socket_message = { 0 };
		char client_message[2000] = { 0 };

		if (recvfrom(socket_desc, client_message, sizeof(client_socket_message), 0,
			(struct sockaddr*)&client_addr, &client_struct_length) < 0) {
			printf("Couldn't receive\n");
			return -1;
		}

		client_socket_message = *((ClientSocketMessage*)client_message);

		if (contains(clients, &client_addr) == 0) {
			clients[clientsIdx] = (SocketClient){ &client_addr };
			strcpy(clients[clientsIdx].nickname, client_socket_message.nickname);
			clientsIdx++;
		}

		printf("Client %s: %s\n", client_socket_message.nickname, client_socket_message.text_message);

		char message_to_client_buffer[MAX_MESSAGE_SIZE] = { 0 };
		sprintf_s(message_to_client_buffer, MAX_MESSAGE_SIZE, "%s: %s", client_socket_message.nickname, client_socket_message.text_message);

		for (int i = 0; i < MAX_CLIENTS; i++) {
			if (clients[i].client_addr == 0) {
				continue;
			}

			if (is_same_address(clients[i].client_addr, &client_addr) == 0) {
				if (sendto(socket_desc, message_to_client_buffer, (int)strlen(message_to_client_buffer), 0,
					(struct sockaddr*)clients[i].client_addr, client_struct_length) < 0) {
					printf("Couldn't send message to: %s (%s:%d)\n", clients[i].nickname, inet_ntoa(clients[i].client_addr->sin_addr), ntohs(clients[i].client_addr->sin_port));
					continue;
				}
			}
			else {
				char sameMessengerBuffer[MAX_MESSAGE_SIZE] = { 0 };
				sprintf_s(sameMessengerBuffer, MAX_MESSAGE_SIZE, "You: %s", client_message);

				if (sendto(socket_desc, sameMessengerBuffer, (int)strlen(sameMessengerBuffer), 0,
					(struct sockaddr*)clients[i].client_addr, client_struct_length) < 0) {
					printf("Couldn't send message to: %s (%s:%d)\n", clients[i].nickname, inet_ntoa(clients[i].client_addr->sin_addr), ntohs(clients[i].client_addr->sin_port));
					continue;
				}
			}

			printf("Sent message to: %s (%s:%d)\n", clients[i].nickname, inet_ntoa(clients[i].client_addr->sin_addr), ntohs(clients[i].client_addr->sin_port));
		}
	}

	return 0;
}

int contains(SocketClient* clients, struct sockaddr_in* current_address) {
	for (int i = 0; i < MAX_CLIENTS; i++) {
		if (clients[i].client_addr != 0 && is_same_address(clients[i].client_addr, current_address)) {
			return 1;
		}
	}

	return 0;
}

int is_same_address(struct sockaddr_in* first, struct sockaddr_in* second) {
	return ntohs(first->sin_port) == ntohs(second->sin_port) &&
		strcmp(inet_ntoa(first->sin_addr), inet_ntoa(second->sin_addr)) == 0;
}