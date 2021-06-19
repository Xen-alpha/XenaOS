/**
 *  file    Utility.h
 *  date    2009/01/17
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   OS에서 사용할 유틸리티 함수에 관련된 파일
 */

#ifndef __UTILITY_H__
#define __UTILITY_H__

#include "Types.h"

////////////////////////////////////////////////////////////////////////////////
//
//  ???
//
////////////////////////////////////////////////////////////////////////////////
void memset( void* pvDestination, BYTE bData, int iSize );
void *memcpy( void* pvDestination, const void* pvSource, int iSize );
int memcmp( const void* pvDestination, const void* pvSource, int iSize );
void *memmove(void* dest, const void  *src, int iSize);
#endif /*__UTILITY_H__*/
