/**
 *  file    SystemCallList.h
 *  date    2009/12/13
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   MINT64 OS에서 지원하는 시스템 콜 목록을 정의한 헤더 파일
 */

#ifndef __SYSTEMCALLLIST_H__
#define __SYSTEMCALLLIST_H__

// 콘솔 I/O 관련
#define SYSCALL_CONSOLEPRINTSTRING          0 
#define SYSCALL_SETCURSOR                   1 
#define SYSCALL_GETCURSOR                   2 
#define SYSCALL_CLEARSCREEN                 3 
#define SYSCALL_GETCH                       4 

// 동적 메모리 할당/해제 관련
#define SYSCALL_MALLOC                      5 
#define SYSCALL_FREE                        6 

// 파일과 디렉터리 I/O 관련
#define SYSCALL_FOPEN                       7 
#define SYSCALL_FREAD                       8 
#define SYSCALL_FWRITE                      9 
#define SYSCALL_FSEEK                       10
#define SYSCALL_FCLOSE                      11
#define SYSCALL_REMOVE                      12
#define SYSCALL_OPENDIR                     13
#define SYSCALL_READDIR                     14
#define SYSCALL_REWINDDIR                   15
#define SYSCALL_CLOSEDIR                    16
#define SYSCALL_ISFILEOPENED                17

// 하드 디스크 I/O 관련
#define SYSCALL_READHDDSECTOR               18
#define SYSCALL_WRITEHDDSECTOR              19

// 태스크와 스케줄러 관련
#define SYSCALL_CREATETASK                  20
#define SYSCALL_SCHEDULE                    21
#define SYSCALL_CHANGEPRIORITY              22
#define SYSCALL_ENDTASK                     23
#define SYSCALL_EXITTASK                    24
#define SYSCALL_GETTASKCOUNT                25
#define SYSCALL_ISTASKEXIST                 26
#define SYSCALL_GETPROCESSORLOAD            27
#define SYSCALL_CHANGEPROCESSORAFFINITY     28

// RTC 관련
#define SYSCALL_READRTCTIME                 29
#define SYSCALL_READRTCDATE                 30

// 시리얼 I/O 관련
#define SYSCALL_SENDSERIALDATA              31
#define SYSCALL_RECEIVESERIALDATA           32
#define SYSCALL_CLEARSERIALFIFO             33

// 기타
#define SYSCALL_GETTOTALRAMSIZE             34
#define SYSCALL_GETTICKCOUNT                35
#define SYSCALL_SLEEP                       36

#define SYSCALL_TEST                        0xFFFFFFFF

#endif /*__SYSTEMCALLLIST_H_*/
