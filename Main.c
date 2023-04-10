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
		start_client();
	}
	else {
		printf("wrong.\n");
	}
}