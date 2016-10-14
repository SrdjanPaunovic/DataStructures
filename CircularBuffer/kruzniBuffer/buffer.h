#include "stdafx.h"

#define MIN_BUFFER_SIZE 10
#define MAX_BUFFER_SIZE 10240

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
void ExpandBuffer(TBuffer* oldBuffer);
void NarrowBuffer(TBuffer* oldBuffer);
void DestoyBuffer(TBuffer **buffer);

