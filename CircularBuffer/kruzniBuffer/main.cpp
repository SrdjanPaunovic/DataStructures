// kruzniBuffer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "buffer.h"

#define BUFFER_SIZE 10
DWORD WINAPI loadBuffer(LPVOID lpParam);
DWORD WINAPI unloadBuffer(LPVOID lpParam);

int _tmain(int argc, _TCHAR* argv[])
{
	TBuffer *buffer;
	InitializeBuffer(&buffer, BUFFER_SIZE);
	HANDLE read, write;


	read = CreateThread(NULL, 0, &unloadBuffer, &buffer, 0, NULL);
	write = CreateThread(NULL, 0, &loadBuffer, &buffer, 0, NULL);

	getchar();

	CloseHandle(read);
	CloseHandle(write);

	PrintBuffer(buffer);

	getchar();

	DestoyBuffer(&buffer);
	return 0;
}

DWORD WINAPI loadBuffer(LPVOID lpParam) {
	TBuffer **buffer = (TBuffer **)lpParam;
	for (int i = 0; i < 100;i++) {
		if (Push(*buffer, i)) {
			printf("\n -Sucessfully added to buffer");
		}
		else {
			
			i--; //vraca se da upise element ponovo 
			printf("\n -Buffer full->Expanding");
			*buffer=ExpandBuffer(*buffer);
			printf("\n -Buffer Expanded!");

			PrintBuffer(*buffer);
		}
	}
	return 0;
}

DWORD WINAPI unloadBuffer(LPVOID lpParam) {
	TBuffer **buffer = (TBuffer **)lpParam;
	for (int i = 0; i < 100;i++) {
		int retValue;
		if (Pop(*buffer, &retValue)) {
			printf("\n -Sucessfully POP from baffer");
			printf("\n -Value = %d",retValue);
		}
		else {
			i--;//probaj ponovo
			printf("\n -Buffer is Empty");
		}
	}
	return 0;
}
