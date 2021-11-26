#include <stdio.h>
#include <WinSock2.h>
#include <Windows.h>

DWORD client_loop(void* ptr)
{
	SOCKET client = (SOCKET)ptr; // reinterpet_cast<SOCKET>(ptr)

	while (true)
	{
		char buffer[1024];
		int recv_size = recv(client, buffer, 1024, 0);
		if (recv_size == -1)
		{
			printf("Client disconnected \n");
			return 0;
		}
		printf("%.*s\n", recv_size, buffer);
	}

	return 0;
}

void server_main()
{

	printf("Port: ");

	unsigned short port;
	scanf_s("%hu", &port);

	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	sockaddr_in bind_addr;
	bind_addr.sin_family = AF_INET;
	bind_addr.sin_addr.s_addr = INADDR_ANY;
	bind_addr.sin_port = htons(port);

	int beind_result = bind(listen_sock, (sockaddr*)&bind_addr, sizeof(bind_addr));
	if (beind_result)
	{
		printf("bind failed %d\n", WSAGetLastError());
	}

	listen(listen_sock, 5);

	while (true)
	{
		SOCKET client = accept(listen_sock, NULL, NULL);
		CreateThread(
			nullptr,
			0,
			client_loop,
			(void*)client,
			0,
			nullptr
		);
	}
}


void client_main()
{

	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET)
	{
		printf("socket failed \n");
		return;
	}

	unsigned char ip_bytes[] = { 127, 0, 0, 1 };
	unsigned int ip = *(unsigned int*)ip_bytes;

	sockaddr_in conn_addr;
	conn_addr.sin_family = AF_INET;
	conn_addr.sin_addr.s_addr = ip;
	conn_addr.sin_port = htons(35000);

	int connect_result = connect(sock, (sockaddr*)&conn_addr, sizeof(conn_addr));
	if (connect_result)
	{
		printf("connection failed \n", WSAGetLastError());
		return;
	}

	while (true)
	{
		char send_buffer[1024];
		gets_s(send_buffer, 1024);

		send(sock, send_buffer, strlen(send_buffer), 0);
	}
}


int main()
{

	WSADATA wsa_data;

	WSAStartup(MAKEWORD(2, 2), &wsa_data);
	printf("what mode?? [c = client, s = server]: ");
	char selector[5];
	scanf_s("%s", selector, 5);

	if (selector[0] == 'c')
	{
		client_main();
	}
	if (selector[0] == 's')
	{
		server_main();
	}




	return 0;
}