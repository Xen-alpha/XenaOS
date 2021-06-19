/**
 *  file    SystemCallLibrary.c
 *  date    2009/12/13
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   MINT64 OS에서 지원하는 시스템 콜에 관련된 소스 파일
 */

#include "SystemCallLibrary.h"

//==============================================================================
//  콘솔 I/O 관련
//==============================================================================
/**
 *  콘솔에 문자열을 출력
 *      printf() 함수 내부에서 사용 
 *      \n, \t와 같은 문자가 포함된 문자열을 출력한 후, 화면상의 다음 출력할 위치를 
 *      반환
 */
int ConsolePrintString( const char* pcBuffer )
{
    PARAMETERTABLE stParameter;
    
    // 파라미터 삽입
    PARAM( 0 ) = ( QWORD ) pcBuffer;

    // 시스템 콜 호출
    return ExecuteSystemCall( SYSCALL_CONSOLEPRINTSTRING, &stParameter );
}

/**
 *  커서의 위치를 설정
 *      문자를 출력할 위치도 같이 설정
 */
BOOL SetCursor( int iX, int iY )
{
    PARAMETERTABLE stParameter;
    
    // 파라미터 삽입
    PARAM( 0 ) = ( QWORD ) iX;
    PARAM( 1 ) = ( QWORD ) iY;

    // 시스템 콜 호출
    return ExecuteSystemCall( SYSCALL_SETCURSOR, &stParameter );
}

/**
 *  현재 커서의 위치를 반환
 */
BOOL GetCursor( int *piX, int *piY )
{
    PARAMETERTABLE stParameter;
    
    // 파라미터 삽입
    PARAM( 0 ) = ( QWORD ) piX;
    PARAM( 1 ) = ( QWORD ) piY;

    // 시스템 콜 호출
    return ExecuteSystemCall( SYSCALL_GETCURSOR, &stParameter );
}

/**
 *  전체 화면을 삭제
 */
BOOL ClearScreen( void )
{
    // 시스템 콜 호출
    return ExecuteSystemCall( SYSCALL_CLEARSCREEN, NULL );
}

/**
 *  getch() 함수의 구현
 */
BYTE getch( void )
{
    // 시스템 콜 호출
    return ExecuteSystemCall( SYSCALL_GETCH, NULL );
}

//==============================================================================
// 동적 메모리 관련
//==============================================================================
/**
 *  메모리를 할당
 */
void* malloc( QWORD qwSize )
{
    PARAMETERTABLE stParameter;
    
    // 파라미터 삽입
    PARAM( 0 ) = qwSize;

    // 시스템 콜 호출
    return ( void* ) ExecuteSystemCall( SYSCALL_MALLOC, &stParameter );
}

/**
 *  할당 받은 메모리를 해제
 */
BOOL free( void* pvAddress )
{
    PARAMETERTABLE stParameter;
    
    // 파라미터 삽입
    PARAM( 0 ) = ( QWORD ) pvAddress;

    // 시스템 콜 호출
    return ( BOOL ) ExecuteSystemCall( SYSCALL_FREE, &stParameter );    
}

//==============================================================================
// 파일과 디렉터리 I/O 관련
//==============================================================================
/**
 *  파일을 열거나 생성 
 */
FILE* fopen( const char* pcFileName, const char* pcMode )
{
    PARAMETERTABLE stParameter;
    
    // 파라미터 삽입
    PARAM( 0 ) = ( QWORD ) pcFileName;
    PARAM( 1 ) = ( QWORD ) pcMode;

    // 시스템 콜 호출
    return ( FILE* ) ExecuteSystemCall( SYSCALL_FOPEN, &stParameter );      
}

/**
 *  파일을 읽어 버퍼로 복사
 */
DWORD fread( void* pvBuffer, DWORD dwSize, DWORD dwCount, FILE* pstFile )
{
    PARAMETERTABLE stParameter;
    
    // 파라미터 삽입
    PARAM( 0 ) = ( QWORD ) pvBuffer;
    PARAM( 1 ) = ( QWORD ) dwSize;
    PARAM( 2 ) = ( QWORD ) dwCount;
    PARAM( 3 ) = ( QWORD ) pstFile;

    // 시스템 콜 호출
    return ExecuteSystemCall( SYSCALL_FREAD, &stParameter );      
}

/**
 *  버퍼의 데이터를 파일에 씀
 */
