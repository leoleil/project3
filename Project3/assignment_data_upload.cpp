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
	
	//���ݿ����ӹؼ���
	const char * SERVER = MYSQL_SERVER.data();
	const char * USERNAME = MYSQL_USERNAME.data();
	const char * PASSWORD = MYSQL_PASSWORD.data();
	const char DATABASE[20] = "di_mian_zhan";
	const int PORT = 3306;
	MySQLInterface mysql;//�������ݿ����Ӷ���
	if (mysql.connectMySQL(SERVER, USERNAME, PASSWORD, DATABASE, PORT)) {
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
			while (1) {
				EnterCriticalSection(&data_CS);//����ؼ������
				char byte_data[70 * 1024];//ÿ�����Ŀռ����70K
				memcpy(byte_data, DATA_MESSAGES[0].val, 70 * 1024);//�����ĳ�������ȡ��
				LeaveCriticalSection(&data_CS);//�뿪�ؼ������
				UpMessage upMessage;
				upMessage.messageParse(byte_data);//��������
				char fileName[32];
				upMessage.getterFileName(fileName);
				char expandName[8];
				upMessage.getterExpandName(expandName);
				UINT32 taskNum = upMessage.getterTaskNum();//��ȡ������
				int size = 64 * 1024;
				char* data = new char[size];
				upMessage.getterData(data, size);//��ȡ����
				StringNumUtils util;
				string taskNumFile = util.numToString<UINT32>(taskNum);
				string ackSql = "";
				ackSql = "update �������� set ����״̬ = 2 ,����ʼʱ�� = now() where ������ = " + taskNumFile;
				mysql.writeDataToDB(ackSql);
				MySQLInterface diskMysql;
				if (!diskMysql.connectMySQL(SERVER, USERNAME, PASSWORD, "disk", PORT)) {
					cout << "| ��������         | �������ݿ�ʧ��" << endl;
					cout << "| �������д�����Ϣ | " << diskMysql.errorNum << endl;
					break;
				}
				vector<vector<string>> disk;
				diskMysql.getDatafromDB("SELECT * FROM disk.����λ��;", disk);
				if (disk.size() == 0) {
					mysql.writeDataToDB("INSERT INTO ϵͳ��־��(ʱ��,ģ��,�¼�,������) VALUES (now(),'��������','����λ��δ֪'," + taskNumFile + ");");
					cout << "| ��������         | ����λ��δ֪�������������á�" << endl;
					break;
				}
				string path = disk[0][1];
				path = path+ "\\���д�������\\" + taskNumFile;
				string file_path = path + "\\" + fileName + expandName;
				//string command;
				//command = "mkdir - p " + path;
				//system(command.c_str());//�����ļ���
				if (_access(path.c_str(), 0) == -1) {	//����ļ��в�����
					_mkdir(path.c_str());				//�򴴽�
					cout << "| �������б���·�� | " + path << endl;

				}
				//���ļ�
				ofstream ofs(file_path, ios::binary | ios::out | ios::app);
				ofs.write(data, size);
				ofs.close();
				delete data;
				EnterCriticalSection(&data_CS);//����ؼ������
				DATA_MESSAGES.erase(DATA_MESSAGES.begin(), DATA_MESSAGES.begin() + 1);//ɾ��Ԫ��
				LeaveCriticalSection(&data_CS);

				//�ж��Ƿ����ļ�β
				if (upMessage.getterEndFlag()) {
					//���ļ�βҪɾ����������
					//DATA_MESSAGES.erase(DATA_MESSAGES.begin(), DATA_MESSAGES.begin() + i + 1);
					long long now = Message::getSystemTime();//��ȡ��ǰʱ��
					ackSql = "update �������� set ����״̬ = 3 , ACK = 1000,�������ʱ�� = now() where ������ = " + taskNumFile;
					mysql.writeDataToDB(ackSql);
					cout << "| ��������         | �ѻ����ļ��������" << endl;
					mysql.writeDataToDB("INSERT INTO ϵͳ��־��(ʱ��,ģ��,�¼�,������) VALUES (now(),'��������','�ѻ����ļ��������'," + taskNumFile + ");");
					break;//����ѭ��
				}
				else {
					int count = 0;
					//�ȴ��µ�����,���һ����
					while (count<60) {
						EnterCriticalSection(&data_CS);//����ؼ������
						if (DATA_MESSAGES.size() > 0) {
							LeaveCriticalSection(&data_CS);
							break;//�����ݵ�������ѭ��
						}
						LeaveCriticalSection(&data_CS);
						count++;
						Sleep(1000);
					}
					if (count == 60) {
						cout << "| ��������         | �ļ��ϴ����ݵȴ���ʱ" << endl;
						mysql.writeDataToDB("INSERT INTO ϵͳ��־��(ʱ��,ģ��,�¼�,������) VALUES (now(),'��������','�ļ��ϴ����ݵȴ���ʱ'," + taskNumFile + ");");
						//ɾ���Ѿ���������
						remove(file_path.c_str());
						cout << "| ��������         | ����ļ��ѻ����ļ�" << endl;
						mysql.writeDataToDB("INSERT INTO ϵͳ��־��(ʱ��,ģ��,�¼�,������) VALUES (now(),'��������','����ļ��ѻ����ļ�'," + taskNumFile + ");");
						long long now = Message::getSystemTime();//��ȡ��ǰʱ��
						ackSql = "update �������� set ����״̬ = 5 , ACK = 1100,�������ʱ�� = now()  where ������ = " + taskNumFile;
						mysql.writeDataToDB(ackSql);
						break;//����ѭ��
					}

				}
			}

		}
		mysql.closeMySQL();
		
	}
	else {
		cout << "| ��������         | �������ݿ�ʧ��" << endl;
		cout << "| �������д�����Ϣ | " << mysql.errorNum << endl;
	}
	
	return 0;
}
