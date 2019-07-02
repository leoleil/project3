#include "assignment_data_download.h"

DWORD downdata(LPVOID lpParameter)
{
	//���ݿ����ӹؼ���
	const char SERVER[10] = "127.0.0.1";
	const char USERNAME[10] = "root";
	const char PASSWORD[10] = "123456";
	const char DATABASE[20] = "di_mian_zhan";
	const int PORT = 3306;
	while (1) {
		//5�������ݿ����������
		Sleep(5000);
		cout << "| ��������         | ������ݿ�����..." << endl;
		MySQLInterface mysql;//�������ݿ����Ӷ���

							 //�������ݿ�
		if (mysql.connectMySQL(SERVER, USERNAME, PASSWORD, DATABASE, PORT)) {
			//�������л�ȡ��������
			//Ѱ�ҷַ���־Ϊ2�����ݷַ���־Ϊ0������
			string selectSql = "select ������,��������,unix_timestamp(�ƻ���ʼʱ��),unix_timestamp(�ƻ���ֹʱ��),���Ǳ��,��������� from �������� where �ַ���־ = 2 and �����־ = 0 and �������� = 111";
			vector<vector<string>> dataSet;
			mysql.getDatafromDB(selectSql, dataSet);
			if (dataSet.size() == 0) {
				continue;//������Ĭ5��������ѯ
			}
			//��ѯ������
			for (int i = 0, len = dataSet.size(); i < len; i++) {
				char* messageDate;
				int messageDataSize = 0;
				if (!dataSet[i][1]._Equal("111")) {

					continue;//�����ȴ�
				}
				//������������
				StringNumUtils util;//�ַ�ת���ֹ���

				long long dateTime = Message::getSystemTime();//��ȡ��ǰʱ���
				bool encrypt = false;//�Ƿ����
				UINT32 taskNum = util.stringToNum<UINT32>(dataSet[i][0]);//������
				UINT16 taskType = util.stringToNum<UINT16>(dataSet[i][1]);//��������
				long long taskStartTime = util.stringToNum<long long>(dataSet[i][2]);//�ƻ���ʼʱ��
				if (taskStartTime > dateTime) {
					//�����û���ƻ���ʼʱ�������
					continue;
				}

				long long taskEndTime = util.stringToNum<long long>(dataSet[i][3]);//�ƻ���ֹʱ��
				char* satelliteId = new char[20];//���Ǳ��
				strcpy_s(satelliteId, dataSet[i][4].size() + 1, dataSet[i][4].c_str());
				char* groundStationId = new char[20];//���������
				strcpy_s(groundStationId, dataSet[i][5].size() + 1, dataSet[i][5].c_str());

				//���ҵ���վip��ַ���ͱ���
				string groundStationSql = "select IP��ַ from ��������Ϣ�� where ��������� =" + dataSet[i][5];
				vector<vector<string>> ipSet;
				mysql.getDatafromDB(groundStationSql, ipSet);
				if (ipSet.size() == 0) {
					delete groundStationId;
					delete satelliteId;
					continue;//û���ҵ�ip��ַ
				}

				//����������
				Socket socketer;
				const char* ip = ipSet[0][0].c_str();//��ȡ����ַ
													 //����TCP����
				if (!socketer.createSendServer(ip, 4999, 0)) {
					//�������ɹ��ͷ���Դ
					delete groundStationId;
					delete satelliteId;
					continue;
				}

				//��ȡ�������������ļ�
				//��ȡ�����ļ��������շ���
				string config = "D:\\���������˹�ϵͳ\\���д�������\\" + dataSet[i][0] + "\\config.txt";
				ifstream is(config, ios::in);
				if (!is.is_open()) {
					cout << "| ��������         | ";
					cout << config << " �޷���" << endl;
					//�������ɹ��ͷ���Դ
					delete groundStationId;
					delete satelliteId;
					continue;
				}
				string fileName = "";//�ļ���
				string expandName = "";//��չ��

				getline(is, fileName);
				getline(is, expandName);
				is.close();
				string file = "D:\\���������˹�ϵͳ\\���д�������\\" + dataSet[i][0] + "\\" + fileName + expandName;
				ifstream fileIs(file, ios::binary | ios::in);
				if (!fileIs.is_open()) {
					cout << "| ��������         | ";
					cout << file << " �޷���" << endl;
					//�������ɹ��ͷ���Դ
					delete groundStationId;
					delete satelliteId;
					continue;
				}
				//��ȡ�ļ�
				while (!fileIs.eof()) {
					int bufLen = 1024 * 64;//�������64K
					char* fileDataBuf = new char[bufLen];//64K
					fileIs.read(fileDataBuf, bufLen);
					bufLen = fileIs.gcount();//��ȡʵ�ʶ�ȡ���ݴ�С

					char* up_file_name = new char[32];//�ļ���
					strcpy_s(up_file_name, fileName.size() + 1, fileName.c_str());
					char* up_expand_name = new char[8];//��չ��
					strcpy_s(up_expand_name, expandName.size() + 1, expandName.c_str());
					bool endFlag = fileIs.eof();//�ļ�β�ж�
												//�����������а�
					DownMessage downMessage(3020, dateTime, encrypt, taskNum, up_file_name, up_expand_name, endFlag);
					downMessage.setterData(fileDataBuf, bufLen);//��������

					const int bufSize = 66560;//���Ͱ��̶�65k
					int returnSize = 0;
					char* sendBuf = new char[bufSize];//���뷢��buf
					ZeroMemory(sendBuf, bufSize);//��շ��Ϳռ�
					downMessage.createMessage(sendBuf, returnSize, bufSize);//���������ֽڰ�
					if (socketer.sendMessage(sendBuf, bufSize) == -1) {//���Ͱ��̶�65k

																	   //����ʧ���ͷ���Դ�����ļ���д
						cout << "| ��������         | ����ʧ�ܣ��Ͽ�����" << endl;
						delete sendBuf;
						delete up_expand_name;
						delete up_file_name;
						delete fileDataBuf;
						break;
					}
					//flieOs.write(fileDataBuf, bufLen);
					if (fileIs.eof() == true) {

						cout << "| ��������         | " << dataSet[i][0] << "���������гɹ�" << endl;

						//�޸����ݿ�ַ���־
						string ackSql = "update �������� set ���ݷַ���־ = 2 where ������ = " + dataSet[i][0];

					}

					delete sendBuf;
					delete up_expand_name;
					delete up_file_name;
					delete fileDataBuf;

				}
				//�Ͽ�TCP
				socketer.offSendServer();
				fileIs.close();
				delete groundStationId;
				delete satelliteId;
			}


		}
		else {
			cout << "| ��������         | �������ݿ�ʧ��" << endl;
			cout << "| �������д�����Ϣ | " << mysql.errorNum << endl;
		}
		cout << "| ��������         | �������" << endl;

	}
	return 0;
}
