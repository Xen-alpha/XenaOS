/**
 *  file    Synchronization.c
 *  date    2009/03/13
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui 
 *  brief   ����ȭ�� ó���ϴ� �Լ��� ���õ� ����
 */

#include "Synchronization.h"
#include "Utility.h"
#include "Task.h"
#include "Console.h"

/**
 *  �ý��� �������� ����ϴ� �����͸� ���� ��� �Լ�
 */
BOOL kLockForSystemData( void )
{
    return kSetInterruptFlag( FALSE );
}

/**
 *  �ý��� �������� ����ϴ� �����͸� ���� ��� ���� �Լ�
 */
void kUnlockForSystemData( BOOL bInterruptFlag )
{
    kSetInterruptFlag( bInterruptFlag );
}

/**
 *  ���ؽ��� �ʱ�ȭ 
 */
void kInitializeMutex( MUTEX* pstMutex )
{
    // ��� �÷��׿� Ƚ��, �׸��� �½�ũ ID�� �ʱ�ȭ
    pstMutex->bLockFlag = FALSE;
    pstMutex->dwLockCount = 0;
    pstMutex->qwTaskID = TASK_INVALIDID;
}

/**
 *  �½�ũ ���̿��� ����ϴ� �����͸� ���� ��� �Լ�
 */
void kLockMutex( MUTEX* pstMutex )
{
    // �̹� ��� �ִٸ� ���� �ᰬ���� Ȯ���ϰ� ��� Ƚ���� ������Ų �� ����
    if( kTestAndSet(&( pstMutex->bLockFlag ), 0, 1 ) == FALSE )
    {
        // �ڽ��� �ᰬ�ٸ� Ƚ���� ������Ŵ
        if( pstMutex->qwTaskID == kGetRunningTask()->stLink.qwID ) 
        {
            pstMutex->dwLockCount++;
            return ;
        }

        // �ڽ��� �ƴ� ���� ��� ���� ������ ������ ���
        while( kTestAndSet( &( pstMutex->bLockFlag ), 0, 1 ) == FALSE )
        {
            kSchedule();
        }
    }
       
    // ��� ����, ��� �÷��״� ���� kTestAndSet() �Լ����� ó����
    pstMutex->dwLockCount = 1;
    pstMutex->qwTaskID = kGetRunningTask()->stLink.qwID;
}

/**
 *  �½�ũ ���̿��� ����ϴ� �����͸� ���� ��� ���� �Լ�
 */
void kUnlockMutex( MUTEX* pstMutex )
{
    // ���ؽ��� ��� �½�ũ�� �ƴϸ� ����
    if( ( pstMutex->bLockFlag == FALSE ) ||
        ( pstMutex->qwTaskID != kGetRunningTask()->stLink.qwID ) )
    {
        return ;
    }
    
    // ���ؽ��� �ߺ����� �ᰬ���� ��� Ƚ���� ����
    if( pstMutex->dwLockCount > 1 )
    {
        pstMutex->dwLockCount--;
        return ;
    }
    
    // ������ ������ ����, ��� �÷��׸� ���� ���߿� �����ؾ� ��
    pstMutex->qwTaskID = TASK_INVALIDID;
    pstMutex->dwLockCount = 0;
    pstMutex->bLockFlag = FALSE;
}

/**
 *  ������� �ʱ�ȭ 
 */
int xInitializeSemaphore( SEMAPHORE* pstSemaphore, int value)
{
    if (value <= 0) return -1;
    // ��� Ƚ���� �ʱ�ȭ
    pstSemaphore->dwInitialCount = value;
    pstSemaphore->dwLockCount = value;
    return 0;
}

/**
 *  �½�ũ ���̿��� ����ϴ� �����͸� ���� ��� �Լ�
 */
void xLockSemaphore( SEMAPHORE* pstSemaphore )
{
    // ī���Ͱ� ������ ī���͸� ���ҽ�Ų �� ����
    // ī���Ͱ� 0 ���ϸ� ��� ���� ������ ������ ��� - ��� ���� �ڱ� �ڽ��� ��쿣 �������¿� ����
    while( pstSemaphore->dwLockCount <= 0)
    {
        kSchedule();
    }
    pstSemaphore->dwLockCount--;
    return;
}

/**
 *  �½�ũ ���̿��� ����ϴ� �����͸� ���� ��� ���� �Լ�
 */
int xUnlockSemaphore( SEMAPHORE* pstSemaphore )
{
    // �ִ� ��ȯ ���ɰ� ���̸� ī���͸� �����ϰ� ���� ����
    if( pstSemaphore->dwLockCount <  pstSemaphore->dwInitialCount)
    {
        pstSemaphore->dwLockCount++;
        return 0;
    }
    //�ִ� ��ȯ���� ���ų� Ŭ ��� ���� ��(����) ���
    return -1;
}

int xGetSemaphoreValue(SEMAPHORE* pstSemaphore){
    return pstSemaphore->dwLockCount;
}
