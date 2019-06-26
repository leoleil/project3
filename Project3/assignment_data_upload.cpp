#include "assignment_data_upload.h"
vector<message_buf> DATA_MESSAGES;//ȫ���ϴ����ݱ��ĳ�
CRITICAL_SECTION data_CS;//�����̳߳عؼ�����ζ���
DWORD assignment_data_upload_rec(LPVOID lpParameter)
{
	InitializeCriticalSection(&data_CS);//��ʼ���ؼ�����ζ���
	HANDLE hThread1;//�������ݽ����̣߳���ȡ���ݳ�������
	hThread1 = CreateThread(NULL, 0, assignment_data_upload, NULL, 0, NULL);
	CloseHandle(hThread1);
	while (1) {
		UpdataSocket service;//���������������
		service.createReceiveServer(4999, DATA_MESSAGES);
	}
	DeleteCriticalSection(&data_CS);//ɾ���ؼ�����ζ���
	return 0;
}

DWORD assignment_data_upload(LPVOID lpParameter)
{
	
	while (1) {
		Sleep(100);
		EnterCriticalSection(&data_CS);//����ؼ������
		if (DATA_MESSAGES.empty()) {
			LeaveCriticalSection(&data_CS);
			continue;
		}
		//���ļ��ϴ�С
		int setLen = DATA_MESSAGES.size();
		LeaveCriticalSection(&data_CS);//�뿪�ؼ������

		for (int i = 0; i < setLen; i++) {
			EnterCriticalSection(&data_CS);//����ؼ������
			char byte_data[70 * 1024];//ÿ�����Ŀռ����70K
			memcpy(byte_data, DATA_MESSAGES[i].val, 70 * 1024);//�����ĳ�������ȡ��
			LeaveCriticalSection(&data_CS);//�뿪�ؼ������
			UpMessage upMessage;
			upMessage.messageParse(byte_data);//��������
			char fileName[32];
			upMessage.getterFileName(fileName);
			char expandName[8];
			upMessage.getterExpandName(expandName);
			UINT32 taskNum =  upMessage.getterTaskNum();//��ȡ������
			int size = 64 * 1024;
			char* data = new char[size];
			upMessage.getterData(data, size);//��ȡ����
			StringNumUtils util;
			string taskNumFile = util.numToString<UINT32>(taskNum);
			string path = "D:\\���������˹�ϵͳ\\���д�������\\" + taskNumFile;
			string file_path = path + "\\" + fileName + expandName;
			//string command;
			//command = "mkdir - p " + path;
			//system(command.c_str());//�����ļ���
			if (_access(path.c_str(), 0) == -1){	//����ļ��в�����
				_mkdir(path.c_str());				//�򴴽�
				cout << "| �������б���·��     | path:" + path << endl;

			}
			//���ļ�
			ofstream ofs(file_path, ios::binary | ios::out | ios::app);
			ofs.write(data, size);
			ofs.close();
			delete data;
			

			//�ж��Ƿ����ļ�β
			if (upMessage.getterEndFlag()) {
				//���ļ�βҪɾ����������
				DATA_MESSAGES.erase(DATA_MESSAGES.begin() , DATA_MESSAGES.begin() + i + 1);
				break;//����ѭ��
			}
			else {
				//�����ļ�βҪ�鿴���ĳ��Ƿ��и���
				//����Ѿ�ɸ�鵽���һ�����ݻ�û���ļ�β
				if (i == setLen - 1) {
					int count = 0;
					//�ȴ��µ�����,���һ����
					while (count<60) {
						EnterCriticalSection(&data_CS);//����ؼ������
						if (DATA_MESSAGES.size() > setLen) {
							int setLen = DATA_MESSAGES.size();
							break;//�����ݵ�������ѭ��
						}
						LeaveCriticalSection(&data_CS);
						count++;
						Sleep(1000);
					}
					if (count == 60) {
						cout << "| ��������         | �ļ��ϴ����ݵȴ���ʱ" << endl;
						//ɾ����������
						DATA_MESSAGES.erase(DATA_MESSAGES.begin() , DATA_MESSAGES.begin() + i + 1);
						//ɾ���Ѿ���������
						remove(file_path.c_str());
						cout << "| ��������         | ����ļ��ѻ����ļ�" << endl;
						break;//����ѭ��
					}
				}
				
			}
		}
		
	}
	return 0;
}
