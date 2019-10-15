#include <stdio.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <string.h>

#include <time.h>
#include <stdlib.h>

#define PORT 9000

char rcvBuffer[100] = {};
char sndBuffer[100] = {};

int main()
{
	int c_socket, s_socket;
	struct sockaddr_in s_addr, c_addr;
	int len, n;
	
	int rand_port;
	srand((unsigned)time(NULL));
	rand_port = rand() % 5;

	// 1. 서버 소켓 생성, 클라이언트의 접속 요청을 처리(허용)을 위한 소켓
	s_socket = socket(PF_INET, SOCK_STREAM, 0);	// TCP/IP 통신을 위한 소켓 생성

	// 2. 서버 소켓 주소 설정
	memset(&s_addr, 0, sizeof(s_addr)); // s_addr 값을 0으로 초기화
	s_addr.sin_addr.s_addr = htonl(INADDR_ANY); // IP 주소 설정
	s_addr.sin_family = AF_INET; // IP 통신 허용
	s_addr.sin_port = htons(PORT + rand_port);

	// 3. 서버 소켓 바인딩
	if (bind(s_socket,(struct sockaddr *)&s_addr, sizeof(s_addr)) == -1)
	{
		printf("Cannot bind\n"); // 보통 포트가 사용중이거나 정상적인 종료가 되지 않았을때 발생 
		// lsof -i -nP 로 서버 확인 (ls open files, 옵션으로 네트워크 관련만 출력)
		// netstat -anp | grep 9000 으로도 확인 가능 (9000은 포트번호)
		return -1;
	}

	// 4. listen() 함수 실행
	if (listen(s_socket, 5) == -1) // listen(소켓, 동접자수)
	{
		printf("Listen fail\n");
		return -1;
	}

	// 5. 클라이언트 요청 처리, 요청 허용 후 메시지 전송
	printf("PORT : %d\n", PORT + rand_port);
	while (1)
	{
		len = sizeof(c_addr);

		printf("Wait for client...\n");
		// 클라이언트 요청 허용(accept) 및 통신을 위해 클라이언트 소켓 반환
		c_socket = accept(s_socket, (struct sockaddr *)&c_addr, &len); // 클라이언트 요청까지 대기..
		printf("Accept\n");

		n = read(c_socket, rcvBuffer, sizeof(rcvBuffer));
		rcvBuffer[n] = '\0';

		if (strcmp(rcvBuffer, "안녕하세요") == 0)
		{
			strcpy(sndBuffer, "안녕하세요. 만나서 반가워요.");
			write(c_socket, sndBuffer, strlen(sndBuffer));
		}
		else if (strcmp(rcvBuffer, "이름이 머야?") == 0)
		{
			strcpy(sndBuffer, "내 이름은 정태준이야.");
			write(c_socket, sndBuffer, strlen(sndBuffer));
		}
		else if (strcmp(rcvBuffer, "몇 살이야?") == 0)
		{
			strcpy(sndBuffer, "나는 24살이야.");
			write(c_socket, sndBuffer, strlen(sndBuffer));
		}

		close(c_socket);
	}
	close(s_socket);
	return 0;
}


