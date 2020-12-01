/*
---------------------XenaOS AHCI & SATA controller driver
---------------------Author: Xenα, 2020/10/30
*/
#ifndef __SATA_H__
#define __SATA_H__

#include "Types.h"
#include "Console.h"
#include "Utility.h"
#include "PCI.h"

typedef enum
{
	FIS_TYPE_REG_H2D	= 0x27,	// Register FIS - host to device
	FIS_TYPE_REG_D2H	= 0x34,	// Register FIS - device to host
	FIS_TYPE_DMA_ACT	= 0x39,	// DMA activate FIS - device to host
	FIS_TYPE_DMA_SETUP	= 0x41,	// DMA setup FIS - bidirectional
	FIS_TYPE_DATA		= 0x46,	// Data FIS - bidirectional
	FIS_TYPE_BIST		= 0x58,	// BIST activate FIS - bidirectional
	FIS_TYPE_PIO_SETUP	= 0x5F,	// PIO setup FIS - device to host
	FIS_TYPE_DEV_BITS	= 0xA1,	// Set device bits FIS - device to host
} FIS_TYPE;


#pragma pack( push, 1 )
typedef struct tagFIS_REG_H2D
{
	// DWORD 0
	BYTE  fis_type;	// FIS_TYPE_REG_H2D
 
	BYTE  pmport:4;	// Port multiplier
	BYTE  rsv0:3;		// Reserved
	BYTE  c:1;		// 1: Command, 0: Control
 
	BYTE  command;	// Command register
	BYTE  featurel;	// Feature register, 7:0
 
	// DWORD 1
	BYTE  lba0;		// LBA low register, 7:0
	BYTE  lba1;		// LBA mid register, 15:8
	BYTE  lba2;		// LBA high register, 23:16
	BYTE  device;		// Device register
 
	// DWORD 2
	BYTE  lba3;		// LBA register, 31:24
	BYTE  lba4;		// LBA register, 39:32
    BYTE  lba5;		// LBA register, 47:40
	BYTE  featureh;	// Feature register, 15:8
 
	// DWORD 3
	BYTE  countl;		// Count register, 7:0
	BYTE  counth;		// Count register, 15:8
	BYTE  icc;		// Isochronous command completion
	BYTE  control;	// Control register
 
	// DWORD 4
	BYTE  rsv1[4];	// Reserved
} FIS_REG_H2D;

typedef struct tagFIS_REG_D2H
{
	// DWORD 0
	BYTE  fis_type;    // FIS_TYPE_REG_D2H
 
	BYTE  pmport:4;    // Port multiplier
	BYTE  rsv0:2;      // Reserved
	BYTE  i:1;         // Interrupt bit
	BYTE  rsv1:1;      // Reserved
 
	BYTE  status;      // Status register
	BYTE  error;       // Error register
 
	// DWORD 1
	BYTE  lba0;        // LBA low register, 7:0
	BYTE  lba1;        // LBA mid register, 15:8
	BYTE  lba2;        // LBA high register, 23:16
	BYTE  device;      // Device register
 
	// DWORD 2
	BYTE  lba3;        // LBA register, 31:24
    BYTE  lba4;        // LBA register, 39:32
	BYTE  lba5;        // LBA register, 47:40
	BYTE  rsv2;        // Reserved
 
	// DWORD 3
	BYTE  countl;      // Count register, 7:0
	BYTE  counth;      // Count register, 15:8
	BYTE  rsv3[2];     // Reserved
 
	// DWORD 4
	BYTE  rsv4[4];     // Reserved
} FIS_REG_D2H;

typedef struct tagFIS_DATA
{
	// DWORD 0
	BYTE  fis_type;	// FIS_TYPE_DATA
 
	BYTE  pmport:4;	// Port multiplier
	BYTE  rsv0:4;		// Reserved
 
	BYTE  rsv1[2];	// Reserved
 
	// DWORD 1 ~ N
	DWORD data[1];	// Payload
} FIS_DATA;

