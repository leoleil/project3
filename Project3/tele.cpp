#include "tele.h"

DWORD tele(LPVOID lpParameter)
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
		cout << "| ����ң��         | ������ݿ�����..." << endl;
		MySQLInterface mysql;//�������ݿ����Ӷ���

							 //�������ݿ�
		if (mysql.connectMySQL(SERVER, USERNAME, PASSWORD, DATABASE, PORT)) {
			//�������л�ȡ��������
			//Ѱ�ҷַ���־Ϊ2�����ݷַ���־Ϊ0������
			string selectSql = "select ������,��������,unix_timestamp(�ƻ���ʼʱ��),unix_timestamp(�ƻ���ֹʱ��),���Ǳ��,��������� from �������� where �ַ���־ = 2 and �����־ = 0 and �������� = 100";
			vector<vector<string>> dataSet;
			mysql.getDatafromDB(selectSql, dataSet);
			if (dataSet.size() == 0) {
				continue;//������Ĭ5��������ѯ
			}
			//��ѯ������
			for (int i = 0, len = dataSet.size(); i < len; i++) {
				char* messageDate;
				int messageDataSize = 0;
				if (!dataSet[i][1]._Equal("100")) {

					continue;//�����ȴ�
				}
				StringNumUtils util;//�ַ�ת���ֹ���
				vector<definitionMes> D_MES_LIST;//ÿһ���豸�������ݱ��Ķ��弯��

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
				//��ȡ�����ļ�
				vector<vector<string>> str_ralation_Array;//���븸�ӹ�ϵ����
				vector<vector<string>> str_definition_Array;//�����ֶζ��弯��
				ifstream inFile1("D:\\���������˹�ϵͳ\\ң������\\���ӹ�ϵ��.csv", ios::in);//�����ļ�1

				string lineStr;//�ļ���ȡһ��
				getline(inFile1, lineStr);//������һ��
				while (getline(inFile1, lineStr)) {
					cout << lineStr << endl;
					stringstream ss(lineStr);
					string str;
					vector<string> lineArray;
					// ���ն��ŷָ�  
					while (getline(ss, str, ','))
						lineArray.push_back(str);
					str_ralation_Array.push_back(lineArray);
				}
				inFile1.close();
				ifstream inFile2("D:\\���������˹�ϵͳ\\ң������\\�ֶζ����.csv", ios::in);//�����ļ�2
				getline(inFile2, lineStr);//������һ��
				while (getline(inFile2, lineStr)) {
					cout << lineStr << endl;
					stringstream ss(lineStr);
					string str;
					vector<string> lineArray;
					// ���ն��ŷָ�  
					while (getline(ss, str, ','))
						lineArray.push_back(str);
					str_definition_Array.push_back(lineArray);
				}
				inFile2.close();
				StringNumUtils utiles;
				for (int i = 0; i < str_ralation_Array.size(); i++) {
					definitionMes mes;
					mes.name = str_ralation_Array[i][0];
					mes.p_name = str_ralation_Array[i][1];
					for (int j = 0; j < str_definition_Array.size(); j++) {
						if (str_definition_Array[j][7] == str_ralation_Array[i][0]) {
							field myfield;
							myfield.f_name = str_definition_Array[j][1];
							myfield.type = utiles.stringToNum<INT16>(str_definition_Array[j][2]);
							if (str_definition_Array[j][3] != "")myfield.min = utiles.stringToNum<double>(str_definition_Array[j][3]);
							if (str_definition_Array[j][4] != "")myfield.max = utiles.stringToNum<double>(str_definition_Array[j][4]);
							myfield.unit = str_definition_Array[j][5];
							myfield.display = utiles.stringToNum<bool>(str_definition_Array[j][6]);
							mes.f_list.push_back(myfield);
						}
					}
					D_MES_LIST.push_back(mes);
				}
				//cout << "�����ļ�������ϣ�" << endl;
				//���Ͷ��屨��
				for (int i = 0; i < D_MES_LIST.size(); i++) {
					//���ĳ���

					UINT32 message_len = 0;// = 117 + D_MES_LIST[i].f_list.size() * 67;//TODO
					BYTE message[M];
					BYTE* ptr = message;//messageָ��
					UINT16 mesage_id = 1000;//���ı�ʶ
					long long timestamp = Message::getSystemTime();//ʱ���
					bool encryption = false;//���ܱ�ʶ
					char name[40];//�豸����
					strcpy_s(name, 40, D_MES_LIST[i].name.c_str());
					char parent_name[40];//���豸����
					strcpy_s(parent_name, 40, D_MES_LIST[i].p_name.c_str());
					//��װmessage
					memcpy(ptr, &mesage_id, sizeof(UINT16));//�豸��ʶ
					ptr = ptr + sizeof(UINT16);
					memcpy(ptr, &message_len, sizeof(UINT32));//�ֶγ���
					ptr = ptr + sizeof(UINT32);
					memcpy(ptr, &timestamp, sizeof(long long));//ʱ��
					ptr = ptr + sizeof(long long);
					memcpy(ptr, &encryption, sizeof(bool));//����
					ptr = ptr + sizeof(bool);
					memcpy(ptr, &taskNum, sizeof(UINT32));//�����
					ptr = ptr + sizeof(UINT32);
					memcpy(ptr, name, 40);//�豸��
					ptr = ptr + 40;
					memcpy(ptr, parent_name, 40);//���豸��
					ptr = ptr + 40;
					memcpy(ptr, satelliteId, 20);//���Ǳ��
					ptr = ptr + 20;
					for (int j = 0; j < D_MES_LIST[i].f_list.size(); j++) {
						char field_name[40];//�ֶ�����
						UINT16 field_type;//�ֶ����ͱ��
						double min = NULL;//��Сֵ
						double max = NULL;//���ֵ
						char unit[8];//��λ
						bool display;//��ʾ��־
						strcpy_s(field_name, 40, D_MES_LIST[i].f_list[j].f_name.c_str());
						field_type = D_MES_LIST[i].f_list[j].type;
						min = D_MES_LIST[i].f_list[j].min;
						max = D_MES_LIST[i].f_list[j].max;
						strcpy_s(unit, 8, D_MES_LIST[i].f_list[j].unit.c_str());
						display = D_MES_LIST[i].f_list[j].display;
						memcpy(ptr, field_name, 40);
						ptr = ptr + 40;
						memcpy(ptr, &field_type, sizeof(UINT16));
						ptr = ptr + sizeof(UINT16);
						memcpy(ptr, &min, sizeof(double));
						ptr = ptr + sizeof(double);
						memcpy(ptr, &max, sizeof(double));
						ptr = ptr + sizeof(double);
						memcpy(ptr, unit, 8);
						ptr = ptr + 8;
						memcpy(ptr, &display, sizeof(bool));
						ptr = ptr + sizeof(bool);
					}
					Sleep(10);
					message_len = ptr - message;
					memcpy(message + 2, &message_len, sizeof(UINT32));
					const int bufSize = 66560;//���Ͱ��̶�65k
					int returnSize = 0;
					char sendBuf[bufSize];//���뷢��buf
					ZeroMemory(sendBuf, bufSize);//��շ��Ϳռ�
					memcpy(sendBuf, message, message_len);
					if (socketer.sendMessage(sendBuf, bufSize) == -1) {//���Ͱ��̶�65k

																	   //����ʧ���ͷ���Դ�����ļ���д
						cout << "| ����ң��         | ���屨�ķ���ʧ��" << endl;
						break;
					}


				}
				vector<vector<string>> str_data_Array;//���������ļ�����
				ifstream inFile("D:\\���������˹�ϵͳ\\ң������\\����.csv", ios::in);
				//���������ļ�
				while (getline(inFile, lineStr)) {
					stringstream ss(lineStr);
					string str;
					vector<string> lineArray;
					// ���ն��ŷָ�  
					while (getline(ss, str, ','))
						lineArray.push_back(str);
					//str_data_Array.push_back(lineArray);
					BYTE buf[M];//����buf
					BYTE* bufPtr = buf;//���ݱ���ָ��
					BYTE* buf_len_Ptr = buf;
					UINT16 id = 2000;//���ı�ʶ
					UINT32 len = 0;//���ĳ���
					memcpy(bufPtr, &id, sizeof(UINT16));//���ı�ʶ
					bufPtr = bufPtr + sizeof(UINT16);
					len = len + sizeof(UINT16);
					memcpy(bufPtr, &len, sizeof(UINT32));//���ĳ���
					bufPtr = bufPtr + sizeof(UINT32);
					len = len + sizeof(UINT16);
					long long timestamp = utiles.stringToNum<long long>(lineArray[0]);//����ʱ��
					memcpy(bufPtr, &timestamp, sizeof(long long));
					len = len + sizeof(long long);
					bufPtr = bufPtr + sizeof(long long);
					bool flag = 0;//���ܱ�ʶ
					memcpy(bufPtr, &flag, sizeof(bool));
					bufPtr = bufPtr + sizeof(bool);
					len = len + sizeof(bool);
					memcpy(bufPtr, &taskNum, sizeof(UINT32));//������
					bufPtr = bufPtr + sizeof(UINT32);
					len = len + sizeof(bool);
					char name[40];//�豸����
					strcpy_s(name, 40, lineArray[1].c_str());
					memcpy(bufPtr, name, 40);
					bufPtr = bufPtr + 40;
					len = len + 40;
					memcpy(bufPtr, satelliteId, 20);//���Ǳ��
					bufPtr = bufPtr + 20;
					long long tm = Message::getSystemTime();//����ʱ��
					memcpy(bufPtr, &tm, sizeof(long long));
					bufPtr = bufPtr + sizeof(long long);
					//ͨ���豸��ȥ��λ���豸���ֶ���Ϣ
					for (int j = 0; j < D_MES_LIST.size(); j++) {
						if (D_MES_LIST[j].name == lineArray[1]) {//����ҵ��˸����ݵĶ��屨��
							for (int k = 0; k < D_MES_LIST[j].f_list.size(); k++) {//���䶨�屨���ֶβ������Ͳ�д��Buffer
								if (D_MES_LIST[j].f_list[k].type == 1) {//16λ����
									INT16 data1 = utiles.stringToNum<INT16>(lineArray[k + 2]);
									memcpy(bufPtr, &data1, sizeof(INT16));
									bufPtr = bufPtr + sizeof(INT16);
									len = len + sizeof(INT16);
								}
								else if (D_MES_LIST[j].f_list[k].type == 2) {//16λ�޷�������
									UINT16 data2 = utiles.stringToNum<UINT16>(lineArray[k + 2]);
									memcpy(bufPtr, &data2, sizeof(UINT16));
									bufPtr = bufPtr + sizeof(UINT16);
									len = len + sizeof(UINT16);
								}
								else if (D_MES_LIST[j].f_list[k].type == 3) {//32λ����
									INT32 data3 = utiles.stringToNum<INT32>(lineArray[k + 2]);
									memcpy(bufPtr, &data3, sizeof(INT32));
									bufPtr = bufPtr + sizeof(INT32);
									len = len + sizeof(INT32);
								}
								else if (D_MES_LIST[j].f_list[k].type == 4) {//32λ�޷�������
									UINT32 data4 = utiles.stringToNum<UINT32>(lineArray[k + 2]);
									memcpy(bufPtr, &data4, sizeof(UINT32));
									bufPtr = bufPtr + sizeof(UINT32);
									len = len + sizeof(UINT32);
								}
								else if (D_MES_LIST[j].f_list[k].type == 5) {//64λ����
									INT64 data5 = utiles.stringToNum<INT64>(lineArray[k + 2]);
									memcpy(bufPtr, &data5, sizeof(INT64));
									bufPtr = bufPtr + sizeof(INT64);
									len = len + sizeof(INT64);
								}
								else if (D_MES_LIST[j].f_list[k].type == 6) {//64λ�޷�������
									UINT64 data6 = utiles.stringToNum<UINT64>(lineArray[k + 2]);
									memcpy(bufPtr, &data6, sizeof(UINT64));
									bufPtr = bufPtr + sizeof(UINT64);
									len = len + sizeof(UINT64);
								}
								else if (D_MES_LIST[j].f_list[k].type == 7) {//�����ȸ�����ֵ
									float data7 = utiles.stringToNum<float>(lineArray[k + 2]);
									memcpy(bufPtr, &data7, sizeof(float));
									bufPtr = bufPtr + sizeof(float);
									len = len + sizeof(float);
								}
								else if (D_MES_LIST[j].f_list[k].type == 8) {//˫���ȸ�����ֵ
									double data8 = utiles.stringToNum<double>(lineArray[k + 2]);
									memcpy(bufPtr, &data8, sizeof(double));
									bufPtr = bufPtr + sizeof(double);
									len = len + sizeof(double);
								}
								else if (D_MES_LIST[j].f_list[k].type == 9) {//�ַ�����
									char data9 = lineArray[k + 2].data()[0];
									memcpy(bufPtr, &data9, sizeof(char));
									bufPtr = bufPtr + sizeof(char);
									len = len + sizeof(char);
								}
								else if (D_MES_LIST[j].f_list[k].type == 10) {//���ַ�������
									char buf_data[15];//���ݰ�
									strcpy(buf_data, lineArray[k + 2].data());//�����ݿ��������ݰ�
									int size = lineArray[k + 2].length() + 1;//���ǵ�'/0'Ҫ��1
									memcpy(bufPtr, lineArray[k + 2].data(), size);
									bufPtr = bufPtr + 15;
									len = len + 15;
								}

								else if (D_MES_LIST[j].f_list[k].type == 11) {//���ַ�������
									char buf_data[255];//���ݰ�
									strcpy(buf_data, lineArray[k + 2].data());//�����ݿ��������ݰ�
									int size = lineArray[k + 2].length() + 1;//���ǵ�'/0'Ҫ��1
									memcpy(bufPtr, buf_data, size);
									bufPtr = bufPtr + 255;
									len = len + 255;
								}
								else if (D_MES_LIST[j].f_list[k].type == 12) {//���ֽ�����255
									char buf_data[255];//���ݰ�
									strcpy(buf_data, lineArray[k + 2].data());//�����ݿ��������ݰ�
									int size = lineArray[k + 2].length() + 1;//���ǵ�'/0'Ҫ��1
									memcpy(bufPtr, buf_data, size);
									bufPtr = bufPtr + 255;
									len = len + 255;
								}
								else if (D_MES_LIST[j].f_list[k].type == 13) {//���ֽ�����32K
									char buf_data[32 * K];//���ݰ�
									strcpy(buf_data, lineArray[k + 2].data());//�����ݿ��������ݰ�
									int size = lineArray[k + 2].length() + 1;//���ǵ�'/0'Ҫ��1
									memcpy(bufPtr, buf_data, size);
									bufPtr = bufPtr + 32 * K;
									len = len + 32 * K;
								}
								else if (D_MES_LIST[j].f_list[k].type == 14) {//���ֽ�����64K
									char buf_data[60 * K];//���ݰ�
									strcpy(buf_data, lineArray[k + 2].data());//�����ݿ��������ݰ�
									int size = lineArray[k + 2].length() + 1;//���ǵ�'/0'Ҫ��1
									memcpy(bufPtr, buf_data, size);
									int add = 60 * K;
									bufPtr = bufPtr + add;
									len = len + 60 * K;

								}
								else if (D_MES_LIST[j].f_list[k].type == 15) {//BOOL
									bool data = utiles.stringToNum<bool>(lineArray[k + 2]);
									memcpy(bufPtr, &data, sizeof(bool));
									bufPtr = bufPtr + sizeof(bool);
									len = len + sizeof(bool);
								}
								else if (D_MES_LIST[j].f_list[k].type == 16) {//ʱ���
									long long data16 = utiles.stringToNum<long long>(lineArray[k + 2]);
									memcpy(bufPtr, &data16, sizeof(long long));
									bufPtr = bufPtr + sizeof(long long);
									len = len + sizeof(long long);
								}
							}
						}
					}
					Sleep(10);//�������õ����ʷ���
					len = bufPtr - buf_len_Ptr;
					memcpy(buf_len_Ptr + 2, &len, sizeof(UINT32));
					const int bufSize = 66560;//���Ͱ��̶�65k
					int returnSize = 0;
					char* sendBuf = new char[bufSize];//���뷢��buf
					ZeroMemory(sendBuf, bufSize);//��շ��Ϳռ�
					memcpy(sendBuf, buf, len);
					if (socketer.sendMessage(sendBuf, bufSize) == -1) {//���Ͱ��̶�65k

																	   //����ʧ���ͷ���Դ�����ļ���д
						cout << "| ����ң��         | ���ݱ��ķ���ʧ��" << endl;
						delete sendBuf;

						break;
					}
					cout << "#";
					
				}

				inFile.close();
				//�Ͽ�TCP
				socketer.offSendServer();
				delete groundStationId;
				delete satelliteId;
			}


		}
		else {
			cout << "| ����ң��         | �������ݿ�ʧ��" << endl;
			cout << "| ����ң�������Ϣ | " << mysql.errorNum << endl;
		}
		cout << "| ����ң��         | �������" << endl;

	}
	return 0;
}
