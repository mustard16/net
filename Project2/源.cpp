#include<iostream>
#include <stdio.h>
#include<WinSock2.h>
#include<string>
#include<thread>
#pragma warning(disable:4996)
#pragma comment(lib, "Ws2_32.lib")
using namespace std;
#define buflen 256
//Э�鲿��
struct head {
	//����ip������port	��4�ֽ�
	int local_ip;
	int local_port;
	//�ͻ��˵����ֺͳ���
	int namelen;
	const char* name;
	//���뷽ʽ	5
	const char* code="utf-8";
	//����ʱ��	8
	time_t sendtime;
};


int recvthread(SOCKET clntsock)
{
	int revres;
	do {
		char msg2[buflen] = "\0";
		revres = recv(clntsock, msg2, buflen, 0);
		if (revres > 0)
		{
			head recvhead;
			string temp = string(msg2);
			recvhead.local_ip = atoi(temp.substr(0, 7).c_str());
			recvhead.local_port = atoi(temp.substr(7, 4).c_str());
			int lennum = 0;
			for (int i = 0;; i++)
			{
				if (!isdigit(temp[i + 11]))
					break;
				lennum++;
			}
			recvhead.namelen = atoi(temp.substr(11, lennum).c_str());
			char* t1 = new char[20];
			strcpy(t1, temp.substr(11 + lennum, recvhead.namelen).c_str());
			recvhead.name = (const char*)t1;
			char* t2 = new char[20];
			strcpy(t2, temp.substr(11 + lennum + recvhead.namelen, 5).c_str());
			recvhead.code = (const char*)t2;
			recvhead.sendtime = atoi(temp.substr(16 + lennum + recvhead.namelen, 10).c_str());
			int headsize = 26 + lennum + recvhead.namelen;
			
			printf("Bytes received: %d\n", revres);
			cout << "from: " << recvhead.name << endl;

			struct tm* ptminfo;
			ptminfo = localtime(&recvhead.sendtime);
			printf("time: %02d-%02d-%02d %02d:%02d:%02d\n",
				ptminfo->tm_year + 1900, ptminfo->tm_mon + 1, ptminfo->tm_mday,
				ptminfo->tm_hour, ptminfo->tm_min, ptminfo->tm_sec);

			cout << "recieve message: " << temp.substr(headsize,strlen(msg2)-headsize) << endl;
		}
		else if (revres == 0)
			printf("Connection closed\n");
		else
			printf("recv failed: %d\n", WSAGetLastError());
	} while (revres > 0);
	return 0;
}

int main()
{
	cout << "what's your name ?";
	string tempname;
	getline(cin, tempname);
	cout << "hello!  " << tempname << endl;
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

	head clnthead;
	sockaddr_in sock_addr;
	sock_addr.sin_family = AF_INET;
	sock_addr.sin_port = htons(7000);
	sock_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	int namelen = sizeof(sock_addr);
	connect(clntsock, (sockaddr*)&sock_addr, namelen);

	//����Э���������
	getsockname(clntsock, (sockaddr*)&sock_addr, &namelen);
	//printf("client:client address = %#x :%#x len %d \n", sock_addr.sin_addr.s_addr, sock_addr.sin_port, namelen);
	clnthead.local_ip = sock_addr.sin_addr.s_addr;
	clnthead.local_port = sock_addr.sin_port;
	clnthead.name = tempname.c_str();
	clnthead.namelen = strlen(tempname.c_str());
	time(&clnthead.sendtime);

	string char_msg = "";
	char ip[20] = "\0";
	itoa(clnthead.local_ip, ip, 16);
	char port[20] = "\0";
	itoa(clnthead.local_port, port, 16);
	char cstime[20] = "\0";
	itoa(int(clnthead.sendtime), cstime, 10);
	char nlen[20] = "\0";
	itoa(strlen(clnthead.name), nlen, 10);
	char_msg = string(ip) + string(port) + string(nlen) + string(clnthead.name) + string(clnthead.code) + string(cstime);
	cout << char_msg<<endl;

	thread th = thread(recvthread, clntsock);
	th.detach(); 
	//���߳����������̷߳��룬�˴˶���ִ�У��˺����������̴߳���ʱ�������ã��ҵ��ô˺�����ʹ�䲻�ܱ�join��
	//join����������ϣ��������
	int sendres;
	do
	{
		string data;
		getline(cin, data);
		const char* msg1;
		string m = char_msg + data;
		msg1 = m.c_str();
		if (data == "over")
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

	}while (sendres > 0);


	int finalres = closesocket(clntsock);
	if (finalres != 0)
	{
		cout << "client socket close error!";
		WSACleanup();
		return -1;
	}
	return 0;

}