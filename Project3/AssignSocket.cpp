#include "AssignSocket.h"
#include "AllocationMessage.h"
#include "MySQLInterface.h"
#include "StringNumUtils.h"
#include <iostream> 
#include <sstream>
#include <string>
using namespace std;
extern CRITICAL_SECTION g_CS;//ȫ�ֹؼ�����ζ���

AssignSocket::AssignSocket()
{
}


AssignSocket::~AssignSocket()
{
}
int AssignSocket::createReceiveServer(const int port, std::vector<message_buf>& message)
{

	cout << "| ��������         | ��������" << endl;
	//��ʼ���׽��ֶ�̬��  
	if (WSAStartup(MAKEWORD(2, 2), &S_wsd) != 0)
	{
		cout << "WSAStartup failed!" << endl;
		return 1;
	}

	//�����׽���  
	sServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == sServer)
	{
		cout << "socket failed!" << endl;
		WSACleanup();//�ͷ��׽�����Դ;  
		return  -1;
	}

	//�������׽��ֵ�ַ   
	addrServ.sin_family = AF_INET;
	addrServ.sin_port = htons(port);
	addrServ.sin_addr.s_addr = INADDR_ANY;
	//���׽���  
	retVal = bind(sServer, (LPSOCKADDR)&addrServ, sizeof(SOCKADDR_IN));
	if (SOCKET_ERROR == retVal)
	{
		cout << "bind failed!" << endl;
		closesocket(sServer);   //�ر��׽���  
		WSACleanup();           //�ͷ��׽�����Դ;  
		return -1;
	}

	//��ʼ����   
	cout << "| ��������         | listening" << endl;
	retVal = listen(sServer, 1);

	if (SOCKET_ERROR == retVal)
	{
		cout << "listen failed!" << endl;
		closesocket(sServer);   //�ر��׽���  
		WSACleanup();           //�ͷ��׽�����Դ;  
		return -1;
	}

	//���ܿͻ�������  
	sockaddr_in addrClient;
	int addrClientlen = sizeof(addrClient);
	sClient = accept(sServer, (sockaddr FAR*)&addrClient, &addrClientlen);
	if (INVALID_SOCKET == sClient)
	{
		cout << "accept failed!" << endl;
		closesocket(sServer);   //�ر��׽���  
		WSACleanup();           //�ͷ��׽�����Դ;  
		return -1;
	}

	cout << "| ��������         | TCP���Ӵ���" << endl;
	while (true) {
		//���ݴ���
		const int data_len = 66560;//ÿ�ν���65K���ݰ�
		char data[66560]; //���ݰ�
		ZeroMemory(data, data_len);//�����ݰ��ռ���0
		char* data_ptr = data;//����ָ��
		int r_len = 0;
		while (1) {
			//���տͻ�������
			//���buffer
			ZeroMemory(buf, BUF_SIZE);

			//��ȡ����
			retVal = recv(sClient, buf, BUF_SIZE, 0);

			if (SOCKET_ERROR == retVal)
			{
				cout << "| ��������         | ���ճ������" << endl;
				closesocket(sServer);   //�ر��׽���    
				closesocket(sClient);   //�ر��׽���
				return -1;
			}
			if (retVal == 0) {
				cout << "| ��������         | ������϶Ͽ���������" << endl;
				closesocket(sServer);   //�ر��׽���    
				closesocket(sClient);   //�ر��׽���
				return -1;
			}
			memcpy(data_ptr, buf, retVal);
			r_len = r_len + retVal;

			data_ptr = data_ptr + retVal;
			if ((data_ptr - data) >= data_len) {
				break;//������յ������ݴ�����󴰿�����ѭ�������ݰ����65K��
			}

		}


		const char SERVER[10] = "127.0.0.1";//���ӵ����ݿ�ip
		const char USERNAME[10] = "root";
		const char PASSWORD[10] = "123456";
		const char DATABASE[20] = "satellite";
		const int PORT = 3306;


		//����ȡ�������ݷ������ݳ���
		char* ptr = data;
		UINT32 length = 0;
		for (int i = 0; i < data_len; i = i + length) {

			if (data[i] == -51 && data[i + 1] == -51)break;
			//��ȡ���ĳ���
			memcpy(&length, ptr + i + 2, 4);

			//��ȡһ��buffer
			message_buf messageBuf;
			//�ڴ渴��
			memcpy(&messageBuf.val, ptr + i, length);
			AllocationMessage allocationMessage;
			allocationMessage.messageParse(messageBuf.val);
			MySQLInterface mysql;
			if (mysql.connectMySQL(SERVER, USERNAME, PASSWORD, DATABASE, PORT)) {
				//ת������
				StringNumUtils utils;
				string ackSql = "INSERT INTO ����� (������, ���Ǳ��, ��������, ִ�б�־, ����ʼʱ��, �������ʱ��) VALUES(";
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
			}
			else {
				cout << "| ��������         | �������ݿ�ʧ��" << endl;
				cout << "| �������������Ϣ | " << mysql.errorNum << endl;
			}

		}

	}
	//�˳�  
	closesocket(sServer);   //�ر��׽���  
	closesocket(sClient);   //�ر��׽���  
							//WSACleanup();           //�ͷ��׽�����Դ;  
	return 0;
}
