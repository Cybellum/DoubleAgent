/* Includes ******************************************************************/
#include <Windows.h>
#include <stdio.h>
#include <crtdbg.h>
#include "Path.h"
#include "Verifier.h"

/* Macros ********************************************************************/
#define DOUBLEAGENT_ACTION_INSTALL (L"install")
#define DOUBLEAGENT_ACTION_REPAIR (L"repair")
#define DOUBLEAGENT_ACTION_UNINSTALL (L"uninstall")
#define DOUBLEAGENT_VERIFIER_DLL_NAME L"DoubleAgentDll.dll"
#define DOUBLEAGENT_VERIFIER_DLL_RELATIVE_PATH_X86 (L".\\x86\\" ## DOUBLEAGENT_VERIFIER_DLL_NAME)
#define DOUBLEAGENT_VERIFIER_DLL_RELATIVE_PATH_X64 (L".\\x64\\" ## DOUBLEAGENT_VERIFIER_DLL_NAME)

/* Types *********************************************************************/
typedef enum _DOUBLEAGENT_ARGUMENTS
{
	DOUBLEAGENT_ARGUMENTS_INVALID_VALUE = -1,
	DOUBLEAGENT_ARGUMENTS_SELF_PATH,
	DOUBLEAGENT_ARGUMENTS_ACTION_TYPE,
	DOUBLEAGENT_ARGUMENTS_PROCESS_NAME,

	/* Must be last */
	DOUBLEAGENT_ARGUMENTS_COUNT
} DOUBLEAGENT_ARGUMENTS, *PDOUBLEAGENT_ARGUMENTS;

/* Function Declarations *****************************************************/
/*
 * Handles the install action
 */
static DOUBLEAGENT_STATUS main_Install(IN PCWSTR *ppcwszArgv);
/*
 * Handles the repair action
 */
static DOUBLEAGENT_STATUS main_Repair(IN PCWSTR *ppcwszArgv);
/*
 * Handles the uninstall action
 */
static DOUBLEAGENT_STATUS main_Uninstall(IN PCWSTR *ppcwszArgv);

/* Function Definitions ******************************************************/
INT wmain(IN SIZE_T nArgc, IN PCWSTR *ppcwszArgv)
{
	DOUBLEAGENT_STATUS eStatus = DOUBLEAGENT_STATUS_INVALID_VALUE;

	/* Prints usage */
	if (1 == nArgc)
	{
		(VOID)wprintf(L"Usage:\tDoubleAgent.exe install\\uninstall\\repair process_name\n");
		(VOID)wprintf(L"e.g.\tDoubleAgent.exe install cmd.exe\n");
		return 0;
	}

	/* Validates the arguments */
	if (DOUBLEAGENT_ARGUMENTS_COUNT != nArgc)
	{
		DOUBLEAGENT_SET(eStatus, DOUBLEAGENT_STATUS_DOUBLEAGENT_WMAIN_INVALID_ARGS_COUNT);
		goto lbl_cleanup;
	}

	/* Install action */
	if (0 == _wcsicmp(ppcwszArgv[DOUBLEAGENT_ARGUMENTS_ACTION_TYPE], DOUBLEAGENT_ACTION_INSTALL))
	{
		eStatus = main_Install(ppcwszArgv);
		if (FALSE == DOUBLEAGENT_SUCCESS(eStatus))
		{
			goto lbl_cleanup;
		}
	}
	/* Repair action */
	else if (0 == _wcsicmp(ppcwszArgv[DOUBLEAGENT_ARGUMENTS_ACTION_TYPE], DOUBLEAGENT_ACTION_REPAIR))
	{
		eStatus = main_Repair(ppcwszArgv);
		if (FALSE == DOUBLEAGENT_SUCCESS(eStatus))
		{
			goto lbl_cleanup;
		}
	}
	/* Uninstall action */
	else if (0 == _wcsicmp(ppcwszArgv[DOUBLEAGENT_ARGUMENTS_ACTION_TYPE], DOUBLEAGENT_ACTION_UNINSTALL))
	{
		eStatus = main_Uninstall(ppcwszArgv);
		if (FALSE == DOUBLEAGENT_SUCCESS(eStatus))
		{
			goto lbl_cleanup;
		}
	}
	/* Unsupported action */
	else
	{
		DOUBLEAGENT_SET(eStatus, DOUBLEAGENT_STATUS_DOUBLEAGENT_WMAIN_UNSUPPORTED_ACTION);
		goto lbl_cleanup;
	}

	/* Succeeded */
	DOUBLEAGENT_SET(eStatus, DOUBLEAGENT_STATUS_SUCCESS);

lbl_cleanup:
	if (FALSE != DOUBLEAGENT_SUCCESS(eStatus))
	{
		(VOID)wprintf(L"Succeeded");
	}
	else
	{
		(VOID)wprintf(L"Failed (error code %lu)", eStatus);
	}
	/* Returns status */
	return eStatus;
}

