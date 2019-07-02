// Server.cpp : Defines the entry point for the console application.  
//  
#include <iostream> 
#include <thread> //thread ͷ�ļ�,ʵ�����й��̵߳���
#include "winsock2.h"  
#include "MySQLInterface.h"
#include "assign.h"
#include "assignment_data_upload.h"
#include "assignment_data_download.h"
#include "AllocationMessage.h"
#include "tele.h"
#include "ack.h"
using namespace std;

vector<message_buf> MESSAGES;//ȫ�ֱ��ĳ�
CRITICAL_SECTION g_CS;//ȫ������ؼ�����ζ���
vector<AllocationMessage> ALLOCATION_MESSAGE;//ȫ��������䱨�ĳ�


int main(int argc, char* argv[])
{
	
	InitializeCriticalSection(&g_CS);//��ʼ���ؼ�����ζ���
    //�����߳�
	HANDLE hThread1;//�������������߳�
	hThread1 = CreateThread(NULL, 0, assign, NULL, 0, NULL);
	CloseHandle(hThread1);
	HANDLE hThread2;//�����������е��߳�
	hThread2 = CreateThread(NULL, 0, assignment_data_upload_rec, NULL, 0, NULL);
	CloseHandle(hThread2);
	//HANDLE hThread3;//��������ack���߳�
	//hThread3 = CreateThread(NULL, 0, ack, NULL, 0, NULL);
	//CloseHandle(hThread3);
	//HANDLE hThread4;//�����������е��߳�
	//hThread4 = CreateThread(NULL, 0, downdata, NULL, 0, NULL);
	//CloseHandle(hThread4);
	//HANDLE hThread5;//ң����߳�
	//hThread5 = CreateThread(NULL, 0, tele, NULL, 0, NULL);
	//CloseHandle(hThread5);
	while (1) {
	}

	Sleep(4000);//���̺߳�����Ĭ4��
	DeleteCriticalSection(&g_CS);//ɾ���ؼ�����ζ���
	system("pause");
	return 0;
}