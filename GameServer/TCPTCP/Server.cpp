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

	// �� �ͽ�
	cout << "�غ� �Ϸ�, ��Ʈ�� " << SERVERPORT << endl;
	while (1)
	{
		// Ŭ���̾�Ʈ �ޱ�
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET)
		{
			err_display("accept()");
			continue;//break;
		}
		
		// �� �ͽ�
		cout << "Ŭ�� ���Ծ��." << endl;
		
		// Ŭ�� ���� ��ü �����, �� ������ �����忡 ���� �����Ѵ�
		_ClientInfo* data = AddClientInfo(client_sock, clientaddr);
				
		// ������ �����
		HANDLE hThread = CreateThread(NULL, 0, ProcessClient, data, 0, NULL);
		if (hThread == NULL) // ������ �� ����ν�
		{
			RemoveClientInfo(data); // �׷� Ŭ�� ������� ��
			continue;
		}
		else
		{
			CloseHandle(hThread); // �� �� ���ν������ε� ���� Ŭ���̾�Ʈ �����忡 ���� ���Ұ�
		}
	}

	// �Ӱ� ���� ����
	Delete_CS();
	closesocket(listen_sock);

	WSACleanup();
	return 0;
}