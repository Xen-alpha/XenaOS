/**
 *  file    SystemCall.c
 *  date    2009/12/08
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   시스템 콜에 관련된 함수를 정의한 소스 파일
 */

#include "Types.h"
#include "SystemCall.h"
#include "AssemblyUtility.h"
#include "Descriptor.h"
#include "SystemCallList.h"
#include "HardDisk.h"
#include "DynamicMemory.h"
#include "RTC.h"
#include "Task.h"
#include "Utility.h"
#include "SerialPort.h"

/**
 *  시스템 콜(syscall/sysret)를 호출할 수 있도록 초기화
 */
void kInitializeSystemCall( void )
{
    QWORD qwRDX;
    QWORD qwRAX;
    
    // IA32_STAR MSR(0xC0000081) 설정
    // 상위 32비트는 SYSRET CS/SS[63:48]와 SYSCALL CS/SS[47:32]로 구성
    qwRDX = ( ( GDT_KERNELDATASEGMENT | SELECTOR_RPL_3 ) << 16 ) | GDT_KERNELCODESEGMENT;
    // 하위 32비트는 예약됨(Reserved)
    qwRAX = 0;
    kWriteMSR( 0xC0000081, qwRDX, qwRAX );
    
    // IA32_LSTAR MSR 설정(0xC0000082), SystemCallEntryPoint() 함수의 어드레스로 지정
    kWriteMSR( 0xC0000082, 0, ( QWORD ) kSystemCallEntryPoint );
    
    // IA32_FMASK MSR 설정(0xC0000084), 
    // RFLAGS 레지스터의 값을 변경할 것은 없으므로 0x00으로 설정하여 값을 유지
    kWriteMSR( 0xC0000084, 0, 0x00 );
}

/**
 *  시스템 콜 번호와 파라미터 자료구조를 이용해서 시스템 콜을 처리
 */ 
