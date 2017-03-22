/* Includes ******************************************************************/
#include <Windows.h>
#include "Status.h"

/* Types *********************************************************************/
typedef struct _RTL_VERIFIER_DLL_DESCRIPTOR
{
	PWSTR pwszDllName;
	DWORD dwDllFlags;
	PVOID pvDllAddress;
	PVOID pvDllThunks;
} RTL_VERIFIER_DLL_DESCRIPTOR, *PRTL_VERIFIER_DLL_DESCRIPTOR;

typedef struct _RTL_VERIFIER_PROVIDER_DESCRIPTOR
{
	DWORD dwLength;
	PVOID pvProviderDlls;
	PVOID pvProviderDllLoadCallback;
	PVOID pvProviderDllUnloadCallback;
	PWSTR pwszVerifierImage;
	DWORD dwVerifierFlags;
	DWORD dwVerifierDebug;
	PVOID pvRtlpGetStackTraceAddress;
	PVOID pvRtlpDebugPageHeapCreate;
	PVOID pvRtlpDebugPageHeapDestroy;
	PVOID pvProviderNtdllHeapFreeCallback;
} RTL_VERIFIER_PROVIDER_DESCRIPTOR, *PRTL_VERIFIER_PROVIDER_DESCRIPTOR;

typedef struct _RTL_VERIFIER_MINILOADATTACH_PROVIDER_DESCRIPTOR
{
	DWORD dwLength;
	DWORD dwReserved;
	DWORD dwReserved1;
	DWORD dwReserved2;
	DWORD dwReserved3;
	DWORD dwReserved4;
	DWORD dwReserved5;
	DWORD dwReserved6;
	PDWORD pdwAVrfDphGlobalFlags;
	PVOID pvAVrfpHeapTable;
	PRTL_VERIFIER_PROVIDER_DESCRIPTOR ptAVrfpProvider;
	CHAR szReserved7[0x18];
} RTL_VERIFIER_MINILOADATTACH_PROVIDER_DESCRIPTOR, *PRTL_VERIFIER_MINILOADATTACH_PROVIDER_DESCRIPTOR;

/* Global Variables **********************************************************/
RTL_VERIFIER_DLL_DESCRIPTOR atDLLs[] = { { 0 } };
RTL_VERIFIER_PROVIDER_DESCRIPTOR tVpd = { sizeof(RTL_VERIFIER_PROVIDER_DESCRIPTOR), atDLLs };

/* Function Definitions ******************************************************/
BOOL VERIFIERDLL_DllMainProcessVerifier(IN PVOID pvReserved)
{
	DOUBLEAGENT_STATUS eStatus = DOUBLEAGENT_STATUS_INVALID_VALUE;

	/* Validates the parameters */
	if (NULL == pvReserved)
	{
		DOUBLEAGENT_SET(eStatus, DOUBLEAGENT_STATUS_DOUBLEAGENTDLL_VERIFIERDLL_DLLMAINPROCESSVERIFIER_INVALID_PARAMS);
		goto lbl_cleanup;
	}

	/* Sets the reserved parameter */
	*((PRTL_VERIFIER_PROVIDER_DESCRIPTOR *)pvReserved) = &tVpd;

	/* Succeeded */
	DOUBLEAGENT_SET(eStatus, DOUBLEAGENT_STATUS_SUCCESS);

lbl_cleanup:
	/* Returns status */
	return FALSE != DOUBLEAGENT_SUCCESS(eStatus);
}
