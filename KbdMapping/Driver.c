#include <ntddk.h>
#include <wdf.h>
#include <kbdmou.h>
#include <ntstrsafe.h>

PDEVICE_OBJECT g_KeyboardDevice = NULL;
PDRIVER_DISPATCH OriginalReadHandler = NULL;
UNICODE_STRING TargetDeviceId = RTL_CONSTANT_STRING(L"HID\VID_04B3&PID_4673&REV_0414&MI_00"); // Keyboard ID

NTSTATUS KeyboardReadHook(
    PDEVICE_OBJECT DeviceObject,
    PIRP Irp
) {
    PIO_STACK_LOCATION irpSp = IoGetCurrentIrpStackLocation(Irp);
    if (irpSp && irpSp->Parameters.Read.Length > 0) {
        PKEYBOARD_INPUT_DATA keyData = (PKEYBOARD_INPUT_DATA)Irp->AssociatedIrp.SystemBuffer;
        ULONG numKeys = irpSp->Parameters.Read.Length / sizeof(KEYBOARD_INPUT_DATA);

        for (ULONG i = 0; i < numKeys; i++) {
            if (keyData[i].MakeCode == 0xA0) {  // Example, maps Left Shift
                keyData[i].MakeCode = 0x30;  // Changes it to letter B
            }
        }
    }
    return OriginalReadHandler(DeviceObject, Irp);
}

NTSTATUS DriverEntry(
    PDRIVER_OBJECT DriverObject,
    PUNICODE_STRING RegistryPath
) {
    UNREFERENCED_PARAMETER(RegistryPath);

    NTSTATUS status;
    PDEVICE_OBJECT deviceObject;
    UNICODE_STRING DeviceId;

	RtlInitUnicodeString(&DeviceId, L"HID\VID_04B3&PID_4673&REV_0414&MI_00"); // Keyboard ID
    if (!RtlEqualUnicodeString(&DeviceId, &TargetDeviceId, TRUE)) {
        return STATUS_UNSUCCESSFUL; 
    }

    status = IoCreateDevice(
        DriverObject,
        0,
        NULL,
        FILE_DEVICE_KEYBOARD,
        0,
        FALSE,
        &deviceObject
    );

    if (!NT_SUCCESS(status)) {
        return status;
    }

    g_KeyboardDevice = deviceObject;
    OriginalReadHandler = DriverObject->MajorFunction[IRP_MJ_READ];
    DriverObject->MajorFunction[IRP_MJ_READ] = KeyboardReadHook;

    return STATUS_SUCCESS;
}
