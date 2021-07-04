/**
 *  file    HardDisk.c
 *  date    2009/04/20
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   하드 디스크 컨트롤러에 관련된 소스 파일
 */

#include "HardDisk.h"
#include "DynamicMemory.h"

// 하드 디스크를 관리하는 자료구조
HDDMANAGER gs_stHDDManager;

/**
 *  하드 디스크 디바이스 드라이버를 초기화
 */
BOOL kInitializeHDD(void)
{
    
    BOOL init1 = xInitializeAHCI( );
    
    if (init1 == FALSE) {
        kPrintf("Something go wrong with ahci\n");
        return FALSE;
    }
    identify_Storage();
    for (int k = 0 ; k < 4 ; k++){
        
        // 뮤텍스 초기화
        kInitializeMutex( &( gs_stHDDManager.stMutex[k] ) );
        
        // 하드 디스크 정보 요청
        if( kReadHDDInformation( k ) == FALSE )
        {
            kPrintf("Cannot read port %d HDD information\n", k);
            gs_stHDDManager.bHDDDetected[k] = FALSE;
            gs_stHDDManager.bCanWrite[k] = FALSE;
            continue;
        }
        kPrintf("port %d has HDD information\n", k);
        // 하드 디스크가 검색되었으면 VirtualBox에서만 쓸 수 있도록 설정
        if( kMemCmp( gs_stHDDManager.stHDDInformation[k]->vwModelNumber, "VBOX", 4 ) == 0 )
        {
            gs_stHDDManager.bCanWrite[k] = TRUE;
        }
        else
        {
            gs_stHDDManager.bCanWrite[k] = FALSE;
        }
    }
    return TRUE;
}

/**
 *  하드 디스크의 상태를 반환
 */
static BYTE kReadHDDStatus( int portnum )
{
    // 해당 SATA 포트의 상태 레지스터에서 값을 반환
    return -1;
}

/**
 *  하드 디스크의 Busy가 해제될 때까지 일정 시간 동안 대기
 */
static BOOL kWaitForHDDNoBusy( int portnum )
{
    QWORD qwStartTickCount;
    BYTE bStatus;

    // 대기를 시작한 시간을 저장
    qwStartTickCount = kGetTickCount();

    // 일정 시간 동안 하드 디스크의 Busy가 해제될 때까지 대기
    while( ( kGetTickCount() - qwStartTickCount ) <= HDD_WAITTIME )
    {
        // HDD의 상태를 반환
        bStatus = kReadHDDStatus( portnum );

        // Busy 비트(비트 7), Data request(비트 3)이 설정되어 있지 않으면 Busy가 해제된 것이므로 종료
        if( ( bStatus & 0x80) == 0 && (bStatus & 0x08) == 0)
        {
            return TRUE;
        }
        kSleep(1);
    }
    return FALSE;
}

/**
 *  하드 디스크가 Ready될 때까지 일정 시간 동안 대기
 */
static BOOL kWaitForHDDReady(int portnum )
{
    QWORD qwStartTickCount;
    BYTE bStatus;

    // 대기를 시작한 시간을 저장
    qwStartTickCount = kGetTickCount();

    // 일정 시간 동안 하드 디스크가 Ready가 될 때까지 대기
    /*
    while( ( kGetTickCount() - qwStartTickCount ) <= HDD_WAITTIME )
    {
        if ((gs_stHDDManager.stSATAHDDInformation.ahciBaseAddress->ports[portnum].sact & 0x01) == 0) {
            return TRUE;
        }
    }
    */
    return FALSE;
}


/**
 *  인터럽트 발생 여부를 설정
 */
void kSetHDDInterruptFlag( int portnum , BOOL bFlag )
{
    gs_stHDDManager.bInterruptOccur[portnum] = bFlag;
}

/**
 *  인터럽트가 발생할 때까지 대기
 */
static BOOL kWaitForHDDInterrupt( int portnum  )
{
    QWORD qwTickCount;
    // 대기를 시작한 시간을 저장
    qwTickCount = kGetTickCount();
    
    // 일정 시간 동안 하드 디스크의 인터럽트가 발생할 때까지 대기
    while( kGetTickCount() - qwTickCount <= HDD_WAITTIME )
    {
        //하드 디스크 자료구조에 인터럽트 발생 플래그를 확인
        if (gs_stHDDManager.bInterruptOccur[portnum] == TRUE){
            return TRUE;
        }
    }
    return FALSE;
}

/**
 *  하드 디스크의 정보를 읽음
 */