DWORD fwrite( const void* pvBuffer, DWORD dwSize, DWORD dwCount, FILE* pstFile )
{
    PARAMETERTABLE stParameter;
    
    // 파라미터 삽입
    PARAM( 0 ) = ( QWORD ) pvBuffer;
    PARAM( 1 ) = ( QWORD ) dwSize;
    PARAM( 2 ) = ( QWORD ) dwCount;
    PARAM( 3 ) = ( QWORD ) pstFile;

    // 시스템 콜 호출
    return ExecuteSystemCall( SYSCALL_FWRITE, &stParameter );    
}

/**
 *  파일 포인터의 위치를 이동
 */
int fseek( FILE* pstFile, int iOffset, int iOrigin )
{
    PARAMETERTABLE stParameter;
    
    // 파라미터 삽입
    PARAM( 0 ) = ( QWORD ) pstFile;
    PARAM( 1 ) = ( QWORD ) iOffset;
    PARAM( 2 ) = ( QWORD ) iOrigin;

    // 시스템 콜 호출
    return ( int ) ExecuteSystemCall( SYSCALL_FSEEK, &stParameter );     
}

/**
 *  파일을 닫음
 */
int fclose( FILE* pstFile )
{
    PARAMETERTABLE stParameter;
    
    // 파라미터 삽입
    PARAM( 0 ) = ( QWORD ) pstFile;

    // 시스템 콜 호출
    return ( int ) ExecuteSystemCall( SYSCALL_FCLOSE, &stParameter );      
}

/**
 *  파일을 삭제
 */
int remove( const char* pcFileName )
{
    PARAMETERTABLE stParameter;
    
    // 파라미터 삽입
    PARAM( 0 ) = ( QWORD ) pcFileName;

    // 시스템 콜 호출
    return ( int ) ExecuteSystemCall( SYSCALL_REMOVE, &stParameter );          
}

/**
 *  디렉터리를 엶
 */
DIR* opendir( const char* pcDirectoryName )
{
    PARAMETERTABLE stParameter;
    
    // 파라미터 삽입
    PARAM( 0 ) = ( QWORD ) pcDirectoryName;

    // 시스템 콜 호출
    return ( DIR* ) ExecuteSystemCall( SYSCALL_OPENDIR, &stParameter );         
}

/**
 *  디렉터리 엔트리를 반환하고 다음으로 이동
 */
struct dirent* readdir( DIR* pstDirectory )
{
    PARAMETERTABLE stParameter;
    
    // 파라미터 삽입
    PARAM( 0 ) = ( QWORD ) pstDirectory;

    // 시스템 콜 호출
    return ( struct dirent* ) ExecuteSystemCall( SYSCALL_READDIR, 
                                                               &stParameter );       
}

/**
 *  디렉터리 포인터를 디렉터리의 처음으로 이동
 */
BOOL rewinddir( DIR* pstDirectory )
{
    PARAMETERTABLE stParameter;
    
    // 파라미터 삽입
    PARAM( 0 ) = ( QWORD ) pstDirectory;

    // 시스템 콜 호출
    return ( BOOL ) ExecuteSystemCall( SYSCALL_REWINDDIR, &stParameter );          
}

/**
 *  열린 디렉터리를 닫음
 */
int closedir( DIR* pstDirectory )
{
    PARAMETERTABLE stParameter;
    
    // 파라미터 삽입
    PARAM( 0 ) = ( QWORD ) pstDirectory;

    // 시스템 콜 호출
    return ( int ) ExecuteSystemCall( SYSCALL_CLOSEDIR, &stParameter );       
}

/**
 *  핸들 풀을 검사하여 파일이 열려있는지를 확인
 */
BOOL IsFileOpened( const struct dirent* pstEntry )
{
    PARAMETERTABLE stParameter;
    
    // 파라미터 삽입
    PARAM( 0 ) = ( QWORD ) pstEntry;

    // 시스템 콜 호출
    return ( BOOL ) ExecuteSystemCall( SYSCALL_ISFILEOPENED, &stParameter );    
}

//==============================================================================
// 하드 디스크 I/O 관련
//==============================================================================
/**
 *  하드 디스크의 섹터를 읽음
 *      최대 256개의 섹터를 읽을 수 있음
 *      실제로 읽은 섹터 수를 반환
 */
int ReadHDDSector( BOOL bPrimary, BOOL bMaster, DWORD dwLBA, int iSectorCount, 
        char* pcBuffer )
{
    PARAMETERTABLE stParameter;
    
    // 파라미터 삽입
    PARAM( 0 ) = ( QWORD ) bPrimary;
    PARAM( 1 ) = ( QWORD ) bMaster;
    PARAM( 2 ) = ( QWORD ) dwLBA;
    PARAM( 3 ) = ( QWORD ) iSectorCount;
    PARAM( 4 ) = ( QWORD ) pcBuffer;

    // 시스템 콜 호출
    return ( int ) ExecuteSystemCall( SYSCALL_READHDDSECTOR, &stParameter );     
}

