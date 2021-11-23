#include <stdio.h>
#include <WinSock2.h>


int main() {

	WSADATA wsa_data;

	WSAStartup(MAKEWORD(2, 2), &wsa_data);

	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET)
	{
		printf("socket failed \n");
		return 1;
	}

	printf("whatever");

	return 0;
}