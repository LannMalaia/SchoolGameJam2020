#include "Process.h"

int main()
{
	Initialize_CS();

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		err_quit("WSAStartUP()");

	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET)
		err_quit("socket()");

	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(SERVERPORT);
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

	int retval = bind(listen_sock,
		(SOCKADDR*)& serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
		err_quit("bind()");

	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR)
		err_quit("listen()");

	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;

	// 잘 와써
	cout << "준비 완료, 포트는 " << SERVERPORT << endl;
	while (1)
	{
		// 클라이언트 받기
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET)
		{
			err_display("accept()");
			continue;//break;
		}
		
		// 잘 와써
		cout << "클라가 들어왔어요." << endl;
		
		// 클라 정보 객체 만들기, 방 배정은 스레드에 들어가서 진행한다
		_ClientInfo* data = AddClientInfo(client_sock, clientaddr);
				
		// 스레드 만들기
		HANDLE hThread = CreateThread(NULL, 0, ProcessClient, data, 0, NULL);
		if (hThread == NULL) // 스레드 못 만들겄슈
		{
			RemoveClientInfo(data); // 그럼 클라도 보내줘야 해
			continue;
		}
		else
		{
			CloseHandle(hThread); // 야 나 메인스레드인데 이제 클라이언트 스레드에 접근 안할게
		}
	}

	// 임계 영역 삭제
	Delete_CS();
	closesocket(listen_sock);

	WSACleanup();
	return 0;
}