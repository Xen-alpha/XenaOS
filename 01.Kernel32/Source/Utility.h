/**
 *  file    Utility.h
 *  date    2009/01/17
 *  author  kkamagui 
 *          Copyright(c)2008 All rights reserved by kkamagui
 *  brief   OS���� ����� ��ƿ��Ƽ �Լ��� ���õ� ����
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
