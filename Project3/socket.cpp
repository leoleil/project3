#include "Socket.h"
#include "AllocationMessage.h"
#include <iostream> 
#include <sstream>
#include <string>
using namespace std;

Socket::Socket()
{
}


Socket::~Socket()
{
	WSACleanup(); //释放套接字资源  
}

bool Socket::createSendServer(const char* ip,const int hton,const double velocity) {
	//初始化套结字动态库
	if (WSAStartup(MAKEWORD(2, 2), &S_wsd) != 0)
	{
		cout << "WSAStartup failed!" << endl;
		return false;
	}
	//创建套接字  
	sHost = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == sHost)
	{
		cout << "socket failed!" << endl;
		//WSACleanup();//释放套接字资源  
		return  false;
	}

	//设置服务器地址和服务端口号  
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr(ip);
	servAddr.sin_port = htons(hton);
	int nServAddlen = sizeof(servAddr);

	//连接服务器  
	retVal = connect(sHost, (LPSOCKADDR)&servAddr, sizeof(servAddr));
	if (SOCKET_ERROR == retVal)
	{
		cout << "connect failed!" << endl;
		closesocket(sHost); //关闭套接字  
							//WSACleanup(); //释放套接字资源  
		return false;
	}
	return true;
}
int Socket::sendMessage(char* message, int lenght) {
	while (1)
	{
		retVal = send(sHost, message, lenght, 0);
		if (retVal == SOCKET_ERROR)
		{
			int r = WSAGetLastError();
			if (r == WSAEWOULDBLOCK)
			{
				continue;
			}
			else
			{
				cout << "send failed!" << endl;
				return -1;
			}
		}
		else
		{
			break;
		}

	}

	return 0;
}

int Socket::offSendServer() {
	//退出  
	closesocket(sHost); //关闭套接字  
	return 0;
}

int Socket::offRecServer()
{
	//退出  
	closesocket(sServer);   //关闭套接字  
	closesocket(sClient);   //关闭套接字  
	return 0;
}

int Socket::createReceiveServer(const int port, vector<message_buf>& message)
{

	cout << "| 任务接收         | 服务启动" << endl;
	//初始化套结字动态库  
	if (WSAStartup(MAKEWORD(2, 2), &S_wsd) != 0)
	{
		cout << "WSAStartup failed!" << endl;
		return 1;
	}

	//创建套接字  
	sServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == sServer)
	{
		cout << "socket failed!" << endl;
		WSACleanup();//释放套接字资源;  
		return  -1;
	}

	//服务器套接字地址   
	addrServ.sin_family = AF_INET;
	addrServ.sin_port = htons(port);
	addrServ.sin_addr.s_addr = INADDR_ANY;
	//绑定套接字  
	retVal = bind(sServer, (LPSOCKADDR)&addrServ, sizeof(SOCKADDR_IN));
	if (SOCKET_ERROR == retVal)
	{
		cout << "bind failed!" << endl;
		closesocket(sServer);   //关闭套接字  
		WSACleanup();           //释放套接字资源;  
		return -1;
	}

	//开始监听   
	cout << "| 任务接收         | 监听中" << endl;
	retVal = listen(sServer, 1);

	if (SOCKET_ERROR == retVal)
	{
		cout << "listen failed!" << endl;
		closesocket(sServer);   //关闭套接字  
		WSACleanup();           //释放套接字资源;  
		return -1;
	}

	//接受客户端请求  
	sockaddr_in addrClient;
	int addrClientlen = sizeof(addrClient);
	sClient = accept(sServer, (sockaddr FAR*)&addrClient, &addrClientlen);
	if (INVALID_SOCKET == sClient)
	{
		cout << "accept failed!" << endl;
		closesocket(sServer);   //关闭套接字  
		WSACleanup();           //释放套接字资源;  
		return -1;
	}

	cout << "| 任务接收         | TCP连接创建" << endl;
	while (true) {
		//数据窗口
		const int data_len = 66560;//每次接收65K数据包
		char data[66560]; //数据包
		ZeroMemory(data, data_len);//将数据包空间置0
		char* data_ptr = data;//数据指针
		int r_len = 0;
		while (1) {
			//接收客户端数据
			//清空buffer
			ZeroMemory(buf, BUF_SIZE);

			//获取数据
			retVal = recv(sClient, buf, BUF_SIZE, 0);

			if (SOCKET_ERROR == retVal)
			{
				cout << "| 任务接收         | 接收程序出错" << endl;
				closesocket(sServer);   //关闭套接字    
				closesocket(sClient);   //关闭套接字
				return -1;
			}
			if (retVal == 0) {
				cout << "| 任务接收         | 接收完毕断开本次连接" << endl;
				closesocket(sServer);   //关闭套接字    
				closesocket(sClient);   //关闭套接字
				return -1;
			}
			memcpy(data_ptr, buf, retVal);
			r_len = r_len + retVal;

			data_ptr = data_ptr + retVal;
			if ((data_ptr - data) >= data_len) {
				break;//如果接收到的数据大于最大窗口跳出循环（数据包最大64K）
			}

		}


		//将获取到的任务放入数据库
		char* ptr = data;
		UINT32 length = 0;
		for (int i = 0; i < data_len; i = i + length) {

			if (data[i] == -51 && data[i + 1] == -51)break;
			//获取报文长度
			memcpy(&length, ptr + i + 2, 4);
			int size = 70 * 1024;
			char val[70 * 1024];
			//获取一个buffer
			//内存复制
			memcpy(val, ptr + i, length);
			AllocationMessage message;
			message.messageParse(val);
			//加入报文池
			string sql = "";

		}

	}
	//退出  
	closesocket(sServer);   //关闭套接字  
	closesocket(sClient);   //关闭套接字  
	return 0;
}


