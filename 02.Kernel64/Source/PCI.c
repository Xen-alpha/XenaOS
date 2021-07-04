#include "PCI.h"
#include "Utility.h"
#include "Console.h"
// 65536개의 테이블 엔트리
PCIDEVICEINFOREGISTER xPCIRegisterInfo[256*32*8];

WORD xPCIConfigReadWord(BYTE bus, BYTE slot, BYTE function, BYTE regoffset){
    DWORD address;
    DWORD lbus = (DWORD) bus;
    DWORD lslot = (DWORD) slot;
    DWORD lfunc = (DWORD) function;
    WORD tmp = 0;

    address = (DWORD)((lbus << 16) | (lslot << 11) | (lfunc << 8) | (regoffset & 0xfc) | ((DWORD) 0x80000000));

    kOutPortDword(0xCF8, address);

    tmp = (WORD) ((kInPortDword(0xCFC) >> ((regoffset & 2) * 8)) & 0xffff);
    return tmp;
}

void xPCIConfigWriteWord(BYTE bus, BYTE slot, BYTE function, BYTE regoffset, WORD value){
    DWORD address;
    DWORD lbus = (DWORD) bus;
    DWORD lslot = (DWORD) slot;
    DWORD lfunc = (DWORD) function;

    address = (DWORD)((lbus << 16) | (lslot << 11) | (lfunc << 8) | (regoffset & 0xfc) | ((DWORD) 0x80000000));

    kOutPortDword(0xCF8, address);
    kOutPortWord(0xCFC, value);
    
    return;
}

void checkBus(BYTE bus) {
    BYTE device;

    for(device = 0; device < 32; device++) {
        checkDevice(bus, device);
    }
}

void checkDevice(BYTE bus, BYTE device) {
    BYTE function = 0;

    WORD vendorID = xPCIConfigReadWord(bus, device, function, 0);
    if(vendorID == 0xFFFF) return;        // Device doesn't exist
    checkFunction(bus, device, function);
    BYTE headerType = (BYTE) xPCIConfigReadWord(bus, device, function, 14);
    if( (headerType & 0x80) != 0) {
        /* It is a multi-function device, so check remaining functions */
        for(function = 1; function < 8; function++) {
            if(xPCIConfigReadWord(bus, device, function, 0) != 0xFFFF) {
                checkFunction(bus, device, function);
            }
        }
    }
}

void checkFunction(BYTE bus, BYTE device, BYTE function) {
    BYTE baseClass;
    BYTE subClass;
    BYTE secondaryBus;
    BYTE progif;

    baseClass = xPCIConfigReadWord(bus, device, function, 10) >> 8;
    subClass = xPCIConfigReadWord(bus, device, function, 10);
    progif = xPCIConfigReadWord(bus, device, function, 8) >> 8;
    if( (baseClass == 0x06) && (subClass == 0x04) ) { // this is pci-to-pci bridge, it may have header type 0x1
        secondaryBus = xPCIConfigReadWord(bus, device, function, 24) >> 8;
        checkBus(secondaryBus);
    }
    // print result
    kPrintf("PCI bus: 0x%x, PCI device: 0x%x, PCI function: 0x%x\n", bus,device,function);
    kPrintf("Base Class: 0x%x, Sub Class: 0x%x, progIF: 0x%x\n", baseClass, subClass, progif);
    xPCIRegisterInfo[bus*32+device*8+function].bus = bus;
    xPCIRegisterInfo[bus*32+device*8+function].device = device;
    xPCIRegisterInfo[bus*32+device*8+function].function = function;
    xPCIRegisterInfo[bus*32+device*8+function].vendorID = xPCIConfigReadWord(bus, device, function, 0);
    xPCIRegisterInfo[bus*32+device*8+function].deviceID = xPCIConfigReadWord(bus, device, function, 2);
    xPCIRegisterInfo[bus*32+device*8+function].command = xPCIConfigReadWord(bus, device, function, 4);
    xPCIRegisterInfo[bus*32+device*8+function].status = xPCIConfigReadWord(bus, device, function, 6);
    xPCIRegisterInfo[bus*32+device*8+function].RevID = xPCIConfigReadWord(bus, device, function, 8);
    xPCIRegisterInfo[bus*32+device*8+function].ProgIF = progif;
    xPCIRegisterInfo[bus*32+device*8+function].ClassCode = baseClass;
    xPCIRegisterInfo[bus*32+device*8+function].SubClassCode = subClass;
    xPCIRegisterInfo[bus*32+device*8+function].CacheLineSize = xPCIConfigReadWord(bus, device, function, 12);
    xPCIRegisterInfo[bus*32+device*8+function].LatencyTimer = xPCIConfigReadWord(bus, device, function, 12) >> 8;
    xPCIRegisterInfo[bus*32+device*8+function].HeaderType = xPCIConfigReadWord(bus, device, function, 14);
    xPCIRegisterInfo[bus*32+device*8+function].Bist = xPCIConfigReadWord(bus, device, function, 14) >> 8;
    if (xPCIRegisterInfo[bus*32+device*8+function].HeaderType == 0x02){
        for (int i = 0; i < 14; i++){
            xPCIRegisterInfo[bus*32+device*8+function].etcData[i] = ((DWORD) xPCIConfigReadWord(bus, device, function, 0x12 + i) << 16) + xPCIConfigReadWord(bus, device, function, 0x10 + i);
        }
    } else {
        for (int i = 0; i < 12; i++){
            xPCIRegisterInfo[bus*32+device*8+function].etcData[i] = ((DWORD) xPCIConfigReadWord(bus, device, function, 0x12 + i) << 16) + xPCIConfigReadWord(bus, device, function, 0x10 + i);
        }
    }
}

