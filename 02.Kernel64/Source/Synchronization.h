/**
 *  file    Synchronization.h
 *  date    2009/03/13
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui 
 *  brief   ����ȭ�� ó���ϴ� �Լ��� ���õ� ����
 */

#ifndef __SYNCHRONIZATION_H__
#define __SYNCHRONIZATION_H__

#include "Types.h"

////////////////////////////////////////////////////////////////////////////////
//
// ����ü
//
////////////////////////////////////////////////////////////////////////////////
// 1����Ʈ�� ����
#pragma pack( push, 1 )

// ���ؽ� �ڷᱸ��
typedef struct kMutexStruct
{
    // �½�ũ ID�� ����� ������ Ƚ��
    volatile QWORD qwTaskID;
    volatile DWORD dwLockCount;

    // ��� �÷���
    volatile BOOL bLockFlag;
    
    // �ڷᱸ���� ũ�⸦ 8����Ʈ ������ ���߷��� �߰��� �ʵ�
    BYTE vbPadding[ 3 ];
} MUTEX;

// �������� �ڷᱸ��
typedef struct kSemaphoreStruct
{
    // �½�ũ ID�� ����� ������ Ƚ��
    volatile DWORD dwLockCount;

    // ��� ���
    volatile DWORD dwInitialCount;
    
    // �ڷᱸ���� ũ�⸦ 8����Ʈ ������ ���߷��� �߰��� �ʵ�
    // BYTE vbPadding[  ];
} SEMAPHORE;

#pragma pack( pop )

////////////////////////////////////////////////////////////////////////////////
//
// �Լ�
//
////////////////////////////////////////////////////////////////////////////////
BOOL kLockForSystemData( void );
void kUnlockForSystemData( BOOL bInterruptFlag );

void kInitializeMutex( MUTEX* pstMutex );
void kLockMutex( MUTEX* pstMutex );
void kUnlockMutex( MUTEX* pstMutex );

int xInitializeSemaphore( SEMAPHORE* pstSemaphore, int value);
void xLockSemaphore( SEMAPHORE* pstSemaphore );
int xUnlockSemaphore( SEMAPHORE* pstSemaphore );
int xGetSemaphoreValue(SEMAPHORE* pstSemaphore);

#endif /*__SYNCHRONIZATION_H__*/
