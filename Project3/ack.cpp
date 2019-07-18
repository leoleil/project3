#include "ack.h"

DWORD ack(LPVOID lpParameter)
{
	//���ݿ����ӹؼ���
	const char * SERVER = MYSQL_SERVER.data();
	const char * USERNAME = MYSQL_USERNAME.data();
	const char * PASSWORD = MYSQL_PASSWORD.data();
	const char DATABASE[20] = "di_mian_zhan";
	const int PORT = 3306;
	while (1) {
		//5�������ݿ����������
		Sleep(5000);
		//cout << "| ACK ����         | ������ݿ�����..." << endl;
		MySQLInterface mysql;//�������ݿ����Ӷ���

							 //�������ݿ�
		if (mysql.connectMySQL(SERVER, USERNAME, PASSWORD, DATABASE, PORT)) {
			//�������л�ȡ��������
			string selectSql = "select ������,ACK,unix_timestamp(����ʼʱ��),unix_timestamp(�������ʱ��),���Ǳ��,���������,����״̬ from �������� where ����״̬ = 3 or ����״̬ = 5 or ����״̬ = 1;";
			vector<vector<string>> dataSet;
			mysql.getDatafromDB(selectSql, dataSet);
			if (dataSet.size() == 0) {
				continue;//������Ĭ5��������ѯ
			}
			//��ѯ������
			for (int i = 0, len = dataSet.size(); i < len; i++) {
				char* messageDate;
				int messageDataSize = 0;
				//if (!dataSet[i][1]._Equal("111")) {

				//	continue;//�����ȴ�
				//}
				StringNumUtils util;//�ַ�ת���ֹ���

				long long dateTime = Message::getSystemTime();//��ȡ��ǰʱ���
				bool encrypt = false;//�Ƿ����
				UINT32 taskNum = util.stringToNum<UINT32>(dataSet[i][0]);//������
				UINT ACK = util.stringToNum<UINT>(dataSet[i][1]);//ACK
				long long taskStartTime = util.stringToNum<long long>(dataSet[i][2]);//����ʼʱ��
				long long taskEndTime = util.stringToNum<long long>(dataSet[i][3]);//�������ʱ��
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
				if (!socketer.createSendServer(ip, 5000, 0)) {
					//�������ɹ��ͷ���Դ
					delete groundStationId;
					delete satelliteId;
					continue;
				}
				const int bufSize = 66560;//���Ͱ��̶�65k
				int returnSize = 0;
				char* sendBuf = new char[bufSize];//���뷢��buf
				ZeroMemory(sendBuf, bufSize);//��շ��Ϳռ�
				ACKMessage message(9000, Message::getSystemTime(), false, taskNum, taskStartTime, taskEndTime, ACK);
				message.createMessage(sendBuf, returnSize, bufSize);
				if (socketer.sendMessage(sendBuf, bufSize) == -1) {
					//����ʧ��
					cout << "| ACK ����         | ����ʧ��" << endl;
				}
				string ackSql = "";
				if (dataSet[i][6]._Equal("3")) {
					ackSql = "update �������� set ����״̬ = 4 where ������ = " + dataSet[i][0];
				}
				else if (dataSet[i][6]._Equal("5")) {
					ackSql = "update �������� set ����״̬ = 6 where ������ = " + dataSet[i][0];
				}
				else {
					ackSql = "update �������� set ����״̬ = 1 where ������ = " + dataSet[i][0];
				}
				mysql.writeDataToDB(ackSql);
				delete sendBuf;
				//�Ͽ�TCP
				socketer.offSendServer();
				delete groundStationId;
				delete satelliteId;
			}
			mysql.closeMySQL();

		}
		else {
			cout << "| ACK ����         | �������ݿ�ʧ��" << endl;
			cout << "| ACK ���ʹ�����Ϣ | " << mysql.errorNum << endl;
		}
		cout << "| ACK ����         | �������" << endl;

	}
	return 0;
}
