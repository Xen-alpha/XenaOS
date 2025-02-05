/**
 *  file    Synchronization.c
 *  date    2009/03/13
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui 
 *  brief   동기화를 처리하는 함수에 관련된 파일
 */

#include "Synchronization.h"
#include "Utility.h"
#include "Task.h"
#include "Console.h"
#include "AssemblyUtility.h"
#include "MultiProcessor.h"

// 스핀락 도입 이전에 사용하던 함수들
#if 0
/**
 *  시스템 전역에서 사용하는 데이터를 위한 잠금 함수
 */
BOOL kLockForSystemData( void )
{
    return kSetInterruptFlag( FALSE );
}

/**
 *  시스템 전역에서 사용하는 데이터를 위한 잠금 해제 함수
 */
void kUnlockForSystemData( BOOL bInterruptFlag )
{
    kSetInterruptFlag( bInterruptFlag );
}
#endif
/**
 *  뮤텍스를 초기화 
 */
void kInitializeMutex( MUTEX* pstMutex )
{
    // 잠김 플래그와 횟수, 그리고 태스크 ID를 초기화
    pstMutex->bLockFlag = FALSE;
    pstMutex->dwLockCount = 0;
    pstMutex->qwTaskID = TASK_INVALIDID;
}

/**
 *  태스크 사이에서 사용하는 데이터를 위한 잠금 함수
 */
void kLockMutex( MUTEX* pstMutex )
{
    BYTE bCurrentAPICID;
    BOOL bInterruptFlag;

    // 인터럽트를 비활성화
    bInterruptFlag = kSetInterruptFlag( FALSE );

    // 현재 코어의 로컬 APIC ID를 확인
    bCurrentAPICID = kGetAPICID();
    
    // 이미 잠겨 있다면 내가 잠갔는지 확인하고 잠근 횟수를 증가시킨 뒤 종료
    if( kTestAndSet(&( pstMutex->bLockFlag ), 0, 1 ) == FALSE )
    {
        // 자신이 잠갔다면 횟수만 증가시킴
        if( pstMutex->qwTaskID == kGetRunningTask( bCurrentAPICID )->stLink.qwID ) 
        {
            // 인터럽트를 복원
            kSetInterruptFlag( bInterruptFlag );
            pstMutex->dwLockCount++;
            return ;
        }
        
        // 자신이 아닌 경우는 잠긴 것이 해제될 때까지 대기
        while( kTestAndSet( &( pstMutex->bLockFlag ), 0, 1 ) == FALSE )
        {
            kSchedule();
        }
    }
       
    // 잠김 설정, 잠김 플래그는 위의 kTestAndSet() 함수에서 처리함
    pstMutex->dwLockCount = 1;
    pstMutex->qwTaskID = kGetRunningTask( bCurrentAPICID )->stLink.qwID;
    // 인터럽트를 복원
    kSetInterruptFlag( bInterruptFlag );
}

/**
 *  태스크 사이에서 사용하는 데이터를 위한 잠금 해제 함수
 */
void kUnlockMutex( MUTEX* pstMutex )
{
    BOOL bInterruptFlag;

    // 인터럽트를 비활성화
    bInterruptFlag = kSetInterruptFlag( FALSE );
    
    // 뮤텍스를 잠근 태스크가 아니면 실패
    if( ( pstMutex->bLockFlag == FALSE ) ||
        ( pstMutex->qwTaskID != kGetRunningTask( kGetAPICID() )->stLink.qwID ) )
    {
        // 인터럽트를 복원
        kSetInterruptFlag( bInterruptFlag );
        return ;
    }
    
    // 뮤텍스를 중복으로 잠갔으면 잠긴 횟수만 감소
    if( pstMutex->dwLockCount > 1 )
    {
        pstMutex->dwLockCount--;
    }
    else
    {
        // 해제된 것으로 설정, 잠김 플래그를 가장 나중에 해제해야 함
        pstMutex->qwTaskID = TASK_INVALIDID;
        pstMutex->dwLockCount = 0;
        pstMutex->bLockFlag = FALSE;
    }
    // 인터럽트를 복원
    kSetInterruptFlag( bInterruptFlag );
}

/**
 *  스핀락을 초기화
 */
void kInitializeSpinLock( SPINLOCK* pstSpinLock )
{
    // 잠김 플래그와 횟수, APIC ID, 인터럽트 플래그를 초기화
    pstSpinLock->bLockFlag = FALSE;
    pstSpinLock->dwLockCount = 0;
    pstSpinLock->bAPICID = 0xFF;
    pstSpinLock->bInterruptFlag = FALSE;
}

/**
 *  시스템 전역에서 사용하는 데이터를 위한 잠금 함수
 */
