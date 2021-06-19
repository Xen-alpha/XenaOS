/**
 *  file    Types.h
 *  date    2008/12/13
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   유저 레벨에서 사용하는 각종 타입이나 자료구조를 정의한 헤더 파일
 */

#ifndef __TYPES_H__
#define __TYPES_H__

////////////////////////////////////////////////////////////////////////////////
//
// 매크로
//
////////////////////////////////////////////////////////////////////////////////
//==============================================================================
//  기본 타입 관련 매크로
//==============================================================================
#define BYTE    unsigned char
#define WORD    unsigned short
#define DWORD   unsigned int
#define QWORD   unsigned long
#define BOOL    unsigned char

#define TRUE    1
#define FALSE   0
#define NULL    0

//==============================================================================
//  콘솔 관련 매크로
//==============================================================================
// 콘솔의 너비(Width)와 높이(Height),그리고 비디오 메모리의 시작 어드레스 설정
#define CONSOLE_WIDTH                       80
#define CONSOLE_HEIGHT                      25


//==============================================================================
//  키보드에 관련된 매크로
//==============================================================================
// 키 상태에 대한 플래그
#define KEY_FLAGS_UP             0x00
#define KEY_FLAGS_DOWN           0x01
#define KEY_FLAGS_EXTENDEDKEY    0x02

// 스캔 코드 매핑 테이블에 대한 매크로
#define KEY_NONE        0x00
#define KEY_ENTER       '\n'
#define KEY_TAB         '\t'
#define KEY_ESC         0x1B
#define KEY_BACKSPACE   0x08

#define KEY_CTRL        0x81
#define KEY_LSHIFT      0x82
#define KEY_RSHIFT      0x83
#define KEY_PRINTSCREEN 0x84
#define KEY_LALT        0x85
#define KEY_CAPSLOCK    0x86
#define KEY_F1          0x87
#define KEY_F2          0x88
#define KEY_F3          0x89
#define KEY_F4          0x8A
#define KEY_F5          0x8B
#define KEY_F6          0x8C
#define KEY_F7          0x8D
#define KEY_F8          0x8E
#define KEY_F9          0x8F
#define KEY_F10         0x90
#define KEY_NUMLOCK     0x91
#define KEY_SCROLLLOCK  0x92
#define KEY_HOME        0x93
#define KEY_UP          0x94
#define KEY_PAGEUP      0x95
#define KEY_LEFT        0x96
#define KEY_CENTER      0x97
#define KEY_RIGHT       0x98
#define KEY_END         0x99
#define KEY_DOWN        0x9A
#define KEY_PAGEDOWN    0x9B
#define KEY_INS         0x9C
#define KEY_DEL         0x9D
#define KEY_F11         0x9E
#define KEY_F12         0x9F
#define KEY_PAUSE       0xA0

//==============================================================================
//  태스크와 스케줄러 관련 매크로
//==============================================================================
// 태스크의 우선 순위
#define TASK_FLAGS_HIGHEST            0
#define TASK_FLAGS_HIGH               1
#define TASK_FLAGS_MEDIUM             2
#define TASK_FLAGS_LOW                3
#define TASK_FLAGS_LOWEST             4
#define TASK_FLAGS_WAIT               0xFF          

// 태스크의 플래그
#define TASK_FLAGS_ENDTASK            0x8000000000000000
#define TASK_FLAGS_SYSTEM             0x4000000000000000
#define TASK_FLAGS_PROCESS            0x2000000000000000
#define TASK_FLAGS_THREAD             0x1000000000000000
#define TASK_FLAGS_IDLE               0x0800000000000000
#define TASK_FLAGS_USERLEVEL          0x0400000000000000

// 함수 매크로
#define GETPRIORITY( x )            ( ( x ) & 0xFF )
#define SETPRIORITY( x, priority )  ( ( x ) = ( ( x ) & 0xFFFFFFFFFFFFFF00 ) | \
        ( priority ) )
#define GETTCBOFFSET( x )           ( ( x ) & 0xFFFFFFFF )

