#include <windows.h>
#include <stdio.h>
#include <ntddstor.h>


// Query the Page 80 information
void QueryPage80(LPWSTR pwszDiskDevice, HANDLE hDiskDevice)
{
 
    // The input parameter 
    STORAGE_PROPERTY_QUERY query;
    query.PropertyId = StorageDeviceProperty;
    query.QueryType = PropertyStandardQuery;
 
    // Prepare a large output buffer (good enough in our sample code)
    BYTE bOutputBuffer[8192];
    DWORD returnedLength;
    if (!DeviceIoControl(
        hDiskDevice,                
        IOCTL_STORAGE_QUERY_PROPERTY,
        &query,
        sizeof( STORAGE_PROPERTY_QUERY ),
        &bOutputBuffer,                   
        sizeof(bOutputBuffer),                      
        &returnedLength,      
        NULL                    
        ))
    {
        wprintf(L"\nERROR: Cannot request Page #80 information for device '%s'.\n "
            L"[DeviceIoControl() error: %d]\n", 
            pwszDiskDevice, GetLastError()); 
        return;
    }
 
    //
    //  Get some basic data about our disk device
    //
 
    STORAGE_DEVICE_DESCRIPTOR *pDesc = (PSTORAGE_DEVICE_DESCRIPTOR) bOutputBuffer;
 
    //  Get the Page 80 information
    //  This code assumes zero-terminated strings, according to the spec
 
    if (pDesc->VendorIdOffset != 0)
        wprintf(L"- Page80.VendorId: %hs\n", (PCHAR)((PBYTE)pDesc + pDesc->VendorIdOffset));
 
    if (pDesc->ProductIdOffset != 0)
        wprintf(L"- Page80.ProductId: %hs\n", (PCHAR)((PBYTE)pDesc + pDesc->ProductIdOffset));
 
    if (pDesc->ProductRevisionOffset != 0)
        wprintf(L"- Page80.ProductRevision: %hs\n", (PCHAR)((PBYTE)pDesc + pDesc->ProductRevisionOffset));
 
    if (pDesc->SerialNumberOffset != 0)
        wprintf(L"- Page80.SerialNumber: %hs\n", (PCHAR)((PBYTE)pDesc + pDesc->SerialNumberOffset));
}
 
 
// Query the Page 83 information
void QueryPage83(LPWSTR pwszDiskDevice, HANDLE hDiskDevice)
{
 
    // The input parameter 
    STORAGE_PROPERTY_QUERY query;
    query.PropertyId = StorageDeviceIdProperty;
    query.QueryType = PropertyStandardQuery;
 
    // Prepare a large output buffer (good enough in our sample code)
    BYTE bOutputBuffer[8192];
    DWORD returnedLength;
    if (!DeviceIoControl(
        hDiskDevice,                
        IOCTL_STORAGE_QUERY_PROPERTY,
        &query,
        sizeof( STORAGE_PROPERTY_QUERY ),
        &bOutputBuffer,                   
        sizeof(bOutputBuffer),                      
        &returnedLength,      
        NULL                    
        ))
    {
        wprintf(L"\nERROR: Cannot request SCSI Inquiry VPD Page #83 information for device '%s'.\n"
            L"Maybe it doesn't support Page 83?\n"
            L"[DeviceIoControl() error: %d]\n", 
            pwszDiskDevice, GetLastError()); 
        return;
    }
 
    STORAGE_DEVICE_ID_DESCRIPTOR *pDesc = (PSTORAGE_DEVICE_ID_DESCRIPTOR) bOutputBuffer;
 
    // Listing all identifiers...
    wprintf(L"\n- Page83.NumberOfIdentifiers: %d\n", pDesc->NumberOfIdentifiers);
 
    STORAGE_IDENTIFIER *pId = (PSTORAGE_IDENTIFIER) pDesc->Identifiers;
    for(UINT i = 0; i < pDesc->NumberOfIdentifiers; i++)
    {
        // Checks if this Identifier is unique
        bool isUnique = false;
        if (pId->Association == StorageIdAssocDevice)
        {
            if (((INT)pId->Type == StorageIdTypeVendorId)
                || ((INT)pId->Type == StorageIdTypeEUI64)
                || ((INT)pId->Type == StorageIdTypeFCPHName)
                || ((INT)pId->Type == StorageIdTypeScsiNameString))
            {
                isUnique = true;
            }
        }
 
        wprintf(L"\n- Page83.Identifier\n", i);
        wprintf(L"   - Type: %d\n", pId->Type);
        wprintf(L"   - Association: %d\n", pId->Association);
        wprintf(L"   - Size: %d\n", pId->IdentifierSize);
        wprintf(L"   - IsGloballyUnique? %s\n", isUnique? L"TRUE": L"FALSE");
 
        wprintf(L"   - Data: ");
        for(int j = 0; j < pId->IdentifierSize; j++)
            wprintf(L"%02hx ", pId->Identifier[j]);
        wprintf(L"\n");
 
        // move to next identifier
        pId = (PSTORAGE_IDENTIFIER) ((BYTE *) pId + pId->NextOffset);
    }
}
 
 
// Entry point
extern "C" void wmain(int argc, WCHAR ** argv)
{
    if (argc != 2)
    {
        wprintf(L"- You must specify a disk device! as argument!\n"); 
        return;
    }
 
    WCHAR * pwszDiskDevice = argv[1];
 
    wprintf(L"Querying information for disk '%s' ... \n", pwszDiskDevice);
 
    // Opening a handle to the disk  
    HANDLE hDiskDevice = CreateFile(
        pwszDiskDevice,
        GENERIC_READ | GENERIC_WRITE,       // dwDesiredAccess
        FILE_SHARE_READ | FILE_SHARE_WRITE, // dwShareMode
        NULL,                               // lpSecurityAttributes
        OPEN_EXISTING,                      // dwCreationDistribution
        0,                                  // dwFlagsAndAttributes
        NULL                                // hTemplateFile
        );
 
    if (hDiskDevice == INVALID_HANDLE_VALUE)
    {
        wprintf(L"\nERROR: Cannot open device '%s'. "
            L"Did you specified a correct SCSI device? "
            L"[CreateFile() error: %d]\n", 
            pwszDiskDevice, GetLastError()); 
        return;
    }
 
    // Query the Page 80 information
    QueryPage80(pwszDiskDevice, hDiskDevice);
 
    // Query the Page 83 information
    QueryPage83(pwszDiskDevice, hDiskDevice);

}
 