/**
 *  file    Main.c
 *  author  xen��
 *  brief   C ���� �ۼ��� 64��Ʈ Ŀ���� ��Ʈ�� ����Ʈ ����
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

// �Լ� ����


/**
 *  �Ʒ� �Լ��� C ��� Ŀ���� ���� �κ���
 */
void Main( void )
{
    int iCursorX, iCursorY;

    kInitializeConsole(0,24);
    kPrintf ("Switch To IA-32e Mode Success~!!\n" );
    kPrintf( "IA-32e C Language Kernel Start..............[Pass]\n" );
    kPrintf( "Initialize Console..........................[Pass]\n" );
    
    // ���� ��Ȳ�� ȭ�鿡 ���
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

    // ���� �޸� �ʱ�ȭ
    iCursorY++;
    kInitializeDynamicMemory();
    kPrintf( "Dynamic Memory Initialize...................[Pass]\n" );

    // 1ms�� �ѹ��� ���ͷ�Ʈ�� �߻��ϵ��� ����
    kInitializePIT( MSTOCOUNT( 1 ), 1 );

    // Ű���带 Ȱ��ȭ
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
    
    
    // PIC ��Ʈ�ѷ� �ʱ�ȭ �� ��� ���ͷ�Ʈ Ȱ��ȭ
    kInitializePIC();
    kMaskPICInterrupt( 0 );
    kEnableInterrupt();
    kPrintf( "PIC Controller And Interrupt Initialize.....[Pass]\n" );

    // �ϵ� ��ũ�� �ʱ�ȭ
    kPrintf( "HDD Initialize..............................[    ]" );
    if( kInitializeHDD() == TRUE )
    {
        kSetCursor( 45, iCursorY++ );
        kPrintf( "Pass\n" );
    }
    else
    {
        kSetCursor( 45, iCursorY++ );
        kPrintf( "Fail\n" );
    }

     // ���� �ý����� �ʱ�ȭ
    kPrintf( "File System Initialize......................[    ]" );
    if( kInitializeFileSystem() == TRUE )
    {
        kSetCursor( 45, iCursorY++ );
        kPrintf( "Pass\n" );
    }
    else
    {
        kSetCursor( 45, iCursorY++ );
        kPrintf( "Fail\n" );
    }

    // ���� �½�ũ�� �����ϰ� ���� ����
    kCreateTask( TASK_FLAGS_LOWEST | TASK_FLAGS_THREAD | TASK_FLAGS_SYSTEM | TASK_FLAGS_IDLE, 0, 0, ( QWORD ) kIdleTask );
    kStartConsoleShell();
}
