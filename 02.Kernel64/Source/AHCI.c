#include "AHCI.h"
#include "Utility.h"
#include "PCI.h"
#include "DynamicMemory.h"
#include "Console.h"

#define HBA_PxCMD_ST    0x0001
#define HBA_PxCMD_FRE   0x0010
#define HBA_PxCMD_FR    0x4000
#define HBA_PxCMD_CR    0x8000

AHCIINFO *ahcihandler;

BOOL xInitializeAHCI(void) {
    BYTE intLine=0xff;
    WORD command;
    for (int i = 0 ; i < 65536; i++){
        if (xPCIRegisterInfo[i].ClassCode == 0x01 && xPCIRegisterInfo[i].SubClassCode == 0x06 && xPCIRegisterInfo[i].ProgIF == 0x01){
            ahcihandler->bus = xPCIRegisterInfo[i].bus;
            ahcihandler->slot= xPCIRegisterInfo[i].device;
            ahcihandler->func = xPCIRegisterInfo[i].function;
            ahcihandler->PCIIndex = i;
            break;
        }
    }
    // disable msi for now
    WORD info = xPCIConfigReadWord(ahcihandler->bus, ahcihandler->slot, ahcihandler->func, 0x34);
    for (BYTE ptr = xPCIConfigReadWord(ahcihandler->bus, ahcihandler->slot, ahcihandler->func, info) >> 8; ptr != NULL; ptr = xPCIConfigReadWord(ahcihandler->bus, ahcihandler->slot, ahcihandler->func, ptr)) {
        if( (xPCIConfigReadWord(ahcihandler->bus, ahcihandler->slot, ahcihandler->func, info) & 0x00ff) == 0x05){
            ahcihandler->msi = info;
            xPCIConfigWriteWord(ahcihandler->bus, ahcihandler->slot, ahcihandler->func, ahcihandler->msi + 0x02, 0x0001);
            break;
        }
        info = (WORD) ptr;
    }
    return TRUE;
}

DWORD arch_msi_address(QWORD* data, BYTE vector, DWORD processor, BYTE edgetrigger, BYTE deassert, BYTE deliveryMode)
{
	*data = (vector & 0xFF) | (edgetrigger == 1 ? 0 : (1 << 15)) | (deassert == 1 ? 0 : (1 << 14)) | ((deliveryMode & 0x07) << 8);
    xPCIConfigWriteWord(ahcihandler->bus, ahcihandler->slot, ahcihandler->func, ahcihandler->msi, data);
	return (0xFEE00000 | (processor << 12) );
}

BOOL identify_Storage (void){
    WORD info = xPCIConfigReadWord(ahcihandler->bus, ahcihandler->slot, ahcihandler->func, 0x34);
    for (BYTE ptr = xPCIConfigReadWord(ahcihandler->bus, ahcihandler->slot, ahcihandler->func, info) >> 8; ptr != NULL; ptr = xPCIConfigReadWord(ahcihandler->bus, ahcihandler->slot, ahcihandler->func, ptr)) {
        if( (xPCIConfigReadWord(ahcihandler->bus, ahcihandler->slot, ahcihandler->func, info) & 0x00ff) == 0x05){
            QWORD *datavalue = kAllocateMemory(sizeof(QWORD));
            ahcihandler->msiaddress = arch_msi_address(datavalue, 41, 0x00, 1, 0, 0x00);
            xPCIConfigWriteWord(ahcihandler->bus, ahcihandler->slot, ahcihandler->func, ahcihandler->msi + 0x04, (WORD) ahcihandler->msiaddress);
            xPCIConfigWriteWord(ahcihandler->bus, ahcihandler->slot, ahcihandler->func, ahcihandler->msi + 0x06, (WORD) (ahcihandler->msiaddress >> 16));
            xPCIConfigWriteWord(ahcihandler->bus, ahcihandler->slot, ahcihandler->func, ahcihandler->msi + 0x08, (WORD) *datavalue);

            *((QWORD *)( ahcihandler->msiaddress + 0x0300)) = *datavalue;
            while (*((QWORD *)( ahcihandler->msiaddress + 0x0300)) & (1 << 12)) kPrintf("interrupt\n");
            kFreeMemory(datavalue);
            datavalue = NULL;
        }
        info = (WORD) ptr;
    }
    
    return TRUE;
}