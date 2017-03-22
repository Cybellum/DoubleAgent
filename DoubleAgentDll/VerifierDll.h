#pragma once

/* Includes ******************************************************************/
#include <Windows.h>
#include "Status.h"

/* Macros ********************************************************************/
#define VERIFIERDLL_DLL_PROCESS_VERIFIER (0x4)

/* Function Declarations *****************************************************/
/*
 * The default main function for VERIFIERDLL_DLL_PROCESS_VERIFIER
 * The verifier dll should call this function from DllMain when VERIFIERDLL_DLL_PROCESS_VERIFIER == nReason
 */
BOOL VERIFIERDLL_DllMainProcessVerifier(IN PVOID pvReserved);
