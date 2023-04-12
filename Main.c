#include <stdlib.h>
#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include "Client.h"
#include "Server.h"

int main() {
	printf("Which one do you want to start?\n");
	printf("1. Server\n");
	printf("2. Client\n");

	char n = getchar();

	if (n == '1') {
		start_server();
	}
	else if (n == '2') {
		char nickname[150];
		printf("Please tell me your nickname (up to 150 characters): ");
		scanf("%s", nickname);
		start_client(nickname);
	}
	else {
		printf("wrong.\n");
	}
}