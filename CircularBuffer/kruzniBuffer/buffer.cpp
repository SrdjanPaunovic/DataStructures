#pragma once

#include "stdafx.h"
#include "buffer.h"

/*THread safe push for circ*/
bool Push(TBuffer *buffer, int number){

	EnterCriticalSection(&buffer->cs);

	if (buffer->count == buffer->bufferSize){
		if (buffer->bufferSize >= MAX_BUFFER_SIZE) {
			LeaveCriticalSection(&buffer->cs);
			return false;
		}
		printf("\nExpanding buffer!");
		ExpandBuffer(buffer);
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

	int usageInPercent = ((float)buffer->count / (float)buffer->bufferSize) * 100; 
	if (usageInPercent < 25 && buffer->bufferSize > MIN_BUFFER_SIZE) {
		//when buffer is under 25% full and bufferSize bigger than minimal value of bufferSize
		printf("\nBuffer is %d%% full,buffer size: %d", usageInPercent, buffer->bufferSize);
		printf("\nBuffer is narrowing..");
		NarrowBuffer(buffer);
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

void ExpandBuffer(TBuffer * circBuffer)
{
	int newBufferSize = circBuffer->bufferSize * 2;
	int *newBuffer = (int*)calloc(newBufferSize,sizeof(int));
	//InitializeBuffer(&retBuffer, oldBuffer->bufferSize * 2);//prosiruje se duplo
	
	int deltaMem = circBuffer->bufferSize - circBuffer->pushldx; // razlika izmedju push i buffer size

	memcpy( newBuffer,									//dest
			circBuffer->buffer + circBuffer->pushldx,				//src
			deltaMem*sizeof(int));								//length
	memcpy( newBuffer + deltaMem,						//dest
			circBuffer->buffer,									//src
			(circBuffer->bufferSize - deltaMem)*sizeof(int));		// length

	free(circBuffer->buffer);

	circBuffer->popldx = 0;
	circBuffer->pushldx = circBuffer->count;
	circBuffer->buffer = newBuffer;
	circBuffer->bufferSize = newBufferSize;

}

void NarrowBuffer(TBuffer * circBuffer)
{
	int newBufferSize = circBuffer->bufferSize / 2;
	int *newBuffer    = (int*)calloc(newBufferSize, sizeof(int));


	if (circBuffer->popldx <= circBuffer->pushldx) {
		//just move data beetwen popIdx and pushIdx to newBuffer
		int deltaMem = circBuffer->pushldx - circBuffer->popldx;
		memcpy( newBuffer,												//dest
				circBuffer->buffer + circBuffer->popldx,				//src
				deltaMem * sizeof(int));								//length
	}
	else {
		//first move data beetwen popIdx and bufferSize to newBuffer
		//than data beetwen 0 and pushIdx from oldBuffer to newBuffer
		int deltaMem = circBuffer->bufferSize - circBuffer->popldx;
		memcpy(	newBuffer,												//dest
				circBuffer->buffer + circBuffer->popldx,				//src
				deltaMem * sizeof(int));								//length

		memcpy(	newBuffer + deltaMem,									//dest
				circBuffer->buffer,										//src
				circBuffer->pushldx * sizeof(int));						//length beetwen 0 and pushIdx
	}

	circBuffer->popldx = 0;
	circBuffer->pushldx = circBuffer->count;
	circBuffer->buffer = newBuffer;
	circBuffer->bufferSize = newBufferSize;
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