static DOUBLEAGENT_STATUS main_Install(IN PCWSTR *ppcwszArgv)
{
	DOUBLEAGENT_STATUS eStatus = DOUBLEAGENT_STATUS_INVALID_VALUE;
	PWSTR pwszExeDirPath = NULL;
	PWSTR pwszVerifierDllPathX86 = NULL;
	PWSTR pwszVerifierDllPathX64 = NULL;

	/* Validates the parameters */
	_ASSERT(NULL != ppcwszArgv);

	/* Gets the executable directory */
	eStatus = PATH_GetDirectory(ppcwszArgv[DOUBLEAGENT_ARGUMENTS_SELF_PATH], &pwszExeDirPath);
	if (FALSE == DOUBLEAGENT_SUCCESS(eStatus))
	{
		goto lbl_cleanup;
	}

	/* Gets the x86 verifier dll path */
	eStatus = PATH_Combine(pwszExeDirPath, DOUBLEAGENT_VERIFIER_DLL_RELATIVE_PATH_X86, &pwszVerifierDllPathX86);
	if (FALSE == DOUBLEAGENT_SUCCESS(eStatus))
	{
		goto lbl_cleanup;
	}

	/* Gets the x64 verifier dll path */
	eStatus = PATH_Combine(pwszExeDirPath, DOUBLEAGENT_VERIFIER_DLL_RELATIVE_PATH_X64, &pwszVerifierDllPathX64);
	if (FALSE == DOUBLEAGENT_SUCCESS(eStatus))
	{
		goto lbl_cleanup;
	}

	/* Installs the application verifier for the process */
	eStatus = VERIFIER_Install(ppcwszArgv[DOUBLEAGENT_ARGUMENTS_PROCESS_NAME], DOUBLEAGENT_VERIFIER_DLL_NAME, pwszVerifierDllPathX86, pwszVerifierDllPathX64);
	if (FALSE == DOUBLEAGENT_SUCCESS(eStatus))
	{
		goto lbl_cleanup;
	}

	/* Succeeded */
	DOUBLEAGENT_SET(eStatus, DOUBLEAGENT_STATUS_SUCCESS);

lbl_cleanup:
	/* Frees the x64 verifier dll path */
	if (NULL != pwszVerifierDllPathX64)
	{
		(VOID)HeapFree(GetProcessHeap(), 0, pwszVerifierDllPathX64);
		pwszVerifierDllPathX64 = NULL;
	}

	/* Frees the x86 verifier dll path */
	if (NULL != pwszVerifierDllPathX86)
	{
		(VOID)HeapFree(GetProcessHeap(), 0, pwszVerifierDllPathX86);
		pwszVerifierDllPathX86 = NULL;
	}

	/* Frees the executable directory */
	if (NULL != pwszExeDirPath)
	{
		(VOID)HeapFree(GetProcessHeap(), 0, pwszExeDirPath);
		pwszExeDirPath = NULL;
	}

	/* Returns status */
	return eStatus;
}

static DOUBLEAGENT_STATUS main_Repair(IN PCWSTR *ppcwszArgv)
{
	UNREFERENCED_PARAMETER(ppcwszArgv);
	DOUBLEAGENT_STATUS eStatus = DOUBLEAGENT_STATUS_INVALID_VALUE;

	/* Validates the parameters */
	_ASSERT(NULL != ppcwszArgv);

	/* Repairs the machine to its original state */
	eStatus = VERIFIER_Repair();
	if (FALSE == DOUBLEAGENT_SUCCESS(eStatus))
	{
		goto lbl_cleanup;
	}

	/* Succeeded */
	DOUBLEAGENT_SET(eStatus, DOUBLEAGENT_STATUS_SUCCESS);

lbl_cleanup:
	/* Returns status */
	return eStatus;
}

static DOUBLEAGENT_STATUS main_Uninstall(IN PCWSTR *ppcwszArgv)
{
	DOUBLEAGENT_STATUS eStatus = DOUBLEAGENT_STATUS_INVALID_VALUE;

	/* Validates the parameters */
	_ASSERT(NULL != ppcwszArgv);

	/* Uninstalls the application verifier from the process */
	VERIFIER_Uninstall(ppcwszArgv[DOUBLEAGENT_ARGUMENTS_PROCESS_NAME], DOUBLEAGENT_VERIFIER_DLL_NAME);

	/* Succeeded */
	DOUBLEAGENT_SET(eStatus, DOUBLEAGENT_STATUS_SUCCESS);

	/* Returns status */
	return eStatus;
}
