#include<iostream>
#include <stdio.h>
#include<WinSock2.h>
#pragma warning(disable:4996)
#pragma comment(lib, "Ws2_32.lib")
using namespace std;
int main()
{
	WSADATA wsadata;
	int startres=WSAStartup(MAKEWORD(2,2),&wsadata);
	//��λָ�����汾����λָ�����汾��ʹ�ð汾2.2
	//�ڶ�������Ϊ���صİ汾��Ϣ
	if (startres != 0)
	{
		cout << "socket start up error!";
		return -1;
	}
	SOCKET WSAAPI servsock = socket(AF_INET, SOCK_STREAM, 0);
	//����һ��socket����TCP��ʽ�����������

	sockaddr_in sock_addr;
	sock_addr.sin_family = AF_INET;
	sock_addr.sin_port = htons(7000);
	sock_addr.sin_addr.s_addr = INADDR_ANY;
	int bindres=bind(servsock, (sockaddr*)&sock_addr, sizeof(sock_addr));
	//��7000�˿ڣ���socket���ʼ���Ľṹ������
	if (bindres != 0)
	{
		cout << "socket bind error!";
		WSACleanup();
		return -1;
	}

	int lisres=listen(servsock,5);
	//�������˼����˿��ź�
	if (lisres != 0)
	{
		cout << "server socket listen error!";
		WSACleanup();
		return -1;
	}

	//���߳̿���������Ľ���һ�����飬����ʱ�������ţ����̲߳���
	SOCKET clntsock;
	sockaddr_in client_addr;
	int addlen = sizeof(client_addr);
	//clntsock = accept(servsock, (sockaddr*)&client_addr, &addlen);
	clntsock = accept(servsock, NULL, NULL);
	//����û�ж�client_addr��ֵ��������accept�������Զ�������䣬Ȼ���������
	if (clntsock == INVALID_SOCKET)
	{
		cout << "server socket accept error!" << endl;
		WSACleanup();
		return -1;
	}
	else
	{
		cout << "a clint join in!" << endl;
		//cout << client_addr.sin_addr.s_addr << endl;
	}

#define buflen 256
	int revres;
	char buf[buflen]="\0";//����Ϣ������
	const char* okmsg = "we accept your message successfully~ great!";
	do
	{
		revres = recv(clntsock, buf, buflen, 0);//accept�ķ���ֵ!!!
		if (revres > 0)
		{
			printf("Bytes received: %d\n", revres);
			cout << buf << endl;

			int iSendResult = send(clntsock, okmsg, strlen(okmsg), 0);
			if (iSendResult == SOCKET_ERROR) {
				printf("send failed: %d\n", WSAGetLastError());
				closesocket(clntsock);
				WSACleanup();
				return 1;
			}
		}
		else if (revres == 0)
			printf("Connection closed\n");
		else
			printf("recv failed: %d\n", WSAGetLastError());
	} while (revres > 0);
	


	int finalres = closesocket(servsock);
	if (finalres != 0)
	{
		cout << "server socket close error!";
		WSACleanup();
		return -1;
	}
	return 0;

}