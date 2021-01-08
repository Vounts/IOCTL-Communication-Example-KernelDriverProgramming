#include "Data.hpp"
int main()
{

    HANDLE hDevice = CreateFile(L"\\\\.\\GED", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);
    if (hDevice == INVALID_HANDLE_VALUE)
    {
        printf("Failed to retrieve handle to device %d", GetLastError());
    }


    KERNEL_REQUEST request;
    KERNEL_RESPONSE responsed;
    DWORD returned;
    request.testData1 = 45;
    request.testData2 = 50;
    BOOL ioStatus = DeviceIoControl(hDevice, IO_GET_MODULE_ADDRESS, &request, sizeof(request), &responsed, sizeof(responsed), &returned, nullptr);
    if (ioStatus)
    {
        printf("Successfully passed the value from usermode to kernel mode\n");
        printf("The sum of %lu + %lu is: %lu \n", request.testData1, request.testData2, responsed.response);
    }
    else
    {
        printf("Failed to pass the value from usermode to kernel mode %d\n", GetLastError());
    }
    CloseHandle(hDevice);
    system("pause");
}
