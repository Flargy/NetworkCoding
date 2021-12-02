#include <stdio.h>
#include <WinSock2.h>
#include <Windows.h>
#include <string.h>

#define USER_MAX 10

struct User
{
	bool active = false;
	SOCKET sock;
};

User users[USER_MAX];


DWORD server_loop(void* ptr) // now to let people select a name
{
	SOCKET client = (SOCKET)ptr; // reinterpet_cast<SOCKET>(ptr)

	printf("Client connected \n");
	int user_id = -1;
	for (int i = 0; i < USER_MAX; ++i)
	{
		if (users[i].active)
		{
			continue;
		}
		user_id = i;
	}

	if (user_id == -1)
	{
		closesocket(client);
		return 0;
	}

	User* user = &users[user_id];
	user->active = true;
	user->sock = client;

	{
		char messageBuffer[] = "Please input your name: ";
		int size = sizeof(messageBuffer);
		send(client, messageBuffer, size, 0);
	}

	char nameBuffer[1024];
	int nameBuffer_size = recv(client, nameBuffer, 64, 0);


	{
		char clientJoinedMessage[1024]{ 0 };
		char mes[23] = " has joined the server";
		strncat_s(clientJoinedMessage, 1024, nameBuffer, nameBuffer_size);
		strncat_s(clientJoinedMessage, 1024, mes, 23);

		for (int i = 0; i < USER_MAX; i++)
		{
			if (users[i].sock != client && users[i].active)
			{
				send(users[i].sock, clientJoinedMessage, nameBuffer_size + 23, 0);
			}
		}
	}

	nameBuffer[nameBuffer_size] = 0;

	char str[3] = ": ";
	strncat_s(nameBuffer, 1024, str, 3);
	nameBuffer_size += 2;

	while (true)
	{
		int recv_size = recv(client, nameBuffer + nameBuffer_size, 1024 - nameBuffer_size, 0);

		if (recv_size == -1)
		{
			printf("Client disconnected \n");
			user->active = false;
			return 0;
		}

		for (int i = 0; i < USER_MAX; i++)
		{
			if (users[i].sock != client && users[i].active)
			{
				send(users[i].sock, nameBuffer, nameBuffer_size + recv_size, 0); // make this go though active clients and send messages to them
			}
		}
	}

	return 0;
}

DWORD client_loop(void* ptr)
{
	SOCKET server = (SOCKET)ptr;	

	printf("starting to recieve messages from server \n");
	while (true)
	{
		char buffer[1024];
		int recv_size = recv(server, buffer, 1024, 0);
		if (recv_size == -1)
		{
			printf("Server shut down \n");
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
			server_loop,
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

	printf("Port: ");
	unsigned short port;
	scanf_s("%hu", &port);

	unsigned char ip_bytes[] = { 127, 0, 0, 1 };
	unsigned int ip = *(unsigned int*)ip_bytes;

	sockaddr_in conn_addr;
	conn_addr.sin_family = AF_INET;
	conn_addr.sin_addr.s_addr = ip;
	conn_addr.sin_port = htons(port);

	int connect_result = connect(sock, (sockaddr*)&conn_addr, sizeof(conn_addr));
	if (connect_result)
	{
		printf("connection failed \n", WSAGetLastError());
		return;
	}

	CreateThread(
		nullptr,
		0,
		client_loop,
		(void*)sock,
		0,
		nullptr
	);

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