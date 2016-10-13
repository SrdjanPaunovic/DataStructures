#include "stdafx.h"

typedef struct {
	int *buffer;
	int pushldx;
	int popldx;
	int count;
	int bufferSize;
	CRITICAL_SECTION cs;
}TBuffer;


bool Push(TBuffer *buffer, int number);
bool Pop(TBuffer *buffer, int *retVal);
void InitializeBuffer(TBuffer **buffer, int bufferSize);
void PrintBuffer(TBuffer *buffer);
TBuffer* ExpandBuffer(TBuffer* oldBuffer);
void DestoyBuffer(TBuffer **buffer);

