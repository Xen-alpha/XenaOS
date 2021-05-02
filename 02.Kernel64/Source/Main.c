/**
 *  file    Main.c
 *  author  xenα
 *  brief   C 언어로 작성된 64비트 커널의 엔트리 포인트 파일
 */

#include "Utility.h"
#include "Types.h"
#include "Keyboard.h"
#include "Descriptor.h"
#include "PIC.h"
#include "Console.h"
#include "ConsoleShell.h"
#include "Task.h"
#include "PIT.h"
#include "DynamicMemory.h"
#include "HardDisk.h"
#include "FileSystem.h"
#include "SerialPort.h"
#include "MultiProcessor.h"

// 함수 선언
void MainForAP(void);

/**
 *  아래 함수는 C 언어 커널의 시작 부분임
 */
void Main( void )
{
    int iCursorX, iCursorY;

    if (*((BYTE *)BOOTSTRAPPROCESSOR_FLAGADDRESS) == 0) {
        MainForAP();
    }

    // Bootstrap Processor는 이제 AP에 대한 사다리 걷어차기를 해야 커널이 손상되는 것을 방지한다.
    *((BYTE *)BOOTSTRAPPROCESSOR_FLAGADDRESS) = 0;

    kInitializeConsole(0,24);
    kPrintf ("Switch To IA-32e Mode Success~!!\n" );
    kPrintf( "IA-32e C Language Kernel Start..............[Pass]\n" );
    kPrintf( "Initialize Console..........................[Pass]\n" );
    
    // 부팅 상황을 화면에 출력
    kGetCursor( &iCursorX, &iCursorY);
    kInitializeGDTTableAndTSS();
    kLoadGDTR( GDTR_STARTADDRESS );
    kPrintf( "GDT Initialize And Switch For IA-32e Mode...[%s]\n" ,"Pass");
    
    kLoadTR( GDT_TSSSEGMENT );
    kPrintf( "TSS Segment Load............................[%s]\n", "Pass" );
    
    kInitializeIDTTables();    
    kLoadIDTR( IDTR_STARTADDRESS );
    kPrintf( "IDT Initialize..............................[%s]\n", "Pass" );
    
    kCheckTotalRAMSize();
    kPrintf( "Total RAM Size Check........................[%s], Size = %d MB\n", "Pass", kGetTotalRAMSize() );
    
    iCursorY++;
    kInitializeScheduler();
    kPrintf( "TCB Pool And Scheduler Initialize...........[Pass]\n" );

    // 동적 메모리 초기화
    iCursorY++;
    kInitializeDynamicMemory();
    kPrintf( "Dynamic Memory Initialize...................[Pass]\n" );

    // 1ms당 한번씩 인터럽트가 발생하도록 설정
    kInitializePIT( MSTOCOUNT( 1 ), 1 );

    // 키보드를 활성화
    if( kInitializeKeyboard() == TRUE )
    {
        kPrintf( "Keyboard Activate And Queue Initialize......[%s]\n", "Pass" );
        kChangeKeyboardLED( FALSE, FALSE, FALSE );
    }
    else
    {
       kPrintf( "Keyboard Activate And Queue Initialize......[%s]\n", "Fail" );
        while( 1 ) ;
    }
    
    
    // PIC 컨트롤러 초기화 및 모든 인터럽트 활성화
    kInitializePIC();
    kMaskPICInterrupt( 0 );
    kEnableInterrupt();
    kPrintf( "PIC Controller And Interrupt Initialize.....[Pass]\n" );

     // HDD 및 파일 시스템을 초기화
    if( kInitializeFileSystem() == TRUE )
    {
        kPrintf( "File System Initialize......................[Pass]\n" );
    }
    else
    {
        kPrintf( "File System Initialize......................[Fail]\n" );
    }

    kInitializeSerialPort();
    kPrintf( "Serial Port Initialize......................[Pass]\n" );
    iCursorY++;

    // 유휴 태스크를 생성하고 셸을 시작
    kCreateTask( TASK_FLAGS_LOWEST | TASK_FLAGS_THREAD | TASK_FLAGS_SYSTEM | TASK_FLAGS_IDLE, 0, 0, ( QWORD ) kIdleTask );
    kStartConsoleShell();
}

void MainForAP(void){
    QWORD qwTickCount;

    // GDT 테이블을 설정
    kLoadGDTR( GDTR_STARTADDRESS );

    // TSS 디스크립터를 설정. TSS 세그먼트와 디스크립터를 Application Processor의 
    // 수만큼 생성했으므로, APIC ID를 이용하여 TSS 디스크립터를 할당
    kLoadTR( GDT_TSSSEGMENT + ( kGetAPICID() * sizeof( GDTENTRY16 ) ) );

    // IDT 테이블을 설정
    kLoadIDTR( IDTR_STARTADDRESS );

    // 1초마다 한번씩 메시지를 출력
    qwTickCount = kGetTickCount();
    while( 1 )
    {
        if( kGetTickCount() - qwTickCount > 1000 )
        {
            qwTickCount = kGetTickCount();
            
            kPrintf( "Application Processor[APIC ID: %d] Is Activated\n",
                    kGetAPICID() );
        }
    }
}
