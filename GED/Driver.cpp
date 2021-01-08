
#include "Driver.hpp"

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath)
{
	UNREFERENCED_PARAMETER(pRegistryPath);

	KdPrint(("GED Driver Entry success!"));

	pDriverObject->DriverUnload = DriverUnloadRoutine;
	pDriverObject->MajorFunction[IRP_MJ_CREATE] = CreateCall;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = CloseCall;
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IoDeviceControl;

	NTSTATUS status = IoCreateDevice(pDriverObject, 0, &deviceName, FILE_DEVICE_UNKNOWN, 0, FALSE, &DeviceObject);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("Failed to create device! (0x%08X)", status));
		return status;
	}

	status = IoCreateSymbolicLink(&symLink, &deviceName);
	if (!NT_SUCCESS(status))
	{
		KdPrint(("Failed to create symbolic Link! (0x%08X)", status));
		IoDeleteDevice(DeviceObject);
		return status;
	}

	return STATUS_SUCCESS;
}

void DriverUnloadRoutine(PDRIVER_OBJECT pDriverObject) 
{
	UNREFERENCED_PARAMETER(pDriverObject);

	IoDeleteSymbolicLink(&symLink);
	IoDeleteDevice(pDriverObject->DeviceObject);


	KdPrint(("Successfully unloaded driver..."));
}


NTSTATUS CreateCall(PDEVICE_OBJECT pDeviceObject, PIRP Irp)
{
	UNREFERENCED_PARAMETER(pDeviceObject);
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	KdPrint(("Connection Established to the driver"));
	return STATUS_SUCCESS;
}
NTSTATUS CloseCall(PDEVICE_OBJECT pDeviceObject, PIRP Irp)
{
	UNREFERENCED_PARAMETER(pDeviceObject);
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	KdPrint(("Disconnected communication to the driver"));
	return STATUS_SUCCESS;
}
NTSTATUS IoDeviceControl(PDEVICE_OBJECT pDeviceObject, PIRP Irp)
{
	UNREFERENCED_PARAMETER(pDeviceObject);
	auto stack = IoGetCurrentIrpStackLocation(Irp);
	auto status = STATUS_UNSUCCESSFUL;
	auto controlCode = stack->Parameters.DeviceIoControl.IoControlCode;
	if (controlCode == IO_GET_MODULE_ADDRESS)
	{
		auto data = (KERNEL_REQUEST*)Irp->AssociatedIrp.SystemBuffer;
		auto responseKing = (KERNEL_RESPONSE*)Irp->AssociatedIrp.SystemBuffer;
		if (data == nullptr || responseKing == nullptr)
		{
			KdPrint(("Input buffer did not received from user mode nullptr"));
			Irp->IoStatus.Information = 0;
			return status;
		}
		if (data->testData1 == NULL || data->testData2 == NULL)
		{
			KdPrint(("Input buffer did not received "));
			Irp->IoStatus.Information = 0;
			return status;
		}
		if (data->testData1 && data->testData2)
		{
			KdPrint(("The input buffer data 1 is: %lu", data->testData1));
			KdPrint(("The input buffer data 2 is: %lu", data->testData2));
			responseKing->response = data->testData1 + data->testData2;
			KdPrint(("Input added is: %lu", responseKing->response));
			status = STATUS_SUCCESS;
			Irp->IoStatus.Information = sizeof(responseKing);
		}
	}

	Irp->IoStatus.Status = status;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return status;
}