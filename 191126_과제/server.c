#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <pthread.h>

#define MAX_CLIENT 10
#define CHATDATA 1024
#define INVAILD_SOCK -1
#define PORT 9001

void* doChat(void*);
int pushClient(int);
int popClient(int);
int findNickname(char*);

const char* ESCAPE = "exit";
const char* GREETING = "Welcome to chatting room\n";
const char* CODE200 = "Sorry No More Connection\n";

pthread_t thread;
pthread_mutex_t mutex;

int client[MAX_CLIENT];

int main(int argc, char* argv[])
{
	int c_socket, s_socket;
	struct sockaddr_in s_addr, c_addr;
	int len, i, res, errno;

	memset(client, 0, sizeof(client));
	if (pthread_mutex_init(&mutex, NULL) != 0)
	{
		printf("Can not create mutex\n");
		return -1;
	}
	s_socket = socket(PF_INET, SOCK_STREAM, 0);
	memset(&s_addr, 0, sizeof(s_addr));
	s_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(PORT);
	if ((errno = bind(s_socket, (struct sockaddr *)&s_addr, sizeof(s_addr))) == -1)
	{
		printf("Can not Bind : %d\n", errno);
		return -1;
	}
	if (listen(s_socket, MAX_CLIENT) == -1)
	{
		printf("listen Fail\n");
		return -1;
	}
	for (i = 0; i < MAX_CLIENT; i++)
	{
		client[i] = INVAILD_SOCK;
	}
	printf("wait connection..\n");
	while (1)
	{
		len = sizeof(c_addr);
		c_socket = accept(s_socket, (struct sockaddr *) &c_addr, &len);
		res = pushClient(c_socket);
		if (res < 0)
		{
			write(c_socket, CODE200, strlen(CODE200));
			close(c_socket);
		}
		else
		{
			write(c_socket, GREETING, strlen(GREETING));
		}
		pthread_create(&thread, NULL, doChat, (void*)&c_socket);
	}
	pthread_mutex_destroy(&mutex);
	return 0;
}

int pushClient(int c_socket)
{
	int i;
	printf("pushClient!!\n");
	for (i = 0; i < MAX_CLIENT; i++)
	{
		if (client[i] == -1)
		{
			client[i] = c_socket;
			return i;
		}
	}
	return -1;
}

int popClient(int c_socket)
{
	int i, res = -1;
	
	pthread_mutex_lock(&mutex);
	for (i = 0; i < MAX_CLIENT; ++i)
	{
		if (client[i] == c_socket)
		{
			close(c_socket);
			client[i] = -1;
			break;
		}
	}
	pthread_mutex_unlock(&mutex);
	if (i != MAX_CLIENT) return 1;
	return 0;
}

void* doChat(void* arg)
{
	int c_socket = *((int *)arg);
	char chatData[CHATDATA];
	int i, n;

	while (1) 
	{
		memset(chatData, 0, sizeof(chatData));
		if ((n = read(c_socket, chatData, sizeof(chatData))) > 0)
		{

			sendGlobalMessage(chatData, strlen(chatData));
			if (isExit(chatData, c_socket)) break;	
		}
	}
}

void sendGlobalMessage(char* message, int len)
{
	int i;
	for (i = 0; i < MAX_CLIENT; ++i)
	{
		if (client[i] != 0)
		{
			write(client[i], message, len + 1);
		}
	}
}

int isExit(char* message, int s_socket)
{
	if (!strcmp(message, ESCAPE)) 
	{	
		popClient(s_socket);
		return 1;
	}
	return 0;
}




















