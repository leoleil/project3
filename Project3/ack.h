#pragma once

#include <iostream>  
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include "winsock2.h" 
#include <time.h>
#include <Windows.h>
#include "MySQLInterface.h"
#include "Message.h"
#include "ACKMessage.h"
#include "StringNumUtils.h"
#include "Socket.h"

using namespace std;

/*ackģ��*/
//ack�����߳����
DWORD WINAPI ack(LPVOID lpParameter);
