#include "assign.h"

DWORD WINAPI assign(LPVOID lpParameter) {
	
	while (1) {
		AssignmentSocket service;//���������������
		service.createReceiveServer(4998, MESSAGES);
	}
	return 0;
}

