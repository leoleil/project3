#pragma once
#include "Message.h"

class DefinitionMessage :
	public Message
{
public:
	DefinitionMessage();
	~DefinitionMessage();
public:
	UINT32 taskNum;//������
	char satelliteId[20];//���Ǳ��
	char equipmentName[20];//�豸����
	char equipmentNameP[20];//���豸����

};