/**
 *  하드 디스크에 섹터를 씀
 *      최대 256개의 섹터를 쓸 수 있음
 *      실제로 쓴 섹터 수를 반환
 */
int WriteHDDSector( BOOL bPrimary, BOOL bMaster, DWORD dwLBA, int iSectorCount, 
        char* pcBuffer )
{
    PARAMETERTABLE stParameter;
    
    // 파라미터 삽입
    PARAM( 0 ) = ( QWORD ) bPrimary;
    PARAM( 1 ) = ( QWORD ) bMaster;
    PARAM( 2 ) = ( QWORD ) dwLBA;
    PARAM( 3 ) = ( QWORD ) iSectorCount;
    PARAM( 4 ) = ( QWORD ) pcBuffer;

    // 시스템 콜 호출
    return ( int ) ExecuteSystemCall( SYSCALL_WRITEHDDSECTOR, &stParameter );      
}

//==============================================================================
// 태스크와 스케줄러 관련
//==============================================================================
/**
 *  태스크를 생성
 */
QWORD CreateTask( QWORD qwFlags, void* pvMemoryAddress, QWORD qwMemorySize, 
                  QWORD qwEntryPointAddress, BYTE bAffinity )
{
    PARAMETERTABLE stParameter;
    
    // 파라미터 삽입
    PARAM( 0 ) = ( QWORD ) qwFlags;
    PARAM( 1 ) = ( QWORD ) pvMemoryAddress;
    PARAM( 2 ) = ( QWORD ) qwMemorySize;
    PARAM( 3 ) = ( QWORD ) qwEntryPointAddress;
    PARAM( 4 ) = ( QWORD ) bAffinity;

    // 시스템 콜 호출
    return ExecuteSystemCall( SYSCALL_CREATETASK, &stParameter );     
}

/**
 *  다른 태스크를 찾아서 전환
 */
BOOL Schedule( void )
{
    // 시스템 콜 호출
    return ( BOOL) ExecuteSystemCall( SYSCALL_SCHEDULE, NULL );
}

/**
 *  태스크의 우선 순위를 변경
 */
BOOL ChangePriority( QWORD qwID, BYTE bPriority, BOOL bExecutedInInterrupt )
{
    PARAMETERTABLE stParameter;
    
    // 파라미터 삽입
    PARAM( 0 ) = qwID;
    PARAM( 1 ) = ( QWORD ) bPriority;
    PARAM( 2 ) = ( QWORD ) bExecutedInInterrupt;

    // 시스템 콜 호출
    return ( BOOL ) ExecuteSystemCall( SYSCALL_CHANGEPRIORITY, &stParameter );        
}

/**
 *  태스크를 종료
 */
BOOL EndTask( QWORD qwTaskID )
{
    PARAMETERTABLE stParameter;
    
    // 파라미터 삽입
    PARAM( 0 ) = qwTaskID;

    // 시스템 콜 호출
    return ( BOOL ) ExecuteSystemCall( SYSCALL_ENDTASK, &stParameter );     
}

/**
 *  태스크가 자신을 종료함
 */
void exit( int iExitValue )
{
    PARAMETERTABLE stParameter;
    
    // 파라미터 삽입
    PARAM( 0 ) = ( QWORD ) iExitValue;

    // 시스템 콜 호출
    ExecuteSystemCall( SYSCALL_EXITTASK, &stParameter );      
}

/**
 *  전체 태스크의 수를 반환
 */ 
int GetTaskCount( BYTE bAPICID )
{
    PARAMETERTABLE stParameter;
    
    // 파라미터 삽입
    PARAM( 0 ) = ( QWORD ) bAPICID;

    // 시스템 콜 호출
    return ( int ) ExecuteSystemCall( SYSCALL_GETTASKCOUNT, &stParameter );     
}

/**
 *  태스크가 존재하는지 여부를 반환
 */
BOOL IsTaskExist( QWORD qwID )
{
    PARAMETERTABLE stParameter;
    
    // 파라미터 삽입
    PARAM( 0 ) = qwID;

    // 시스템 콜 호출
    return ( BOOL ) ExecuteSystemCall( SYSCALL_ISTASKEXIST, &stParameter );       
}

/**
 *  프로세서의 사용률을 반환
 */
