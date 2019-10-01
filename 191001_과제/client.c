#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>

#define IPADDR "127.0.0.1"
#define BUFSIZE 100

int PORT;

int main()
{
	int c_socket, n;
	struct sockaddr_in c_addr;
	char rcvBuffer[BUFSIZE] = {};
	char sndBuffer[BUFSIZE] = {};

	printf("PORT : ");
	scanf("%d", &PORT);
	scanf("%c", &rcvBuffer[0]);

	// 1. 클라이언트 소켓 생성
	c_socket = socket(PF_INET, SOCK_STREAM, 0); // 서버와 동일한 설정으로 생성해야함
	
	// 2. 소켓 정보 초기화
	memset(&c_addr, 0, sizeof(c_addr));
	c_addr.sin_addr.s_addr = inet_addr(IPADDR); // 접속할 IP 설정
	c_addr.sin_family = AF_INET;
	c_addr.sin_port = htons(PORT);

	// 3. 서버에 접속
	if (connect(c_socket, (struct sockaddr *)&c_addr, sizeof(c_addr)) == -1)
	{
		printf("Cannot connect\n");
		close(c_socket);	// 자원회수
		return -1;
	}

	printf(">> ");
	fgets(sndBuffer, sizeof(sndBuffer), stdin);
	write(c_socket,sndBuffer, strlen(sndBuffer) - 1);

	n = read(c_socket, rcvBuffer, sizeof(rcvBuffer));
	//rcvBuffer[n] = '\0';

	printf("%s\n", rcvBuffer);
	close(c_socket);
	return 0;
}


