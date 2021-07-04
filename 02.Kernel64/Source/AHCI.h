#ifndef AHCI_H
#define AHCI_H

#include "Types.h"
#include "Synchronization.h"

// -----------------------------------------------매크로------------------------------------------------------
#define AHCI_PORT_OFFSET 0x0100 // port location offset
#define	AHCI_BASE	0x400000	// 4M
 
#define HBA_PxCMD_ST    0x0001
#define HBA_PxCMD_FRE   0x0010
#define HBA_PxCMD_FR    0x4000
#define HBA_PxCMD_CR    0x8000

#define MAXDEVICEDATALENGTH 1024
// ---------------------------------------------- 구조체 --------------------------------------------------------

#pragma pack(push, 1)


#pragma pack(pop)

typedef struct xAHCIINFO {
    WORD PCIIndex;
	int bus;
	int slot;
	int func;
	BYTE *msi;
	DWORD msiaddress;
	MUTEX stAHCIMutex;
} AHCIINFO;

extern AHCIINFO *ahcihandler;

BOOL xInitializeAHCI(void);
BOOL identify_Storage(void);
DWORD arch_msi_address(QWORD* data, BYTE vector, DWORD processor, BYTE edgetrigger, BYTE deassert, BYTE deliveryMode);
#endif