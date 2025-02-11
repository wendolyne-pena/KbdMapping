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
            if (keyData[i].MakeCode == 0x09) {  // Maps "Tab" (Enter)
                keyData[i].MakeCode = 0x0D;  // Changes it to "Enter"
            }
            if (keyData[i].MakeCode == 0xBF) {  // Maps "/" (00)
                keyData[i].MakeCode = 0xBE;  // Changes it to "."
            }
			if (keyData[i].MakeCode == 0x1B) {  // Maps "Esc" (up arrow)
				keyData[i].MakeCode = 0x26;  // Changes it to "Up arrow"
            }
            if (keyData[i].MakeCode == 0x91) {  // Maps "Scroll Lock" (down arrow)
                keyData[i].MakeCode = 0x28;  // Changes it to "Down arrow"
            }
            if (keyData[i].MakeCode == 0x08) {  // Maps "Backspace" (sign on/off)
                keyData[i].MakeCode = 0x7D;  // Changes it to "F14"
            }
			if (keyData[i].MakeCode == 0x70) {  // Maps "F1" (Dept)
				keyData[i].MakeCode = 0x7E;  // Changes it to "F15"
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
