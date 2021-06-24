#ifndef ACPI_H
#define ACPI_H

#include "Types.h"

// -------------------------------------------구조체-----------------------------------------
/*
* -----------------------------------------XSDT Related-------------------------------------
*/
#pragma pack( push, 1 )
typedef struct RSDPDESCRIPTOR {
    char Sign[8];
    BYTE Checksum;
    char OEMID[6];
    BYTE Revision;
    DWORD RSDTAddress;
} RSDP;

typedef struct RSDPDESCRIPTOR2 {
    RSDP LegacyPart;

    DWORD TableLength;
    QWORD XSDTAddress;
    BYTE ExtendedChecksum;
    BYTE Reserved[3];
} RSDP2;

typedef struct ACPIRSDTHeader {
  char Signature[4];
  DWORD Length;
  BYTE Revision;
  BYTE Checksum;
  char OEMID[6];
  char OEMTableID[8];
  DWORD OEMRevision;
  DWORD CreatorID;
  DWORD CreatorRevision;
} RSDTHeader;

typedef struct ACPIRSDT {
    RSDTHeader header;
    DWORD *PointerToOtherSDT;
} RSDT;

typedef struct ACPIXSDT {
    RSDTHeader header;
    QWORD *PointerToOtherSDT;
} XSDT;

/*
*  ----------------------------------------FADT Related-------------------------------------
*/

typedef struct GenericAddressStructure
{
  BYTE AddressSpace;
  BYTE BitWidth;
  BYTE BitOffset;
  BYTE AccessSize;
  QWORD Address;
}GAS; // GAS GAS GAS

typedef struct FADT
{
    struct   ACPIRSDTHeader h;
    DWORD FirmwareCtrl;
    DWORD DSDT;
 
    // field used in ACPI 1.0; no longer in use, for compatibility only
    BYTE Reserved;
 
    BYTE  PreferredPowerManagementProfile;
    WORD SCI_Interrupt;
    DWORD SMI_CommandPort;
    BYTE  AcpiEnable;
    BYTE  AcpiDisable;
    BYTE  S4BIOS_REQ;
    BYTE  PSTATE_Control;
    DWORD PM1aEventBlock;
    DWORD PM1bEventBlock;
    DWORD PM1aControlBlock;
    DWORD PM1bControlBlock;
    DWORD PM2ControlBlock;
    DWORD PMTimerBlock;
    DWORD GPE0Block;
    DWORD GPE1Block;
    BYTE PM1EventLength;
    BYTE PM1ControlLength;
    BYTE PM2ControlLength;
    BYTE PMTimerLength;
    BYTE GPE0Length;
    BYTE GPE1Length;
    BYTE  GPE1Base;
    BYTE  CStateControl;
    WORD WorstC2Latency;
    WORD WorstC3Latency;
    WORD FlushSize;
    WORD FlushStride;
    BYTE  DutyOffset;
    BYTE  DutyWidth;
    BYTE  DayAlarm;
    BYTE MonthAlarm;
    BYTE  Century;
 
    // reserved in ACPI 1.0; used since ACPI 2.0+
    WORD BootArchitectureFlags;
 
    BYTE  Reserved2;
    DWORD Flags;
 
    // 12 byte structure; see below for details
    GAS ResetReg;
 
    BYTE  ResetValue;
    BYTE  Reserved3[3];
 
    // 64bit pointers - Available on ACPI 2.0+
    QWORD               X_FirmwareControl;
    QWORD                X_Dsdt;
 
    GAS X_PM1aEventBlock;
    GAS X_PM1bEventBlock;
    GAS X_PM1aControlBlock;
    GAS X_PM1bControlBlock;
    GAS X_PM2ControlBlock;
    GAS X_PMTimerBlock;
    GAS X_GPE0Block;
    GAS X_GPE1Block;
} FADT;

/*
* ---------------------------------------------MADT Related----------------------------------
*/

// TBD when OS need MADT instead of MP Table

/*
* ----------------------------------------PCI Express Related--------------------------------
*/
typedef struct PCIExpressAddressSpace {
    QWORD BaseAddress;
    WORD SegmentGroupNumber;
    BYTE StartPCIBus;
    BYTE EndPCIBus;
    DWORD reserved;
} PCIEAddressSpace;

typedef struct PCIExpressConfigTable {
    RSDTHeader h;
    QWORD  reserved1;
    PCIEAddressSpace *AddressSpace;
} PCIEConfigTable;

#pragma pack( pop )
/*
* ----------------------------------------ACPI Manager, containing almost all structure above
*/
typedef struct ACPIManager {
    RSDP2* rsdp;
    XSDT* rsdt;
    FADT *fadt;
    // MADT *madt;
    // int iProcessorCount;
    BOOL bUsePICMode; // set by finding madt
    PCIEConfigTable *pcieConfigTable;
} ACPIManager;

// --------------------------------------------함수------------------------------------------
void xInitACPIMode(void);
BOOL xFindACPIFloatingPointerAddress( void );
void *xFindDescriptorTable(const char * sign);
void xParsePCIExpressInformation(void);
void xShowPCIExpressInformation( const char* pcParameterBuffer );
#endif