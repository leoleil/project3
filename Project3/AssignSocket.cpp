#include "AssignSocket.h"
#include "AllocationMessage.h"
#include "MySQLInterface.h"
#include "StringNumUtils.h"
#include <iostream> 
#include <sstream>
#include <string>
using namespace std;
extern CRITICAL_SECTION g_CS;//全局关键代码段对象

AssignSocket::AssignSocket()
{
}


AssignSocket::~AssignSocket()
{
}
int AssignSocket::createReceiveServer(const int port, std::vector<message_buf>& message)
{

	cout << "| 接收任务         | 服务启动" << endl;
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
	cout << "| 接收任务         | listening" << endl;
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

	cout << "| 接收任务         | TCP连接创建" << endl;
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
				cout << "| 接收任务         | 接收程序出错" << endl;
				closesocket(sServer);   //关闭套接字    
				closesocket(sClient);   //关闭套接字
				return -1;
			}
			if (retVal == 0) {
				cout << "| 接收任务         | 接收完毕断开本次连接" << endl;
				closesocket(sServer);   //关闭套接字    
				closesocket(sClient);   //关闭套接字
				return -1;
			}
			memcpy(data_ptr, buf, retVal);
			r_len = r_len + retVal;

			data_ptr = data_ptr + retVal;
			if ((data_ptr - data) >= data_len) {
				break;//如果接收到的数据大于最大窗口跳出循环（数据包最大65K）
			}

		}


		const char SERVER[10] = "127.0.0.1";//连接的数据库ip
		const char USERNAME[10] = "root";
		const char PASSWORD[10] = "";
		const char DATABASE[20] = "satellite";
		const int PORT = 3306;


		//将获取到的数据放入数据池中
		char* ptr = data;
		UINT32 length = 0;
		for (int i = 0; i < data_len; i = i + length) {

			if (data[i] == -51 && data[i + 1] == -51)break;
			//获取报文长度
			memcpy(&length, ptr + i + 2, 4);

			//获取一个buffer
			message_buf messageBuf;
			//内存复制
			memcpy(&messageBuf.val, ptr + i, length);
			AllocationMessage allocationMessage;
			allocationMessage.messageParse(messageBuf.val);
			MySQLInterface mysql;
			if (mysql.connectMySQL(SERVER, USERNAME, PASSWORD, DATABASE, PORT)) {
				//转换工具
				StringNumUtils utils;
				string ackSql = "INSERT INTO 任务表 (任务编号, 卫星编号, 任务类型, 执行标志, 任务开始时间, 任务结束时间) VALUES(";
				ackSql += utils.numToString<UINT32>(allocationMessage.getterTaskNum()) + ",";
				char satelliteId[50];
				int size;
				allocationMessage.getterSatelliteId(satelliteId, size);
				ackSql += satelliteId;
				ackSql += ",";
				ackSql += utils.numToString<UINT16>(allocationMessage.getterTaskType()) + ",";
				ackSql += "FROM_UNIXTIME(" + utils.numToString<long long>(allocationMessage.getterTaskStartTime()) + "),";
				ackSql += "FROM_UNIXTIME(" + utils.numToString<long long>(allocationMessage.getterTaskEndTime()) + "));";
				mysql.writeDataToDB(ackSql);
				mysql.writeDataToDB("INSERT INTO 系统日志表(时间,模块,事件,任务编号) VALUES (now(),'任务分配','接收任务'," + utils.numToString<UINT32>(allocationMessage.getterTaskNum()) + ");");
			}
			else {
				cout << "| 接收任务         | 连接数据库失败" << endl;
				cout << "| 接收任务错误信息 | " << mysql.errorNum << endl;
			}

		}

	}
	//退出  
	closesocket(sServer);   //关闭套接字  
	closesocket(sClient);   //关闭套接字  
							//WSACleanup();           //释放套接字资源;  
	return 0;
}
