#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <pthread.h>
#include <signal.h>

#define CHATDATA 1024
#define IPADDR "127.0.0.1"
#define PORT 9001

void* doSendChat(void*);
void* doReceiveChat(void*);

const char* ESCAPE = "exit";

pthread_t thread1, thread2;
char nickname[20];

int main(int argc, char* argv[])
{
	int c_socket, len, n, status1, status2;
	struct sockaddr_in c_addr;
	
	c_socket = socket(PF_INET, SOCK_STREAM, 0);
	memset(&c_addr, 0, sizeof(c_addr));
	c_addr.sin_addr.s_addr = inet_addr(IPADDR);
	c_addr.sin_family = AF_INET;
	c_addr.sin_port = htons(PORT);

	printf("Input Nickname : ");
	scanf("%s", nickname);

	if (connect(c_socket, (struct sockaddr*)&c_addr, sizeof(c_addr)) == -1)
	{
		printf("Can not connect\n");
		return -1;
	}
	status1 =pthread_create(&thread1, NULL, doSendChat, (void*)&c_socket);
	status2 =pthread_create(&thread2, NULL, doReceiveChat, (void*)&c_socket);
	pthread_join(thread1, (void**)&status1);
	pthread_join(thread2, (void**)&status2);
	close(c_socket);
}

void* doSendChat(void* arg)
{
	char chatData[CHATDATA + 23];
	char buf[CHATDATA];
	int n, c_socket = *((int*)arg);
	
	while (1)
	{
		memset(buf, 0, sizeof(buf));
		if ((n = read(0, buf, sizeof(buf))) > 0)
		{
			sprintf(chatData, "[%s] %s", nickname, buf);
			write(c_socket, chatData, strlen(chatData));
			if (!strncmp(buf, ESCAPE, strlen(ESCAPE)))
			{
				pthread_kill(thread2, SIGINT);	
				break;
			}
		}
	} 
}

void* doReceiveChat(void* arg)
{
	char chatData[CHATDATA];
	int n, c_socket = *((int*)arg);
	
	while (1)
	{
		memset(chatData, 0, sizeof(chatData));
		if ((n = read(c_socket, chatData, sizeof(chatData))) > 0)
		{
			write(1, chatData, n);
		}
	}
}

void sendNickname(int c_socket)
{
	write(c_socket, nickname, strlen(nickname) + 1);
}









































