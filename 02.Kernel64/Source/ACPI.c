/**
 *  file    MPConfigurationTable.c
 *  date    2009/06/20
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   MP 설정 테이블(MP Configuration Table)에 관련된 소스 파일
 */

#include "ACPI.h"
#include "Utility.h"
#include "Console.h"
#include "PCI.h"

static ACPIManager gs_stACPIConfigurationManager = {0, };

void xInitACPIMode(void){
    xFindACPIFloatingPointerAddress();
    xParsePCIExpressInformation();
    gs_stACPIConfigurationManager.fadt = (FADT *)xFindDescriptorTable("FACP");
    if (gs_stACPIConfigurationManager.fadt->SMI_CommandPort == 0
    && gs_stACPIConfigurationManager.fadt->AcpiEnable == 0
    && gs_stACPIConfigurationManager.fadt->AcpiDisable == 0
    && kInPortWord( gs_stACPIConfigurationManager.fadt->PM1aControlBlock) & 1 == 1 ) {
        kPrintf("FADT is already set.");
        return;
    } else {
        kOutPortByte(gs_stACPIConfigurationManager.fadt->SMI_CommandPort, gs_stACPIConfigurationManager.fadt->AcpiEnable);
        while (kInPortDword(gs_stACPIConfigurationManager.fadt->PM1aControlBlock) & 1 == 0);
    }
    return;
}

BOOL xFindACPIFloatingPointerAddress( void )
{
    QWORD* pcACPIFloatingPointer;
    QWORD qwEBDAAddress;
    QWORD qwSystemBaseMemory;

    qwEBDAAddress = *( WORD* ) ( 0x040E );
    // 세그먼트의 시작 어드레스이므로 16을 곱하여 실제 물리 어드레스로 변환
    qwEBDAAddress *= 16;
    
    for( pcACPIFloatingPointer = ( QWORD * ) qwEBDAAddress ; 
         ( QWORD ) pcACPIFloatingPointer <= ( qwEBDAAddress + 1024 ) ; 
         pcACPIFloatingPointer++ )
    {
        if( memcmp( pcACPIFloatingPointer, "RSD PTR ", 8 ) == 0 )
        {
            gs_stACPIConfigurationManager.rsdp = (RSDP2 *)pcACPIFloatingPointer;
            gs_stACPIConfigurationManager.rsdt = (XSDT *)gs_stACPIConfigurationManager.rsdp->XSDTAddress;
            return TRUE;
        }
    }

    // BIOS의 ROM 영역을 검색하여 ACPI 플로팅 포인터를 찾음
    for( pcACPIFloatingPointer = ( QWORD* ) 0x0E0000; 
         ( QWORD) pcACPIFloatingPointer < 0x0FFFFF; pcACPIFloatingPointer++ )
    {
        if( memcmp( pcACPIFloatingPointer, "RSD PTR ", 8 ) == 0 )
        {
            gs_stACPIConfigurationManager.rsdp = (RSDP2 *)pcACPIFloatingPointer;
            gs_stACPIConfigurationManager.rsdt = (XSDT *)gs_stACPIConfigurationManager.rsdp->XSDTAddress;
            
            return TRUE;
        }
    }
    return FALSE;
}

void *xFindDescriptorTable( const char * sign){ // sign must be 4 BYTE Characters
    DWORD entries = (gs_stACPIConfigurationManager.rsdt->header.Length -sizeof(gs_stACPIConfigurationManager.rsdt->header))/8;
    unsigned char target[5];
    for (DWORD i = 0 ; i < entries;i++){
        RSDTHeader* header = (RSDTHeader *) (&(gs_stACPIConfigurationManager.rsdt->PointerToOtherSDT))[i];
        if (memcmp(header->Signature, sign, 4) == 0){
            return (void *) header;
        }
    }
    kPrintf("No such signature found\n");
    return NULL;
}

void xParsePCIExpressInformation(void) {
    gs_stACPIConfigurationManager.pcieConfigTable= (PCIEConfigTable *) xFindDescriptorTable( "MCFG");
}

void xShowPCIExpressInformation( const char* pcParameterBuffer ) {
    kPrintf("Searching PCIe info...\n");
    if (gs_stACPIConfigurationManager.pcieConfigTable == NULL) {
        kPrintf("error: pcieConfigTable is empty\n");
        return;
    }
    for (DWORD i = 0 ; i < (gs_stACPIConfigurationManager.pcieConfigTable->h.Length - 44) / 16; i++){
        QWORD *baseAddress = ((&gs_stACPIConfigurationManager.pcieConfigTable->AddressSpace)[i]);
        kPrintf("base address: 0x%x\n", *baseAddress);
        WORD *segmentGroup = ((&gs_stACPIConfigurationManager.pcieConfigTable->AddressSpace)[i]+8);
        kPrintf("PCIe segment group: 0x%x\n", *segmentGroup);
        BYTE *start = ((&gs_stACPIConfigurationManager.pcieConfigTable->AddressSpace)[i]+10);
        BYTE *end = ((&gs_stACPIConfigurationManager.pcieConfigTable->AddressSpace)[i]+11);
        kPrintf("bus startpoint: 0x%x, bus endpoint: 0x%x\n", *start, *end);
        
        for (int bus = start; bus < end; bus++){
            for ( int device = 0; device < 32; device ++ ) {
                for (int func = 0 ; func < 8 ; func++) {
                    QWORD *Physical_Address = (QWORD *)baseAddress + (((bus) << 20) | (device << 15)  | (func << 12) );
                    if ( (WORD) *Physical_Address == 0xffff) {
                        continue; // invalid vendor ID
                    }

                    kPrintf("Header Type 0x%x, Device ID = 0x%x\n", (BYTE) *(Physical_Address+0x0E) ,(WORD) *(Physical_Address+0x02));
                    kPrintf("Class = 0x%x, Subclass = 0x%x, ProgIF = 0x%x\n", (BYTE)*(Physical_Address+0x0b), (BYTE)*(Physical_Address+0x0a), (BYTE)*(Physical_Address+0x09));
                    // 4개를 출력하고 나면 키 입력을 대기
                    if( ( func != 0 ) && ( ( ( func + 1 ) % 4 ) == 0 ) )
                    {
                        kPrintf( "Press any key to continue... ('q' is exit) : " );
                        if( kGetCh() == 'q' )
                        {
                            kPrintf( "\n" );
                            return ;
                        }
                        kPrintf( "\n" );            
                    }
                }
            }
            
        }
    }
    return;
}