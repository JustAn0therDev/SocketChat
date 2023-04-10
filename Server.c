#include <stdlib.h>
#include <winsock2.h>
#include <windows.h>
#include <stdio.h>

int start_server() {
	int socket_desc;
	char server_message[2000], client_message[2000];
	struct sockaddr_in server_addr, client_addr;
	int client_struct_length = sizeof(client_addr);

	// Clean buffers:
	memset(server_message, '\0', sizeof(server_message));
	memset(client_message, '\0', sizeof(client_message));

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

	strcpy(server_message, "I'll reply to you with the same message you send me.");

	//if (recvfrom(socket_desc, client_message, sizeof(client_message), 0,
	//	(struct sockaddr*)&client_addr, &client_struct_length) < 0) {
	//	printf("Couldn't receive\n");
	//	return -1;
	//}

	//if (sendto(socket_desc, server_message, strlen(server_message), 0,
	//	(struct sockaddr*)&client_addr, client_struct_length) < 0) {
	//	printf("Can't send\n");
	//	return -1;
	//}

	while (1) {
		if (recvfrom(socket_desc, client_message, sizeof(client_message), 0,
			(struct sockaddr*)&client_addr, &client_struct_length) < 0) {
			printf("Couldn't receive\n");
			return -1;
		}

		printf("Received message from IP: %s and port: %i\n",
			inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

		printf("Msg from client: %s\n", client_message);

		strcpy(server_message, client_message);

		if (sendto(socket_desc, server_message, strlen(server_message), 0,
			(struct sockaddr*)&client_addr, client_struct_length) < 0) {
			printf("Can't send\n");
			return -1;
		}

		strcpy(client_message, "");
	}

	return 0;
}