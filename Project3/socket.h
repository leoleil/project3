#pragma comment(lib, "ws2_32.lib")
#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h> 
#include <vector>

extern CRITICAL_SECTION g_CS;//ȫ�ֹؼ�����ζ���

//SocketΪʹ�����ṩһ��70k���ݰ�
struct message_buf
{
	//70K
	char val[70 * 1024];
};

class Socket
{
public:
	Socket();
	~Socket();
public:
	bool createSendServer(const char* ip, const int hton, const double velocity);

	int sendMessage(char* message, int lengt);

	int createReceiveServer(const int port, std::vector<message_buf>& message);

	int offSendServer();

	int offRecServer();

protected:
	const int BUF_SIZE = 1024;//buffer��С

protected:
	WSADATA wsd;//WSADATA����  

	SOCKET sServer;//��Ϊ���ն˵ķ������׽���  

	SOCKET sClient;//��Ϊ���ն˵Ŀͻ����׽���

	SOCKADDR_IN addrServ;//��������ַ 

	char buf[1024];  //�������ݻ�����  

	int retVal;//����ֵ 

	WSADATA S_wsd; //�����WSADATA����  

	WSADATA H_wsd; //�ͻ���WSADATA����  

	SOCKET sHost; //��Ϊ���Ͷ˵ķ������׽��� 

	SOCKADDR_IN servAddr; //��������ַ  
};