QWORD GetProcessorLoad( BYTE bAPICID )
{
    PARAMETERTABLE stParameter;
     
    // 파라미터 삽입
    PARAM( 0 ) = ( QWORD ) bAPICID;

    // 시스템 콜 호출
    return ExecuteSystemCall( SYSCALL_GETPROCESSORLOAD, &stParameter );   
}

/**
 *  프로세서 친화도를 변경
 */
BOOL ChangeProcessorAffinity( QWORD qwTaskID, BYTE bAffinity )
{
    PARAMETERTABLE stParameter;
     
    // 파라미터 삽입
    PARAM( 0 ) = qwTaskID;
    PARAM( 1 ) = ( QWORD ) bAffinity;

    // 시스템 콜 호출
    return ( BOOL ) ExecuteSystemCall( SYSCALL_CHANGEPROCESSORAFFINITY, &stParameter );      
}

//==============================================================================
// RTC 관련
//==============================================================================
/**
 *  CMOS 메모리에서 RTC 컨트롤러가 저장한 현재 시간을 읽음
 */
BOOL ReadRTCTime( BYTE* pbHour, BYTE* pbMinute, BYTE* pbSecond )
{
    PARAMETERTABLE stParameter;
    
    // 파라미터 삽입
    PARAM( 0 ) = ( QWORD ) pbHour;
    PARAM( 1 ) = ( QWORD ) pbMinute;
    PARAM( 2 ) = ( QWORD ) pbSecond;
    
    // 시스템 콜 호출
    return ( BOOL ) ExecuteSystemCall( SYSCALL_READRTCTIME, &stParameter );        
}

/**
 *  CMOS 메모리에서 RTC 컨트롤러가 저장한 현재 일자를 읽음
 */
BOOL ReadRTCDate( WORD* pwYear, BYTE* pbMonth, BYTE* pbDayOfMonth, 
                  BYTE* pbDayOfWeek )
{
    PARAMETERTABLE stParameter;
    
    // 파라미터 삽입
    PARAM( 0 ) = ( QWORD ) pwYear;
    PARAM( 1 ) = ( QWORD ) pbMonth;
    PARAM( 2 ) = ( QWORD ) pbDayOfMonth;
    PARAM( 3 ) = ( QWORD ) pbDayOfWeek;
    
    // 시스템 콜 호출
    return ( BOOL ) ExecuteSystemCall( SYSCALL_READRTCDATE, &stParameter );      
}

//==============================================================================
// 시리얼 통신 관련
//==============================================================================
/**
 *  시리얼 포트로 데이터를 송신
 */
void SendSerialData( BYTE* pbBuffer, int iSize )
{
    PARAMETERTABLE stParameter;
    
    // 파라미터 삽입
    PARAM( 0 ) = ( QWORD ) pbBuffer;
    PARAM( 1 ) = ( QWORD ) iSize;
    
    // 시스템 콜 호출
    ExecuteSystemCall( SYSCALL_SENDSERIALDATA, &stParameter );
}

/**
 *  시리얼 포트에서 데이터를 읽음
 */
int ReceiveSerialData( BYTE* pbBuffer, int iSize )
{
    PARAMETERTABLE stParameter;
    
    // 파라미터 삽입
    PARAM( 0 ) = ( QWORD ) pbBuffer;
    PARAM( 1 ) = ( QWORD ) iSize;
    
    // 시스템 콜 호출
    return ( int ) ExecuteSystemCall( SYSCALL_RECEIVESERIALDATA, &stParameter );
}

/**
 *  시리얼 포트 컨트롤러의 FIFO를 초기화
 */
void ClearSerialFIFO( void )
{
    // 시스템 콜 호출
    ExecuteSystemCall( SYSCALL_CLEARSERIALFIFO, NULL );
}

//==============================================================================
// 기타
//==============================================================================
/**
 *  RAM 크기를 반환
 */
QWORD GetTotalRAMSize( void )
{
    // 시스템 콜 호출
    return ExecuteSystemCall( SYSCALL_GETTOTALRAMSIZE, NULL );
}

/**
 *  Tick Count를 반환
 */
QWORD GetTickCount( void )
{
    // 시스템 콜 호출
    return ExecuteSystemCall( SYSCALL_GETTICKCOUNT, NULL );
}

/**
 *  밀리세컨드(milisecond) 동안 대기
 */
void Sleep( QWORD qwMillisecond )
{
    PARAMETERTABLE stParameter;
    
    // 파라미터 삽입
    PARAM( 0 ) = qwMillisecond;
    
    // 시스템 콜 호출
    ExecuteSystemCall( SYSCALL_SLEEP, &stParameter );    
}


