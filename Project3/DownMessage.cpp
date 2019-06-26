#include "DownMessage.h"



DownMessage::DownMessage(UINT16 messageId, long long dateTime, bool encrypt, UINT32 taskNum, char * fileName, char * expandName, bool endFlag) :Message(messageId, dateTime, encrypt), taskNum(taskNum), endFlag(endFlag)
{
	strcpy_s(this->fileName, 32, fileName);
	strcpy_s(this->expandName, 8, expandName);
	this->messageLength = this->messageLength + sizeof(UINT32) + sizeof(bool) + 32 + 8;
}

DownMessage::DownMessage()
{
}


DownMessage::~DownMessage()
{
	delete data;
}

UINT32 DownMessage::getterTaskNum()
{
	return this->taskNum;
}

void DownMessage::getterFileName(char * fileName)
{
	memcpy(fileName, this->fileName, 32);

}

void DownMessage::getterExpandName(char * expandName)
{
	memcpy(expandName, this->expandName, 8);
}

bool DownMessage::getterEndFlag()
{
	return this->endFlag;
}

int DownMessage::getterDataSize()
{
	return size;
}

void DownMessage::setterDataSize(int size)
{
	this->size = size;
}

void DownMessage::getterData(char * data, int & size)
{
	memcpy(data, this->data, this->size);
	size = this->size;
}

void DownMessage::setterData(char * data, int size)
{
	this->data = new char[size];
	memcpy(this->data, data, size);
	this->messageLength = this->messageLength + size;//���ĳ��ȼ�
	this->size = size;//���ݴ�С�ı�
}

void DownMessage::createMessage(char * buf, int & message_size, int buf_size)
{
	if (buf_size >= messageLength) {
		char* bufPtr = buf;//bufָ��
		memcpy(bufPtr, &(messageId), sizeof(UINT16));//�����־
		bufPtr = bufPtr + sizeof(UINT16);//�ƶ�ָ��
		memcpy(bufPtr, &messageLength, sizeof(UINT32));//���ĳ���
		bufPtr = bufPtr + sizeof(UINT32);//�ƶ�ָ��
		memcpy(bufPtr, &dateTime, sizeof(long long));//ʱ���
		bufPtr = bufPtr + sizeof(long long);//�ƶ�ָ��
		memcpy(bufPtr, &encrypt, sizeof(bool));//���ܱ�ʶ
		bufPtr = bufPtr + sizeof(bool);//�ƶ�ָ��
		memcpy(bufPtr, &taskNum, sizeof(UINT32));//������
		bufPtr = bufPtr + sizeof(UINT32);//�ƶ�ָ��
		memcpy(bufPtr, fileName, 32);//�ļ���
		bufPtr = bufPtr + 32;//�ƶ�ָ��
		memcpy(bufPtr, expandName, 8);//�ļ���
		bufPtr = bufPtr + 8;//�ƶ�ָ��
		memcpy(bufPtr, &endFlag, sizeof(bool));//������־
		bufPtr = bufPtr + sizeof(bool);//�ƶ�ָ��
		memcpy(bufPtr, data, size);//���ݰ�
		message_size = messageLength;
	}
}

void DownMessage::messageParse(char * buf)
{
	char* bufPtr = buf;//bufָ��
	memcpy(&(messageId), bufPtr, sizeof(UINT16));//�����־
	bufPtr = bufPtr + sizeof(UINT16);//�ƶ�ָ��
	memcpy(&messageLength, bufPtr, sizeof(UINT32));//���ĳ���
	bufPtr = bufPtr + sizeof(UINT32);//�ƶ�ָ��
	memcpy(&dateTime, bufPtr, sizeof(long long));//ʱ���
	bufPtr = bufPtr + sizeof(long long);//�ƶ�ָ��
	memcpy(&encrypt, bufPtr, sizeof(bool));//���ܱ�ʶ
	bufPtr = bufPtr + sizeof(bool);//�ƶ�ָ��
	memcpy(&taskNum, bufPtr, sizeof(UINT32));//������
	bufPtr = bufPtr + sizeof(UINT32);//�ƶ�ָ��
	memcpy(fileName, bufPtr, 32);//�ļ���
	bufPtr = bufPtr + 32;//�ƶ�ָ��
	memcpy(expandName, bufPtr, 8);//�ļ���
	bufPtr = bufPtr + 8;//�ƶ�ָ��
	memcpy(&endFlag, bufPtr, sizeof(bool));//������־
	bufPtr = bufPtr + sizeof(bool);//�ƶ�ָ��
	size = messageLength - (bufPtr - buf);
	this->data = new char[size];
	memcpy(data, bufPtr, size);//���ݰ�
}
