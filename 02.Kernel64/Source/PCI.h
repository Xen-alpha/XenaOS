/*
 *  file    PCI.h
 *  date    2019/08/31
 *  author  Xena
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   PCI Management
 */

#ifndef __PCI_H__
#define __PCI_H__

#include "Types.h"
#include "MPConfigurationTable.h"

////////////////////////////////////////////////////////////////////////////////
//
// 변수
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
// 매크로
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
//
// 구조체
//
////////////////////////////////////////////////////////////////////////////////

typedef struct xPCIDEVICEINFOREGISTER{
    BYTE bus;
    BYTE device;
    BYTE function;
    
    // initial header info, only used in bootstraping
    WORD vendorID;
    WORD deviceID;
    WORD command;
    WORD status;
    BYTE RevID;
    BYTE ProgIF;
    BYTE SubClassCode;
    BYTE ClassCode;
    BYTE CacheLineSize;
    BYTE LatencyTimer;
    BYTE HeaderType;
    BYTE Bist;
    DWORD etcData[14]; // diffrent between diffrent HeaderType
} PCIDEVICEINFOREGISTER;

extern PCIDEVICEINFOREGISTER xPCIRegisterInfo[256*32*8];
////////////////////////////////////////////////////////////////////////////////
//
// 함수
//
////////////////////////////////////////////////////////////////////////////////

WORD xPCIConfigReadWord(BYTE bus, BYTE slot, BYTE function, BYTE regoffset);
void xPCIConfigWriteWord(BYTE bus, BYTE slot, BYTE function, BYTE regoffset, WORD value);
void checkFunction(BYTE bus, BYTE device, BYTE function);
void checkDevice(BYTE bus, BYTE device);
void checkBus(BYTE bus);

void xGetAndParsePCIRegisterInfo(void);
void xPrintPCIRegisterInfo(BYTE bus, BYTE slot, BYTE func);
#endif