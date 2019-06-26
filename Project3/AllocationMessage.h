#pragma once
#include <winsock2.h> 
#include "Message.h"
class AllocationMessage :
	public Message
{
public:
	AllocationMessage(UINT16 messageId, long long dateTime, bool encrypt, UINT32 taskNum, UINT16 taskType, long long taskStartTime, long long taskEndTime, char* satelliteId, char* groundStationId);
	AllocationMessage();
	~AllocationMessage();
private:
	UINT32 taskNum;//������
	UINT16 taskType;//��������
	long long taskStartTime;//�ƻ���ʼʱ��
	long long taskEndTime;//�ƻ�����ʱ��
	char satelliteId[20];//���Ǳ��
	char groundStationId[20];//����վ���
	int messageSize;//���ĳ���
	char* message;//��������
public:
	UINT32 getterTaskNum();
	void setterTaskNum(UINT32 taskNum);
	UINT16 getterTaskType();
	void setterTaskType(UINT16 taskType);
	long long getterTaskStartTime();
	void setterTaskStartTime(long long taskStartTime);
	long long getterTaskEndTime();
	void setterTaskEndTime(long long taskEndTime);
	void getterSatelliteId(char* satelliteId, int & size);
	void setterSatelliteId(char* satelliteId, int size);
	void getterGroundStationId(char* groundStationId, int & size);
	void setterGroundStationId(char* groundStationId, int size);
	int getterMessageSize();
	void setterMessageSize(int messageSize);
	void getterMessage(char* message,int & size);
	void setterMessage(char* message,int size);
public:
	void createMessage(char* buf, int & message_size, int buf_size);
	void messageParse(char* buf);

};