typedef struct tagFIS_PIO_SETUP
{
	// DWORD 0
	BYTE  fis_type;	// FIS_TYPE_PIO_SETUP
 
	BYTE  pmport:4;	// Port multiplier
	BYTE  rsv0:1;		// Reserved
	BYTE  d:1;		// Data transfer direction, 1 - device to host
	BYTE  i:1;		// Interrupt bit
	BYTE  rsv1:1;
 
	BYTE  status;		// Status register
	BYTE  error;		// Error register
 
	// DWORD 1
	BYTE  lba0;		// LBA low register, 7:0
	BYTE  lba1;		// LBA mid register, 15:8
	BYTE  lba2;		// LBA high register, 23:16
	BYTE  device;		// Device register
    // DWORD 2
	BYTE  lba3;		// LBA register, 31:24
	BYTE  lba4;		// LBA register, 39:32
	BYTE  lba5;		// LBA register, 47:40
	BYTE  rsv2;		// Reserved
 
	// DWORD 3
	BYTE  countl;		// Count register, 7:0
	BYTE  counth;		// Count register, 15:8
	BYTE  rsv3;		// Reserved
	BYTE  e_status;	// New value of status register
 
	// DWORD 4
	WORD tc;		// Transfer count
	BYTE  rsv4[2];	// Reserved
} FIS_PIO_SETUP;

typedef struct tagFIS_DMA_SETUP
{
	// DWORD 0
	BYTE  fis_type;	// FIS_TYPE_DMA_SETUP
 
	BYTE  pmport:4;	// Port multiplier
	BYTE  rsv0:1;		// Reserved
	BYTE  d:1;		// Data transfer direction, 1 - device to host
	BYTE  i:1;		// Interrupt bit
	BYTE  a:1;            // Auto-activate. Specifies if DMA Activate FIS is needed
 
        BYTE  rsved[2];       // Reserved
 
	//DWORD 1&2
 
        QWORD DMAbufferID;    // DMA Buffer Identifier. Used to Identify DMA buffer in host memory. SATA Spec says host specific and not in Spec. Trying AHCI spec might work.
 
        //DWORD 3
        DWORD rsvd;           //More reserved
 
        //DWORD 4
        DWORD DMAbufOffset;   //Byte offset into buffer. First 2 bits must be 0
 
        //DWORD 5
        DWORD TransferCount;  //Number of bytes to transfer. Bit 0 must be 0
 
        //DWORD 6
        DWORD resvd;          //Reserved
 
} FIS_DMA_SETUP;

#pragma pack( pop )

// Memory allocated structures for AHCI control

#define AHCI_MEMORY_LOCATION 0x24

typedef volatile struct tagHBA_PORT
{
    DWORD clb;		// 0x00, command list base address, 1K-byte aligned
	DWORD clbu;		// 0x04, command list base address upper 32 bits
	DWORD fb;		// 0x08, FIS base address, 256-byte aligned
	DWORD fbu;		// 0x0C, FIS base address upper 32 bits
	DWORD is;		// 0x10, interrupt status
	DWORD ie;		// 0x14, interrupt enable
	DWORD cmd;		// 0x18, command and status
	DWORD rsv0;		// 0x1C, Reserved
	DWORD tfd;		// 0x20, task file data
	DWORD sig;		// 0x24, signature
	DWORD ssts;		// 0x28, SATA status (SCR0:SStatus)
	DWORD sctl;		// 0x2C, SATA control (SCR2:SControl)
	DWORD serr;		// 0x30, SATA error (SCR1:SError)
	DWORD sact;		// 0x34, SATA active (SCR3:SActive)
	DWORD ci;		// 0x38, command issue
	DWORD sntf;		// 0x3C, SATA notification (SCR4:SNotification)
	DWORD fbs;		// 0x40, FIS-based switch control
	DWORD rsv1[11];	// 0x44 ~ 0x6F, Reserved
	DWORD vendor[4];	// 0x70 ~ 0x7F, vendor specific
} HBA_PORT;

