/*
�����̣߳���ȡ�յ�������
*/
#pragma once
#include <iostream>  
#include <sstream>
#include <string>
#include <fstream>  
#include <vector>
#include <unordered_map>
#include "winsock2.h" 
#include <time.h>
#include <Windows.h>
#include "MySQLInterface.h"
#include "StringNumUtils.h"
#include "Socket.h"
#include <thread> //thread ͷ�ļ�,ʵ�����й��̵߳���
#include "AllocationMessage.h"
#include "DownMessage.h"
#include "AssignmentSocket.h"
using namespace std;

extern string MYSQL_SERVER;
extern string MYSQL_USERNAME;
extern string MYSQL_PASSWORD;
extern vector<message_buf> MESSAGES;//������䱨�ĳ�
extern CRITICAL_SECTION g_CS;//ȫ�ֹؼ�����ζ���
DWORD WINAPI assign(LPVOID lpParameter);//�����ȡ�߳