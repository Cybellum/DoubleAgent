/* Includes ******************************************************************/
#include <Windows.h>
#include "Status.h"
#include "Process.h"
#include "VerifierDll.h"

/* Function Declarations *****************************************************/
/*
 * The event handler for DLL attach events
 */
static BOOL main_DllMainProcessAttach(VOID);
/*
 * The event handler for DLL detach events
 */
static BOOL main_DllMainProcessDetach(VOID);

/* Function Definitions ******************************************************/
BOOL WINAPI DllMain(IN HINSTANCE hInstDLL, IN SIZE_T nReason, IN PVOID pvReserved)
{
	UNREFERENCED_PARAMETER(hInstDLL);

	/* Process Attach */
	if (VERIFIERDLL_DLL_PROCESS_VERIFIER == nReason)
	{
		return VERIFIERDLL_DllMainProcessVerifier(pvReserved);
	}

	else if (DLL_PROCESS_ATTACH == nReason)
	{
		return main_DllMainProcessAttach();
	}

	/* Process Detach */
	else if (DLL_PROCESS_DETACH == nReason)
	{
		return main_DllMainProcessDetach();
	}

	/* Thread Attach\Detach */
	else
	{
		return TRUE;
	}
}

static BOOL main_DllMainProcessAttach(VOID)
{
	DOUBLEAGENT_STATUS eStatus = DOUBLEAGENT_STATUS_INVALID_VALUE;

	/*
	 **************************************************************************
	 Enter Your Code Here
	 **************************************************************************
	 */

	 /* Sample Code - Launch cmd.exe */
	 //eStatus = PROCESS_Create(L"cmd.exe");
	 //if (FALSE == DOUBLEAGENT_SUCCESS(eStatus))
	 //{
	 //	goto lbl_cleanup;
	 //}

	 /* Succeeded */
	DOUBLEAGENT_SET(eStatus, DOUBLEAGENT_STATUS_SUCCESS);

	//lbl_cleanup:
	/* Returns status */
	return FALSE != DOUBLEAGENT_SUCCESS(eStatus);
}

static BOOL main_DllMainProcessDetach(VOID)
{
	DOUBLEAGENT_STATUS eStatus = DOUBLEAGENT_STATUS_INVALID_VALUE;

	/* Succeeded */
	DOUBLEAGENT_SET(eStatus, DOUBLEAGENT_STATUS_SUCCESS);

	/* Returns status */
	return FALSE != DOUBLEAGENT_SUCCESS(eStatus);
}