typedef volatile struct tagHBA_MEM
{
	// 0x00 - 0x2B, Generic Host Control
	DWORD cap;		// 0x00, Host capability
	DWORD ghc;		// 0x04, Global host control
	DWORD is;		// 0x08, Interrupt status
	DWORD pi;		// 0x0C, Port implemented
	DWORD vs;		// 0x10, Version
	DWORD ccc_ctl;	// 0x14, Command completion coalescing control
	DWORD ccc_pts;	// 0x18, Command completion coalescing ports
	DWORD em_loc;		// 0x1C, Enclosure management location
	DWORD em_ctl;		// 0x20, Enclosure management control
	DWORD cap2;		// 0x24, Host capabilities extended
	DWORD bohc;		// 0x28, BIOS/OS handoff control and status
 
	// 0x2C - 0x9F, Reserved
	BYTE  rsv[0xA0-0x2C];
    // 0xA0 - 0xFF, Vendor specific registers
	BYTE  vendor[0x100-0xA0];
 
	// 0x100 - 0x10FF, Port control registers
	HBA_PORT	ports[32];	// 1 ~ 32 bits for device arrangement
} HBA_MEM;

typedef volatile struct tagHBA_FIS
{
	// 0x00
	FIS_DMA_SETUP	dsfis;		// DMA Setup FIS
	BYTE         pad0[4];
 
	// 0x20
	FIS_PIO_SETUP	psfis;		// PIO Setup FIS
	BYTE         pad1[12];
 
	// 0x40
	FIS_REG_D2H	rfis;		// Register – Device to Host FIS
	BYTE         pad2[4];
 
	// 0x58
	BYTE	sdbfis;		// Set Device Bit FIS
 
	// 0x60
	BYTE         ufis[64];
 
	// 0xA0
    BYTE   	rsv[0x100-0xA0];
} HBA_FIS;

typedef struct tagHBA_CMD_HEADER
{
	// DW0
	BYTE  cfl:5;		// Command FIS length in DWORDS, 2 ~ 16
	BYTE  a:1;		// ATAPI
	BYTE  w:1;		// Write, 1: H2D, 0: D2H
	BYTE  p:1;		// Prefetchable
 
	BYTE  r:1;		// Reset
	BYTE  b:1;		// BIST
	BYTE  c:1;		// Clear busy upon R_OK
	BYTE  rsv0:1;		// Reserved
	BYTE  pmp:4;		// Port multiplier port
 
	WORD prdtl;		// Physical region descriptor table length in entries
 
	// DW1
	volatile
	DWORD prdbc;		// Physical region descriptor byte count transferred
    // DW2, 3
	DWORD ctba;		// Command table descriptor base address
	DWORD ctbau;		// Command table descriptor base address upper 32 bits
 
	// DW4 - 7
	DWORD rsv1[4];	// Reserved
} HBA_CMD_HEADER;

typedef struct tagHBA_PRDT_ENTRY
{
	DWORD dba;		// Data base address
	DWORD dbau;		// Data base address upper 32 bits
	DWORD rsv0;		// Reserved
 
	// DW3
	DWORD dbc:22;		// Byte count, 4M max
	DWORD rsv1:9;		// Reserved
	DWORD i:1;		// Interrupt on completion
} HBA_PRDT_ENTRY;

typedef struct tagHBA_CMD_TBL
{
	// 0x00
	BYTE  cfis[64];	// Command FIS
 
	// 0x40
	BYTE  acmd[16];	// ATAPI command, 12 or 16 bytes
 
	// 0x50
	BYTE  rsv[48];	// Reserved
 
	// 0x80
	HBA_PRDT_ENTRY	prdt_entry[65536];	// Physical region descriptor table entries, 0 ~ 65535
} HBA_CMD_TBL;


// 하드 디스크를 관리하는 구조체
typedef struct xAHCIManagerStruct
{
    // SATA Drive 존재 여부와 쓰기를 수행할 수 있는지 여부
    BOOL bSATADetected;

    
    // 인터럽트 발생 여부와 동기화 객체
    volatile BOOL bPrimaryInterruptOccur;
    volatile BOOL bSecondaryInterruptOccur;
    MUTEX stMutex;
    
    // SATA 정보
    HBA_CMD_HEADER cmd_header;
	HBA_CMD_TBL cmd_tbl;
	HBA_FIS ahci_fis;
	HBA_MEM ahci_mem;
	HBA_PORT ahci_port;
	HBA_PRDT_ENTRY prdt_entry;
} AHCIMANAGER;

void ahci_probe_port(HBA_MEM *abar);
int ahci_check_type(HBA_PORT *port);
void ahci_port_rebase(HBA_PORT *port, int portno);
void ahci_start_cmd(HBA_PORT *port);
void ahci_stop_cmd(HBA_PORT *port);

#endif