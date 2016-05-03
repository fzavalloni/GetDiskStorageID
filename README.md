# GetDiskStorageID

Program which get all phisycal information about a disk. It was built specially to get the Disk ID of
iSCSI Disks in a Windows Server. 
The Windows Server does not have anything by design to bring this information.

# Parameters

```
GetDiskStorageID.exe \\.\PHYSICALDRIVE1
```

The parameter \\.\PHYSICALDRIVE1 match with the disk1, \\.\PHYSICALDRIVE2 with disk2 and on.

# Requisites

The client host must have the *Microsoft Visual C++ Redistributable Package* installed.

