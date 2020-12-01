/**
 *  file    Synchronization.h
 *  date    2009/03/13
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui 
 *  brief   동기화를 처리하는 함수에 관련된 파일
 */

#ifndef __SYNCHRONIZATION_H__
#define __SYNCHRONIZATION_H__

#include "Types.h"

////////////////////////////////////////////////////////////////////////////////
//
// 구조체
//
////////////////////////////////////////////////////////////////////////////////
// 1바이트로 정렬
#pragma pack( push, 1 )

// 뮤텍스 자료구조
typedef struct kMutexStruct
{
    // 태스크 ID와 잠금을 수행한 횟수
    volatile QWORD qwTaskID;
    volatile DWORD dwLockCount;

    // 잠금 플래그
    volatile BOOL bLockFlag;
    
    // 자료구조의 크기를 8바이트 단위로 맞추려고 추가한 필드
    BYTE vbPadding[ 3 ];
} MUTEX;

// 세마포어 자료구조
typedef struct kSemaphoreStruct
{
    // 태스크 ID와 잠금을 수행한 횟수
    volatile DWORD dwLockCount;

    // 잠금 경계
    volatile DWORD dwInitialCount;
    
    // 자료구조의 크기를 8바이트 단위로 맞추려고 추가한 필드
    // BYTE vbPadding[  ];
} SEMAPHORE;

#pragma pack( pop )

////////////////////////////////////////////////////////////////////////////////
//
// 함수
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
