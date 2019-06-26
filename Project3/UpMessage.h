#pragma once
#include "Message.h"
class UpMessage :
	public Message
{
public:
	UpMessage(UINT16 messageId, long long dateTime, bool encrypt, UINT32 taskNum, char* fileName, char* expandName, bool endFlag);
	UpMessage();
	~UpMessage();
private:
	UINT32 taskNum;//������
	char fileName[32];//�ļ���
	char expandName[8];//��չ��
	bool endFlag;//������־
	int size;//���ݿ鳤��
	char* data;//���ݿ�
public:
	UINT32 getterTaskNum();

	void getterFileName(char* fileName);

	void getterExpandName(char* expandName);

	bool getterEndFlag();

	int getterDataSize();

	void setterDataSize(int size);

	void getterData(char* data, int & size);

	void setterData(char* data, int size);
public:
	//Ϊ���ݴ��䴴��һ��message�Ľӿ�
	void createMessage(char* buf, int & message_size, int buf_size);
	//ͨ�����ݰ����������ݽӿ�
	void messageParse(char* buf);
};

