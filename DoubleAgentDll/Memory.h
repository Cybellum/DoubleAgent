#pragma once

/* Includes ******************************************************************/
#include <Windows.h>
#include "Status.h"

/* Function Declarations *****************************************************/
/*
 * Override the compiler memset intrinsic functions
 * This may be used to solve linkage problems with the CRT
 */
#pragma function(memset)
void* memset(void *pvBuffer, int iValue, size_t nBufferLen);