void kLockForSpinLock( SPINLOCK* pstSpinLock )
{
    BOOL bInterruptFlag;
    
    // 인터럽트를 먼저 비활성화
    bInterruptFlag = kSetInterruptFlag( FALSE );

    // 이미 잠겨 있다면 내가 잠갔는지 확인하고 그렇다면 잠근 횟수를 증가시킨 뒤 종료
    if( kTestAndSet(&( pstSpinLock->bLockFlag ), 0, 1 ) == FALSE )
    {
        // 자신이 잠갔다면 횟수만 증가시킴
        if( pstSpinLock->bAPICID == kGetAPICID() )
        {
            pstSpinLock->dwLockCount++;
            return ;
        }
        
        // 자신이 아닌 경우는 잠긴 것이 해제될 때까지 대기
        while( kTestAndSet( &( pstSpinLock->bLockFlag ), 0, 1 ) == FALSE )
        {
            // kTestAndSet() 함수를 계속 호출하여 메모리 버스가 Lock 되는 것을 방지
            while( pstSpinLock->bLockFlag == TRUE )
            {
                kPause();
            }
        }
    }
    
    // 잠김 설정, 잠김 플래그는 위의 kTestAndSet() 함수에서 처리함
    pstSpinLock->dwLockCount = 1;
    pstSpinLock->bAPICID = kGetAPICID();

    // 인터럽트 플래그를 저장하여 Unlock 수행 시 복원
    pstSpinLock->bInterruptFlag = bInterruptFlag;
}

/**
 *  시스템 전역에서 사용하는 데이터를 위한 잠금 해제 함수
 */
void kUnlockForSpinLock( SPINLOCK* pstSpinLock )
{
    BOOL bInterruptFlag;
    
    // 인터럽트를 먼저 비활성화
    bInterruptFlag = kSetInterruptFlag( FALSE );

    // 스핀락을 잠근 태스크가 아니면 실패
    if( ( pstSpinLock->bLockFlag == FALSE ) ||
        ( pstSpinLock->bAPICID != kGetAPICID() ) )
    {
        kSetInterruptFlag( bInterruptFlag );
        return ;
    }
    
    // 스핀락을 중복으로 잠갔으면 잠긴 횟수만 감소
    if( pstSpinLock->dwLockCount > 1 )
    {
        pstSpinLock->dwLockCount--;
        return ;
    }
    
    // 스핀락을 해제된 것으로 설정하고 인터럽트 플래그를 복원
    // 인터럽트 플래그는 미리 저장해두었다가 사용
    bInterruptFlag = pstSpinLock->bInterruptFlag;    
    pstSpinLock->bAPICID = 0xFF;
    pstSpinLock->dwLockCount = 0;
    pstSpinLock->bInterruptFlag = FALSE;
    pstSpinLock->bLockFlag = FALSE;  
    
    kSetInterruptFlag( bInterruptFlag );
}

/**
 *  세마포어를 초기화 
 */
int xInitializeSemaphore( SEMAPHORE* pstSemaphore, int value)
{
    if (value <= 0) return -1;
    // 잠김 횟수를 초기화
    pstSemaphore->dwInitialCount = value;
    pstSemaphore->dwLockCount = value;
    return 0;
}

/**
 *  태스크 사이에서 사용하는 데이터를 위한 잠금 함수
 */
void xLockSemaphore( SEMAPHORE* pstSemaphore )
{
    BYTE bCurrentAPICID;
    BOOL bInterruptFlag;

    // 인터럽트를 비활성화
    bInterruptFlag = kSetInterruptFlag( FALSE );

    // 현재 코어의 로컬 APIC ID를 확인
    bCurrentAPICID = kGetAPICID();
    // 카운터가 양수라면 카운터를 감소시킨 뒤 종료
    // 카운터가 0 이하면 잠긴 것이 해제될 때까지 대기 - 잠근 것이 자기 자신일 경우엔 교착상태에 빠짐
    while( pstSemaphore->dwLockCount <= 0)
    {
        kSchedule();
    }
    pstSemaphore->dwLockCount--;
    // 인터럽트를 복원
    kSetInterruptFlag( bInterruptFlag );
    return;
}

/**
 *  태스크 사이에서 사용하는 데이터를 위한 잠금 해제 함수
 */
int xUnlockSemaphore( SEMAPHORE* pstSemaphore )
{
    BOOL bInterruptFlag;

    // 인터럽트를 비활성화
    bInterruptFlag = kSetInterruptFlag( FALSE );
    // 최대 반환 가능값 밑이면 카운터만 증가하고 정상 종료
    if( pstSemaphore->dwLockCount <  pstSemaphore->dwInitialCount)
    {
        pstSemaphore->dwLockCount++;
        // 인터럽트를 복원
        kSetInterruptFlag( bInterruptFlag );
        return 0;
    }
    // 인터럽트를 복원
    kSetInterruptFlag( bInterruptFlag );
    //최대 반환값과 같거나 클 경우 오류 값(음수) 출력
    return -1;
}

int xGetSemaphoreValue(SEMAPHORE* pstSemaphore){
    return pstSemaphore->dwLockCount;
}
