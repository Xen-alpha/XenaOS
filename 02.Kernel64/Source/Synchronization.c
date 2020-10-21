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
    // 이미 잠겨 있다면 내가 잠갔는지 확인하고 잠근 횟수를 증가시킨 뒤 종료
    if( kTestAndSet(&( pstMutex->bLockFlag ), 0, 1 ) == FALSE )
    {
        // 자신이 잠갔다면 횟수만 증가시킴
        if( pstMutex->qwTaskID == kGetRunningTask()->stLink.qwID ) 
        {
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
    pstMutex->qwTaskID = kGetRunningTask()->stLink.qwID;
}

/**
 *  태스크 사이에서 사용하는 데이터를 위한 잠금 해제 함수
 */
void kUnlockMutex( MUTEX* pstMutex )
{
    // 뮤텍스를 잠근 태스크가 아니면 실패
    if( ( pstMutex->bLockFlag == FALSE ) ||
        ( pstMutex->qwTaskID != kGetRunningTask()->stLink.qwID ) )
    {
        return ;
    }
    
    // 뮤텍스를 중복으로 잠갔으면 잠긴 횟수만 감소
    if( pstMutex->dwLockCount > 1 )
    {
        pstMutex->dwLockCount--;
        return ;
    }
    
    // 해제된 것으로 설정, 잠김 플래그를 가장 나중에 해제해야 함
    pstMutex->qwTaskID = TASK_INVALIDID;
    pstMutex->dwLockCount = 0;
    pstMutex->bLockFlag = FALSE;
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
    // 카운터가 양수라면 카운터를 감소시킨 뒤 종료
    // 카운터가 0 이하면 잠긴 것이 해제될 때까지 대기 - 잠근 것이 자기 자신일 경우엔 교착상태에 빠짐
    while( pstSemaphore->dwLockCount <= 0)
    {
        kSchedule();
    }
    pstSemaphore->dwLockCount--;
    return;
}

/**
 *  태스크 사이에서 사용하는 데이터를 위한 잠금 해제 함수
 */
int xUnlockSemaphore( SEMAPHORE* pstSemaphore )
{
    // 최대 반환 가능값 밑이면 카운터만 증가하고 정상 종료
    if( pstSemaphore->dwLockCount <  pstSemaphore->dwInitialCount)
    {
        pstSemaphore->dwLockCount++;
        return 0;
    }
    //최대 반환값과 같거나 클 경우 오류 값(음수) 출력
    return -1;
}

int xGetSemaphoreValue(SEMAPHORE* pstSemaphore){
    return pstSemaphore->dwLockCount;
}
