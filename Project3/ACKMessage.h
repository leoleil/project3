#pragma once
#include "Message.h"
class ACKMessage :
	public Message
{
public:
	ACKMessage(UINT16 messageId, long long dateTime, bool encrypt, UINT32 taskNum, long long taskStartTime, long long taskEndTime, UINT ACK);
	ACKMessage();
	~ACKMessage();
private:
	UINT32 taskNum;//������
	long long taskStartTime;//����ʼʱ��
	long long taskEndTime;//�����ֹʱ��
	UINT ACK;
public:
	UINT32 getTaskNum();
	void setTaskNum(UINT32 taskNum);
	long long getTaskStartTime();//����ʼʱ��
	void setTaskStartTime(long long taskStartTime);
	long long getTaskEndTime();//�����ֹʱ��
	void setTaskEndTime(long long taskEndTime);
	UINT getACK();
	void setACK(UINT ACK);
public:
	void createMessage(char* buf, int & message_size, int buf_size);
	void messageParse(char* buf);
};