BOOL kReadHDDInformation( int portnum )
{
    return FALSE;
    if ( portnum >= 4) {
        kPrintf("cannot access port %d\n", portnum);
        return FALSE;
    }

    //int slot = find_cmdslot(&gs_stHDDManager.stSATAHDDInformation.ahciBaseAddress->ports[portnum]);

    // 동기화 처리
    kLockMutex( &(gs_stHDDManager.stMutex[portnum] ) );
    
    
    
    // 동기화 처리
    kUnlockMutex( &(gs_stHDDManager.stMutex[portnum] ) );
    return TRUE;
    
}

/**
 *  WORD 내의 바이트 순서를 바꿈
 */
static void kSwapByteInWord(WORD* pwData, int iWordCount)
{
    int i;
    WORD wTemp;

    for (i = 0; i < iWordCount; i++)
    {
        wTemp = pwData[ i ];
        pwData[ i ] = (wTemp >> 8 ) | (wTemp << 8 );
    }
}

/**
 *  하드 디스크의 섹터를 읽음
 *      최대 256개의 섹터를 읽을 수 있음
 *      실제로 읽은 섹터 수를 반환
 */
int kReadHDDSector( int portnum , DWORD dwLBA, int iSectorCount,
        char* pcBuffer )
{
    /*
    WORD wPortBase;
    int i, j;
    BYTE bDriveFlag;
    BYTE bStatus;
    long lReadCount = 0;
    BOOL bWaitResult;

    // 범위 검사. 최대 256 섹터를 처리할 수 있음
    if( ( gs_stHDDManager.bHDDDetected == FALSE ) ||
        ( iSectorCount <= 0 ) || ( 256 < iSectorCount ) ||
        ( ( dwLBA + iSectorCount ) >= gs_stHDDManager.stHDDInformation.dwTotalSectors ) )
    {
        return 0;
    }

    // PATA 포트에 따라서 I/O 포트의 기본 어드레스를 설정
    if( bPrimary == TRUE )
    {
        // 첫 번째 PATA 포트이면 포트 0x1F0을 저장
        wPortBase = HDD_PORT_PRIMARYBASE;
    }
    else
    {
        // 두 번째 PATA 포트이면 포트 0x170을 저장
        wPortBase = HDD_PORT_SECONDARYBASE;
    }

    // 동기화 처리
    kLockMutex( &( gs_stHDDManager.stMutex ) );
    
    // 아직 수행 중인 커맨드가 있다면 일정 시간 동안 끝날 때까지 대기
    if( kWaitForHDDNoBusy( bPrimary ) == FALSE )
    {
        // 동기화 처리
        kUnlockMutex( &( gs_stHDDManager.stMutex ) );
        return FALSE;
    }

    //==========================================================================
    //  데이터 레지스터 설정
    //      LBA 모드의 경우, 섹터 번호 -> 실린더 번호 -> 헤드 번호의 순으로 
    //      LBA 어드레스를 대입
    //==========================================================================
    // 섹터 수 레지스터(포트 0x1F2 또는 0x172)에 읽을 섹터 수를 전송
    kOutPortByte( wPortBase + HDD_PORT_INDEX_SECTORCOUNT, iSectorCount );
    // 섹터 번호 레지스터(포트 0x1F3 또는 0x173)에 읽을 섹터 위치(LBA 0~7비트)를 전송
    kOutPortByte( wPortBase + HDD_PORT_INDEX_SECTORNUMBER, dwLBA );
    // 실린더 LSB 레지스터(포트 0x1F4 또는 0x174)에 읽을 섹터 위치(LBA 8~15비트)를 전송
    kOutPortByte( wPortBase + HDD_PORT_INDEX_CYLINDERLSB, dwLBA >> 8 );
    // 실린더 MSB 레지스터(포트 0x1F5 또는 0x175)에 읽을 섹터 위치(LBA 16~23비트)를 전송
    kOutPortByte( wPortBase + HDD_PORT_INDEX_CYLINDERMSB, dwLBA >> 16 );
    // 드라이브와 헤드 데이터 설정
    if( bMaster == TRUE )
    {
        bDriveFlag = HDD_DRIVEANDHEAD_LBA;
    }
    else
    {
        bDriveFlag = HDD_DRIVEANDHEAD_LBA | HDD_DRIVEANDHEAD_SLAVE;
    }
    // 드라이브/헤드 레지스터(포트 0x1F6 또는 0x176)에 읽을 섹터의 위치(LBA 24~27비트)와
    // 설정된 값을 같이 전송
    kOutPortByte( wPortBase + HDD_PORT_INDEX_DRIVEANDHEAD, bDriveFlag | ( (dwLBA
            >> 24 ) & 0x0F ) );

    //==========================================================================
    //  커맨드 전송
    //==========================================================================
    // 커맨드를 받아들일 준비가 될 때까지 일정 시간 동안 대기
    if( kWaitForHDDReady( bPrimary ) == FALSE )
    {
        // 동기화 처리
        kUnlockMutex( &( gs_stHDDManager.stMutex ) );
        return FALSE;
    }

    // 인터럽트 플래그를 초기화
    kSetHDDInterruptFlag( bPrimary, FALSE );
    
    // 커맨드 레지스터(포트 0x1F7 또는 0x177)에 읽기(0x20)을 전송
    kOutPortByte( wPortBase + HDD_PORT_INDEX_COMMAND, HDD_COMMAND_READ );

    //==========================================================================
    //  인터럽트 대기 후, 데이터 수신
    //==========================================================================
    // 섹터 수만큼 루프를 돌면서 데이터 수신
    for( i = 0 ; i < iSectorCount ; i++ )
    {
        // 에러가 발생하면 종료
        bStatus = kReadHDDStatus( bPrimary );
        if( ( bStatus & HDD_STATUS_ERROR ) == HDD_STATUS_ERROR )
        {
            kPrintf( "Error Occur\n" );
            // 동기화 처리
            kUnlockMutex( &( gs_stHDDManager.stMutex ) );
            return i;
        }

        // DATAREQUEST 비트가 설정되지 않았으면 데이터가 수신되길 기다림
        if( ( bStatus & HDD_STATUS_DATAREQUEST ) != HDD_STATUS_DATAREQUEST )
        {
            // 처리가 완료될 때까지 일정 시간 동안 인터럽트를 기다림
            bWaitResult = kWaitForHDDInterrupt( bPrimary );
            kSetHDDInterruptFlag( bPrimary, FALSE );
            // 인터럽트가 발생하지 않으면 문제가 발생한 것이므로 종료
            if( bWaitResult == FALSE )
            {
                kPrintf( "Interrupt Not Occur\n" );
                // 동기화 처리
                kUnlockMutex( &( gs_stHDDManager.stMutex ) );
                return FALSE;
            }
        }
        
        // 한 섹터를 읽음
        for( j = 0 ; j < 512 / 2 ; j++ )
        {
            ( ( WORD* ) pcBuffer )[ lReadCount++ ]
                    = kInPortWord( wPortBase + HDD_PORT_INDEX_DATA );
        }
    }

    // 동기화 처리
    kUnlockMutex( &( gs_stHDDManager.stMutex ) );
    return i;
    */
   return 0;
}

