#pragma once

#include <iostream>
#include <Windows.h>


#define IO_GET_MODULE_ADDRESS CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)


struct KERNEL_REQUEST
{
	ULONG testData1;
	ULONG testData2;
};

struct KERNEL_RESPONSE
{
	ULONG response;
};