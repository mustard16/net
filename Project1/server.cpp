#include<iostream>
#include <stdio.h>
#include<WinSock2.h>
#include<thread>
#pragma warning(disable:4996)
#pragma comment(lib, "Ws2_32.lib")
using namespace std;
#define buflen 256

int serverthread(LPVOID lpParameter , int i)
{
	SOCKET* clntsock = (SOCKET*)lpParameter;
	int revres = 0;
	int j;
	if (i == 0)
		j = 1;
	else
		j = 0;
	do {
		char buf[buflen] = "";
		//����Ϣ������
		revres = recv(clntsock[i], buf, buflen, 0);//accept�ķ���ֵ!!!
		if (revres > 0)
		{
			string temp = string(buf);
			int lennum = 0;
			for (int i = 0;; i++)
			{
				if (!isdigit(temp[i + 11]))
					break;
				lennum++;
			}
			int namelen = atoi(temp.substr(11, lennum).c_str());
			char* t1 = new char[20];
			strcpy(t1, temp.substr(11 + lennum, namelen).c_str());
			const char* name = (const char*)t1;
			char* t2 = new char[20];
			strcpy(t2, temp.substr(11 + lennum + namelen, 5).c_str());
			const char* code = (const char*)t2;
			time_t sendtime = atoi(temp.substr(16 + lennum + namelen, 10).c_str());
			int headsize = 26 + lennum + namelen;

			printf("Bytes received: %d\n", revres);
			cout << "from: " << name << endl;
			cout << "message: " << temp.substr(headsize, strlen(buf) - headsize) << endl;
			struct tm* ptminfo;
			ptminfo = localtime(&sendtime);
			printf("time: %02d-%02d-%02d %02d:%02d:%02d\n",
				ptminfo->tm_year + 1900, ptminfo->tm_mon + 1, ptminfo->tm_mday,
				ptminfo->tm_hour, ptminfo->tm_min, ptminfo->tm_sec);

			//ת������һ��clint
				int tranres = send(clntsock[j], buf, strlen(buf), 0);
				if (tranres == SOCKET_ERROR)
				{
					printf("transform failed: %d\n", WSAGetLastError());
					closesocket(clntsock[j]);
					WSACleanup();
					return 1;
				}
			}
			else if (revres == 0)
				printf("Connection closed\n");
			else
				printf("recv failed: %d\n", WSAGetLastError());

		} while (revres > 0);
		//closesocket(*clntsock);
}

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

	SOCKET clntsock[2];
	for (int i = 0; i < 2; i++)
	{
		sockaddr_in client_addr;
		int addlen = sizeof(client_addr);
		//clntsock = accept(servsock, (sockaddr*)&client_addr, &addlen);
		clntsock[i] = accept(servsock, NULL, NULL);
		//����û�ж�client_addr��ֵ��������accept�������Զ�������䣬Ȼ���������
		if (clntsock[i] == INVALID_SOCKET)
		{
			cout << "server socket accept error!" << endl;
			WSACleanup();
			return -1;
		}
		else
		{
			printf("%d clint join in!\n", i + 1);
			//cout << client_addr.sin_addr.s_addr << endl;
		}
	}
	thread th[2];
	for (int i = 0; i < 2; i++)
	{
		th[i] = thread(serverthread, clntsock, i);
	}
	for (int i = 0; i < 2; i++)
	{
		th[i].join();
	}

	int finalres = closesocket(servsock);
	if (finalres != 0)
	{
		cout << "server socket close error!";
		WSACleanup();
		return -1;
	}

		return 0;
}