void xGetAndParsePCIRegisterInfo(void) {
    BYTE function;
    BYTE bus;

    BYTE headerType = xPCIConfigReadWord(0, 0, 0, 14);
    if( (headerType & 0x80) == 0) {
        /* Single PCI host controller */
        checkBus(0);
    } else {
        /* Multiple PCI host controllers */
        for(function = 0; function < 8; function++) {
            if(xPCIConfigReadWord(0, 0, function, 0) != 0xFFFF) break;
            bus = function;
            checkBus(bus);
        }
    }
    return;
}

void xPrintPCIRegisterInfo(BYTE bus, BYTE slot, BYTE func) {
    kPrintf("bus 0x%x, slot 0x%x, func 0x%x\n", bus, slot, func);
    kPrintf("0x%x 0x%x\n", xPCIConfigReadWord(bus, slot, func, 0x02), xPCIConfigReadWord(bus, slot, func, 0x00));
    kPrintf("0x%x 0x%x\n", xPCIConfigReadWord(bus, slot, func, 0x06), xPCIConfigReadWord(bus, slot, func, 0x04));
    kPrintf("0x%x 0x%x\n", xPCIConfigReadWord(bus, slot, func, 0x0A), xPCIConfigReadWord(bus, slot, func, 0x08));
    kPrintf("0x%x 0x%x\n", xPCIConfigReadWord(bus, slot, func, 0x0E), xPCIConfigReadWord(bus, slot, func, 0x0C));
    kPrintf("0x%x 0x%x\n", xPCIConfigReadWord(bus, slot, func, 0x12), xPCIConfigReadWord(bus, slot, func, 0x10));
    kPrintf("0x%x 0x%x\n", xPCIConfigReadWord(bus, slot, func, 0x18), xPCIConfigReadWord(bus, slot, func, 0x14));
    kPrintf("0x%x 0x%x\n", xPCIConfigReadWord(bus, slot, func, 0x1A), xPCIConfigReadWord(bus, slot, func, 0x18));
    kPrintf("0x%x 0x%x\n", xPCIConfigReadWord(bus, slot, func, 0x1E), xPCIConfigReadWord(bus, slot, func, 0x1C));
    kPrintf("0x%x 0x%x\n", xPCIConfigReadWord(bus, slot, func, 0x22), xPCIConfigReadWord(bus, slot, func, 0x20));
    kPrintf("0x%x 0x%x\n", xPCIConfigReadWord(bus, slot, func, 0x26), xPCIConfigReadWord(bus, slot, func, 0x24));
    return;
}


