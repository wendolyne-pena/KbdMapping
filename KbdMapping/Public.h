/*++

Module Name:

    public.h

Abstract:

    This module contains the common declarations shared by driver
    and user applications.

Environment:

    user and kernel

--*/

//
// Define an Interface Guid so that app can find the device and talk to it.
//

DEFINE_GUID (GUID_DEVINTERFACE_KbdMapping,
    0xa918e4eb,0x1db9,0x44de,0x8f,0xbf,0x24,0x74,0x44,0x5b,0x5f,0xbe);
// {a918e4eb-1db9-44de-8fbf-2474445b5fbe}