/**
 *  하드 디스크에 섹터를 씀
 *      최대 256개의 섹터를 쓸 수 있음
 *      실제로 쓴 섹터 수를 반환
 */
int kWriteHDDSector(int portnum , DWORD dwLBA, int iSectorCount,
        char* pcBuffer)
{
    /*
    WORD wPortBase;
    WORD wTemp;
    int i, j;
    BYTE bDriveFlag;
    BYTE bStatus;
    long lReadCount = 0;
    BOOL bWaitResult;

    // 범위 검사. 최대 256 섹터를 처리할 수 있음
    if( ( gs_stHDDManager.bCanWrite == FALSE ) || 
        ( iSectorCount <= 0 ) || ( 256 < iSectorCount ) ||
        ( ( dwLBA + iSectorCount ) >= gs_stHDDManager.stHDDInformation.dwTotalSectors ) )

    {
        return 0;
    }

    // PATA 포트에 따라서 I/O 포트의 기본 어드레스를 설정
    if( bPrimary == TRUE )
    {
        // 첫 번째 PATA 포트이면 포트 0x1F0을 저장
        wPortBase = HDD_PORT_PRIMARYBASE;
    }
    else
    {
        // 두 번째 PATA 포트이면 포트 0x170을 저장
        wPortBase = HDD_PORT_SECONDARYBASE;
    }

    // 아직 수행 중인 커맨드가 있다면 일정 시간 동안 끝날 때까지 대기
    if( kWaitForHDDNoBusy( bPrimary ) == FALSE )
    {
        return FALSE;
    }

    // 동기화 처리
    kLockMutex( &(gs_stHDDManager.stMutex ) );

    //==========================================================================
    //  데이터 레지스터 설정
    //      LBA 모드의 경우, 섹터 번호 -> 실린더 번호 -> 헤드 번호의 순으로 
    //      LBA 어드레스를 대입
    //==========================================================================
    // 섹터 수 레지스터(포트 0x1F2 또는 0x172)에 쓸 섹터 수를 전송
    kOutPortByte( wPortBase + HDD_PORT_INDEX_SECTORCOUNT, iSectorCount );
    // 섹터 번호 레지스터(포트 0x1F3 또는 0x173)에 쓸 섹터 위치(LBA 0~7비트)를 전송
    kOutPortByte( wPortBase + HDD_PORT_INDEX_SECTORNUMBER, dwLBA );
    // 실린더 LSB 레지스터(포트 0x1F4 또는 0x174)에 쓸 섹터 위치(LBA 8~15비트)를 전송
    kOutPortByte( wPortBase + HDD_PORT_INDEX_CYLINDERLSB, dwLBA >> 8 );
    // 실린더 MSB 레지스터(포트 0x1F5 또는 0x175)에 쓸 섹터 위치(LBA 16~23비트)를 전송
    kOutPortByte( wPortBase + HDD_PORT_INDEX_CYLINDERMSB, dwLBA >> 16 );
    // 드라이브와 헤드 데이터 설정
    if( bMaster == TRUE )
    {
        bDriveFlag = HDD_DRIVEANDHEAD_LBA;
    }
    else
    {
        bDriveFlag = HDD_DRIVEANDHEAD_LBA | HDD_DRIVEANDHEAD_SLAVE;
    }
    // 드라이브/헤드 레지스터(포트 0x1F6 또는 0x176)에 쓸 섹터의 위치(LBA 24~27비트)와
    // 설정된 값을 같이 전송
    kOutPortByte(wPortBase + HDD_PORT_INDEX_DRIVEANDHEAD, bDriveFlag | ( (dwLBA
            >> 24 ) & 0x0F ) );

    //==========================================================================
    //  커맨드 전송 후, 데이터 송신이 가능할 때까지 대기
    //==========================================================================
    // 커맨드를 받아들일 준비가 될 때까지 일정 시간 동안 대기
    if( kWaitForHDDReady( bPrimary ) == FALSE )
    {
        // 동기화 처리
        kUnlockMutex( &( gs_stHDDManager.stMutex ) );
        return FALSE;
    }

    // 커맨드 전송
    kOutPortByte( wPortBase + HDD_PORT_INDEX_COMMAND, HDD_COMMAND_WRITE );
    
    // 데이터 송신이 가능할 때까지 대기
    while( 1 )
    {
        bStatus = kReadHDDStatus( bPrimary );
        // 에러가 발생하면 종료
        if( ( bStatus & HDD_STATUS_ERROR ) == HDD_STATUS_ERROR )
        {
            // 동기화 처리
            kUnlockMutex( &(gs_stHDDManager.stMutex ) );
            return 0;
        }
        
        // Data Request비트가 설정되었다면 데이터 송신 가능
        if( ( bStatus & HDD_STATUS_DATAREQUEST ) == HDD_STATUS_DATAREQUEST )
        {
            break;
        }
        
        kSleep( 1 );        
    }

    //==========================================================================
    //  데이터 송신 후, 인터럽트 대기
    //==========================================================================
    // 섹터 수만큼 루프를 돌면서 데이터 송신
    for( i = 0 ; i < iSectorCount ; i++)
    {
        // 인터럽트 플래그를 초기화하고 한 섹터를 씀
        kSetHDDInterruptFlag( bPrimary, FALSE );
        for( j = 0 ; j < 512 / 2 ; j++ )
        {
            kOutPortWord( wPortBase + HDD_PORT_INDEX_DATA, 
                         ( ( WORD* ) pcBuffer )[ lReadCount++ ]);
        }
        
        // 에러가 발생하면 종료
        bStatus = kReadHDDStatus( bPrimary );
        if( ( bStatus & HDD_STATUS_ERROR ) == HDD_STATUS_ERROR )
        {
            // 동기화 처리
            kUnlockMutex( &(gs_stHDDManager.stMutex ) );
            return i;
        }
        
        // DATAREQUEST 비트가 설정되지 않았으면 데이터가 처리가 완료되길 기다림
        if( ( bStatus & HDD_STATUS_DATAREQUEST ) != HDD_STATUS_DATAREQUEST )
        {
            // 처리가 완료될 때까지 일정 시간 동안 인터럽트를 기다림
            bWaitResult = kWaitForHDDInterrupt( bPrimary );
            kSetHDDInterruptFlag( bPrimary, FALSE );
            // 인터럽트가 발생하지 않으면 문제가 발생한 것이므로 종료
            if( bWaitResult == FALSE )
            {
                // 동기화 처리
                kUnlockMutex( &( gs_stHDDManager.stMutex ) );
                return FALSE;
            }
        }        
    }
    
    // 동기화 처리
    kUnlockMutex( &(gs_stHDDManager.stMutex ) );
    return i;
    */
   return 0;
}
