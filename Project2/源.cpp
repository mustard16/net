#include<iostream>
#include <stdio.h>
#include<WinSock2.h>
#include<string>
#pragma warning(disable:4996)
#pragma comment(lib, "Ws2_32.lib")
using namespace std;
int main()
{
	WSADATA wsadata;
	int startres = WSAStartup(MAKEWORD(2, 2), &wsadata);
	//��λָ�����汾����λָ�����汾��ʹ�ð汾2.2
	//�ڶ�������Ϊ���صİ汾��Ϣ
	if (startres != 0)
	{
		cout << "socket start up error!";
		return -1;
	}
	SOCKET WSAAPI clntsock = socket(AF_INET, SOCK_STREAM, 0);
	//����һ��socket����TCP��ʽ�����������

	sockaddr_in sock_addr;
	sock_addr.sin_family = AF_INET;
	sock_addr.sin_port = htons(7000);
	sock_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	connect(clntsock, (sockaddr*)&sock_addr, sizeof(sock_addr));

#define buflen 256
	int sendres;
	int revres;
	do
	{
		string data;
		getline(cin, data);
		const char* msg1;
		msg1 = data.c_str();
		char msg2[buflen]="\0";//������Ϣ������
		if (msg1 == "over" || msg2 == "over")
		{
			break;
		}

		sendres = send(clntsock, msg1, strlen(msg1), 0);
		if (sendres > 0)
			printf("Bytes sended: %d\n", sendres);
		else if (sendres == 0)
			printf("Connection closed\n");
		else
			printf("send failed: %d\n", WSAGetLastError());

		revres = recv(clntsock, msg2, buflen, 0);
		if (revres > 0)
		{
			printf("Bytes received: %d\n", revres);
			cout << "recieve message: " << msg2 << endl;
		}
		else if (revres == 0)
			printf("Connection closed\n");
		else
			printf("recv failed: %d\n", WSAGetLastError());


	}while (sendres > 0 || revres > 0);


	int finalres = closesocket(clntsock);
	if (finalres != 0)
	{
		cout << "client socket close error!";
		WSACleanup();
		return -1;
	}
	return 0;

}