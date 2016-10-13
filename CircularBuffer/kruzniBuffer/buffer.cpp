#pragma once

#include "stdafx.h"
#include "buffer.h"

/*THread safe push for circ*/
bool Push(TBuffer *buffer, int number){

	EnterCriticalSection(&buffer->cs);

	if (buffer->count == buffer->bufferSize){
		LeaveCriticalSection(&buffer->cs);
		return false;
	}

	buffer->buffer[buffer->pushldx] = number;
	buffer->pushldx++;
	if (buffer->pushldx == buffer->bufferSize){
		buffer->pushldx = 0;
	}
	buffer->count++;

	LeaveCriticalSection(&buffer->cs);
	return true;
}

bool Pop(TBuffer *buffer,int *retVal){

	EnterCriticalSection(&buffer->cs);

	if (buffer->count == 0){
		LeaveCriticalSection(&buffer->cs);
		return false;
	}

	*retVal = buffer->buffer[buffer->popldx];
	buffer->popldx++;
	if (buffer->popldx == buffer->bufferSize){
		buffer->popldx = 0;
	}

	buffer->count--;

	LeaveCriticalSection(&buffer->cs);
	return true;
}

void PrintBuffer(TBuffer *buffer){

	EnterCriticalSection(&buffer->cs);
	printf("\n\n***PRINT BUFFER***\n");
	printf(" -Count: %d\n", buffer->count);
	printf(" -PopIndex: %d\n", buffer->popldx);
	printf(" -PushIndex: %d\n", buffer->pushldx);
	printf(" -Buffer SIZE: %d\n", buffer->bufferSize);
	printf("[");
	for (int i = 0; i < buffer->bufferSize; i++){
		if (i % 10 == 0) {
			printf("\n");
		}
		printf(" %d ", buffer->buffer[i]);
	}
	printf("]\n");
	LeaveCriticalSection(&buffer->cs);

}

TBuffer* ExpandBuffer(TBuffer * oldBuffer)
{
	TBuffer *retBuffer;
	InitializeBuffer(&retBuffer, oldBuffer->bufferSize * 2);//prosiruje se duplo
	
	int deltaMem = oldBuffer->bufferSize - oldBuffer->pushldx; // razlika izmedju push i buffer size

	memset(retBuffer->buffer, 0, sizeof(int) * retBuffer->bufferSize);//optional, set all values on NULL

	memcpy( retBuffer->buffer,									//dest
			oldBuffer->buffer + oldBuffer->pushldx,				//src
			deltaMem*sizeof(int));								//length
	memcpy( retBuffer->buffer + deltaMem,						//dest
			oldBuffer->buffer,									//src
			(oldBuffer->bufferSize - deltaMem)*sizeof(int));		// length
	retBuffer->count   = oldBuffer->count;
	retBuffer->pushldx = oldBuffer->count;
	
	free(oldBuffer->buffer);
	return retBuffer;
}

void DestoyBuffer(TBuffer ** buffer)
{
	free((*buffer)->buffer);
	DeleteCriticalSection(&(*buffer)->cs);
	free(*buffer);
}

void InitializeBuffer(TBuffer **buffer,int bufferSize){
	*buffer = (TBuffer*)malloc(sizeof(TBuffer));
	InitializeCriticalSection(&(*buffer)->cs);
	(*buffer)->bufferSize = bufferSize;
	(*buffer)->buffer  = (int*)malloc(sizeof(int)*bufferSize);
	(*buffer)->count   = 0;
	(*buffer)->popldx  = 0;
	(*buffer)->pushldx = 0;
}