QWORD kProcessSystemCall( QWORD qwServiceNumber, PARAMETERTABLE* pstParameter )
{
    TCB* pstTCB;
    
    switch( qwServiceNumber )
    {
        //----------------------------------------------------------------------
        // 콘솔 I/O 관련
        //----------------------------------------------------------------------
    case SYSCALL_CONSOLEPRINTSTRING:
        return kConsolePrintString( (char*)PARAM( 0 ) );        
    case SYSCALL_SETCURSOR:
        kSetCursor( PARAM( 0 ), PARAM( 1 ) );
        return TRUE;
    case SYSCALL_GETCURSOR:
        kGetCursor( ( int* ) PARAM( 0 ), ( int* ) PARAM( 1 ) );
        return TRUE;
    case SYSCALL_CLEARSCREEN:
        kClearScreen();
        return TRUE;
    case SYSCALL_GETCH:
        return kGetCh();

        // 동적 메모리 할당/해제 관련
    case SYSCALL_MALLOC:
        return ( QWORD ) kAllocateMemory( PARAM( 0 ) );
    case SYSCALL_FREE:
        return kFreeMemory( ( void* ) PARAM( 0 ) );

    //----------------------------------------------------------------------
    // 파일과 디렉터리 I/O 관련
    //----------------------------------------------------------------------
    /*
    case SYSCALL_FOPEN:
        return ( QWORD ) fopen( ( char* ) PARAM( 0 ), ( char* ) PARAM( 1 ) );
    case SYSCALL_FREAD:
        return fread( ( void* ) PARAM( 0 ), PARAM( 1 ), PARAM( 2 ), 
                      ( FILE* ) PARAM( 3 ) );
    case SYSCALL_FWRITE:
        return fwrite( ( void* ) PARAM( 0 ), PARAM( 1 ), PARAM( 2 ), 
                       ( FILE* ) PARAM( 3 ) );
    case SYSCALL_FSEEK:
        return fseek( ( FILE* ) PARAM( 0 ), PARAM( 1 ), PARAM( 2 ) );
    case SYSCALL_FCLOSE:
        return fclose( ( FILE* ) PARAM( 0 ) );
    case SYSCALL_REMOVE:
        return remove( ( char* ) PARAM( 0 ) );
    case SYSCALL_OPENDIR:
        return ( QWORD ) opendir( ( char* ) PARAM( 0 ) );
    case SYSCALL_READDIR:
        return ( QWORD ) readdir( ( DIR* ) PARAM( 0 ) );
    case SYSCALL_REWINDDIR:
        rewinddir( ( DIR* ) PARAM( 0 ) );
        return TRUE;
    case SYSCALL_CLOSEDIR:
        return closedir( ( DIR* ) PARAM( 0 ) );
    case SYSCALL_ISFILEOPENED:
        return kIsFileOpened( ( DIRECTORYENTRY* ) PARAM( 0 ) );
    */

        //----------------------------------------------------------------------
        // 하드 디스크 I/O 관련
        //----------------------------------------------------------------------
    case SYSCALL_READHDDSECTOR:
        return kReadHDDSector( PARAM( 0 ), PARAM( 1), PARAM( 2 ), 
                               ( char* ) PARAM( 3 ) );
    case SYSCALL_WRITEHDDSECTOR:
        return kWriteHDDSector( PARAM( 0 ), PARAM( 1 ), PARAM( 2 ), 
                               ( char* ) PARAM( 3 ) );

        //----------------------------------------------------------------------
        // 태스크와 스케줄러 관련
        //----------------------------------------------------------------------
    case SYSCALL_CREATETASK:
        pstTCB = kCreateTask( PARAM( 0 ), ( void* ) PARAM( 1 ), PARAM( 2 ), 
                              PARAM( 3 ), PARAM( 4 ) );
        if( pstTCB == NULL )
        {
            return TASK_INVALIDID;
        }   
        return pstTCB->stLink.qwID;
    case SYSCALL_SCHEDULE:
        return kSchedule();        
    case SYSCALL_CHANGEPRIORITY:
        return kChangePriority( PARAM( 0 ), PARAM( 1 ) );
    case SYSCALL_ENDTASK:
        return kEndTask( PARAM( 0 ) );
    case SYSCALL_EXITTASK:
        kExitTask();
        return TRUE;
    case SYSCALL_GETTASKCOUNT:
        return kGetTaskCount( PARAM( 0 ) );
    case SYSCALL_ISTASKEXIST:
        return kIsTaskExist( PARAM( 0 ) );
    case SYSCALL_GETPROCESSORLOAD:
        return kGetProcessorLoad( PARAM( 0 ) );
    case SYSCALL_CHANGEPROCESSORAFFINITY:
        return kChangePriority( PARAM( 0 ), PARAM( 1 ) );

        //----------------------------------------------------------------------
        // RTC 관련
        //----------------------------------------------------------------------
    case SYSCALL_READRTCTIME:
        kReadRTCTime( ( BYTE* ) PARAM( 0 ), ( BYTE* ) PARAM( 1 ), 
                             ( BYTE* ) PARAM( 2 ) );
        return TRUE;
    case SYSCALL_READRTCDATE:
        kReadRTCDate( ( WORD* ) PARAM( 0 ), ( BYTE* ) PARAM( 1 ), 
                             ( BYTE* ) PARAM( 2 ), ( BYTE* ) PARAM( 3 ) );
        return TRUE;


    //----------------------------------------------------------------------
        // 시리얼 I/O 관련
        //----------------------------------------------------------------------
    case SYSCALL_SENDSERIALDATA:
        kSendSerialData( ( BYTE* ) PARAM( 0 ), PARAM( 1 ) );
        return TRUE;
    case SYSCALL_RECEIVESERIALDATA:
        return kReceiveSerialData( ( BYTE* ) PARAM( 0 ), PARAM( 1 ) );  
    case SYSCALL_CLEARSERIALFIFO:
        kClearSerialFIFO();
        return TRUE;
        //----------------------------------------------------------------------
        // 기타
        //----------------------------------------------------------------------
    case SYSCALL_GETTOTALRAMSIZE:
        return kGetTotalRAMSize();
    case SYSCALL_GETTICKCOUNT:
        return kGetTickCount();
    case SYSCALL_SLEEP:
        kSleep( PARAM( 0 ) );
        return TRUE;
        //----------------------------------------------------------------------
        // test
        //----------------------------------------------------------------------
    case SYSCALL_TEST:
        kPrintf( "Test System Call... System Call Test Success~!!\n" );
        return TRUE;
        
        //----------------------------------------------------------------------
        // 정의되지 않은 시스템 콜 처리
        //----------------------------------------------------------------------
    default:
        kPrintf( "Undefined System Call~!!!, Service Number: %d\n", 
                 qwServiceNumber );
        return FALSE;
    }
}
