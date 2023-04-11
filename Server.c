#include <stdlib.h>
#include <winsock2.h>
#include <windows.h>
#include <stdio.h>

#define MAX_MESSAGE_SIZE 2000

int start_server() {
	int socket_desc;
	struct sockaddr_in server_addr = { 0 }, client_addr = { 0 };
	int client_struct_length = sizeof(client_addr);
	struct sockaddr_in addresses[10] = { 0 };
	int addressesIdx = 0;

	WSADATA wsaData = { 0 };

	// Initializes the socket DLL.
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (iResult != 0) {
		wprintf(L"WSAStartup failed: %d\n", iResult);
		return 1;
	}

	// Create UDP socket:
	socket_desc = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (socket_desc == INVALID_SOCKET) {
		printf(WSAGetLastError());
		printf("Error while creating socket\n");
		return -1;
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(2000);
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	if (bind(socket_desc, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
		printf("Couldn't bind to the port\n");
		return -1;
	}

	printf("Done with binding\n");

	printf("Listening for incoming messages...\n\n");

	while (1) {
		char client_message[2000];
		memset(client_message, '\0', sizeof(client_message));

		if (recvfrom(socket_desc, client_message, sizeof(client_message), 0,
			(struct sockaddr*)&client_addr, &client_struct_length) < 0) {
			printf("Couldn't receive\n");
			return -1;
		}

		if (contains(addresses, 10, &client_addr) == 0) {
			addresses[addressesIdx] = client_addr;
			addressesIdx++;
		}

		// printf("Received message from IP: %s and port: %i\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

		printf("Client %d: %s\n", ntohs(client_addr.sin_port), client_message);

		char buffer[MAX_MESSAGE_SIZE] = { 0 };
		sprintf_s(buffer, MAX_MESSAGE_SIZE, "%d: %s", ntohs(client_addr.sin_port), client_message);

		for (int i = 0; i < 10; i++) {
			if (&addresses[i] == 0) {
				continue;
			}

			if (is_same_address(&addresses[i], &client_addr) == 0) {
				if (sendto(socket_desc, buffer, strlen(buffer), 0,
					(struct sockaddr*)&addresses[i], client_struct_length) < 0) {
					printf("Couldn't send message to: %s:%d\n", inet_ntoa(addresses[i].sin_addr), ntohs(addresses[i].sin_port));
					continue;
				}
			}
			else {
				char sameMessengerBuffer[MAX_MESSAGE_SIZE] = { 0 };
				sprintf_s(sameMessengerBuffer, MAX_MESSAGE_SIZE, "%d (You): %s", ntohs(client_addr.sin_port), client_message);

				if (sendto(socket_desc, sameMessengerBuffer, strlen(sameMessengerBuffer), 0,
					(struct sockaddr*)&addresses[i], client_struct_length) < 0) {
					printf("Couldn't send message to: %s:%d\n", inet_ntoa(addresses[i].sin_addr), ntohs(addresses[i].sin_port));
					continue;
				}
			}

			printf("Sent message to: %d\n", ntohs(addresses[i].sin_port));
		}
	}

	return 0;
}

int contains(struct sockaddr_in* addresses, size_t addresses_size, struct sockaddr_in* current_address) {
	for (int i = 0; i < addresses_size; i++) {
		if (is_same_address(&addresses[i], current_address)) {
			return 1;
		}
	}

	return 0;
}

int is_same_address(struct sockaddr_in* first, struct sockaddr_in* second) {
	return ntohs(first->sin_port) == ntohs(second->sin_port) &&
		strcmp(inet_ntoa(first->sin_addr), inet_ntoa(second->sin_addr)) == 0;
}