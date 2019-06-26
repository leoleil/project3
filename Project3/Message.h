#pragma once
#include <winsock2.h> 
#include <sys/timeb.h>
class Message
{
public:
	Message();
	Message(UINT16 messageId, long long dateTime, bool encrypt);
	~Message();
protected:
	UINT16 messageId;//���ı�ʶ
	long long dateTime;//ʱ���
	bool encrypt;//�Ƿ����
protected:
	UINT32 messageLength;//���ĳ���
public:
	UINT16 getterMessageId();

	void setterMessageId(UINT16 messageId);
	
	UINT32 getterMessageLength();

	void setterMessageLength(UINT32 messageLengh);

	void messageLengthAdd(int len);

	long long getterDateTime();

	void setDateTime(long long dateTime);

	bool getterEncrypt();

	void setterEncrypt(bool encrypt);
public:
	//��ȡ��ǰʱ���
	static long long getSystemTime()
	{
		struct timeb t;

		ftime(&t);

		return 1000 * t.time + t.millitm;
	}
public:
	//Ϊ���ݴ��䴴��һ��message�Ľӿ�
	virtual void createMessage(char* buf, int & message_size, int buf_size)=0;
	//ͨ�����ݰ����������ݽӿ�
	virtual void messageParse(char* buf) = 0;
};