// 유효하지 않은 태스크 ID
#define TASK_INVALIDID              0xFFFFFFFFFFFFFFFF

// 프로세서 친화도 필드에 아래의 값이 설정되면, 해당 태스크는 특별한 요구사항이 없는 
// 것으로 판단하고 태스크 부하 분산 수행
#define TASK_LOADBALANCINGID        0xFF

//==============================================================================
//  파일 시스템 관련 매크로
//==============================================================================
// 파일 이름의 최대 길이
#define FILESYSTEM_MAXFILENAMELENGTH        24

// SEEK 옵션 정의
#define SEEK_SET                            0
#define SEEK_CUR                            1
#define SEEK_END                            2

// MINT 파일 시스템 타입과 필드를 표준 입출력의 타입으로 재정의
#define size_t      DWORD       
#define dirent      DirectoryEntryStruct
#define d_name      vcFileName


//==============================================================================
//  기타 매크로
//==============================================================================
#define MIN( x, y )     ( ( ( x ) < ( y ) ) ? ( x ) : ( y ) )
#define MAX( x, y )     ( ( ( x ) > ( y ) ) ? ( x ) : ( y ) )

////////////////////////////////////////////////////////////////////////////////
//
// 구조체
//
////////////////////////////////////////////////////////////////////////////////
#pragma pack( push, 1 )

//==============================================================================
//  키보드에 관련된 자료구조
//==============================================================================
// 키 큐에 삽입할 데이터 구조체
typedef struct KeyDataStruct
{
    // 키보드에서 전달된 스캔 코드
    BYTE bScanCode;
    // 스캔 코드를 변환한 ASCII 코드
    BYTE bASCIICode;
    // 키 상태를 저장하는 플래그(눌림/떨어짐/확장 키 여부)
    BYTE bFlags;
} KEYDATA;

//==============================================================================
//  파일 시스템에 관련된 자료구조
//==============================================================================
// 디렉터리 엔트리 자료구조
typedef struct DirectoryEntryStruct
{
    // 파일 이름
    char vcFileName[ FILESYSTEM_MAXFILENAMELENGTH ];
    // 파일의 실제 크기
    DWORD dwFileSize;
    // 파일이 시작하는 클러스터 인덱스
    DWORD dwStartClusterIndex;
} DIRECTORYENTRY;

#pragma pack( pop )

// 파일을 관리하는 파일 핸들 자료구조
typedef struct kFileHandleStruct
{
    // 파일이 존재하는 디렉터리 엔트리의 오프셋
    int iDirectoryEntryOffset;
    // 파일 크기
    DWORD dwFileSize;
    // 파일의 시작 클러스터 인덱스
    DWORD dwStartClusterIndex;
    // 현재 I/O가 수행중인 클러스터의 인덱스
    DWORD dwCurrentClusterIndex;
    // 현재 클러스터의 바로 이전 클러스터의 인덱스
    DWORD dwPreviousClusterIndex;
    // 파일 포인터의 현재 위치
    DWORD dwCurrentOffset;
} FILEHANDLE;

// 디렉터리를 관리하는 디렉터리 핸들 자료구조
typedef struct kDirectoryHandleStruct
{
    // 루트 디렉터리를 저장해둔 버퍼
    DIRECTORYENTRY* pstDirectoryBuffer;
    
    // 디렉터리 포인터의 현재 위치
    int iCurrentOffset;
} DIRECTORYHANDLE;

// 파일과 디렉터리에 대한 정보가 들어있는 자료구조
typedef struct kFileDirectoryHandleStruct
{
    // 자료구조의 타입 설정. 파일 핸들이나 디렉터리 핸들, 또는 빈 핸들 타입 지정 가능
    BYTE bType;

    // bType의 값에 따라 파일 또는 디렉터리로 사용
    union
    {
        // 파일 핸들
        FILEHANDLE stFileHandle;
        // 디렉터리 핸들
        DIRECTORYHANDLE stDirectoryHandle;
    };    
} FILE, DIR;

#endif /*TYPES_H_*/
