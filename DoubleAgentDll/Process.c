/* Includes ******************************************************************/
#include <Windows.h>
#include "Status.h"
#include "Process.h"

/* Public Function Definitions ***********************************************/
DOUBLEAGENT_STATUS PROCESS_Create(IN PCWSTR pcwszExePath)
{
	DOUBLEAGENT_STATUS eStatus = DOUBLEAGENT_STATUS_INVALID_VALUE;
	STARTUPINFOW tStartup = { 0 };
	PROCESS_INFORMATION tProcess = { 0 };

	/* Validates the parameters */
	if (NULL == pcwszExePath)
	{
		DOUBLEAGENT_SET(eStatus, DOUBLEAGENT_STATUS_DOUBLEAGENTDLL_PROCESS_CREATE_INVALID_PARAMS);
		goto lbl_cleanup;
	}

	/* Creates the process */
	tStartup.cb = sizeof(tStartup);
	if (FALSE == CreateProcessW(pcwszExePath, NULL, NULL, NULL, 0, 0, NULL, NULL, &tStartup, &tProcess))
	{
		DOUBLEAGENT_SET(eStatus, DOUBLEAGENT_STATUS_DOUBLEAGENTDLL_PROCESS_CREATE_CREATEPROCESSW_FAILED);
		goto lbl_cleanup;
	}

	/* Succeeded */
	DOUBLEAGENT_SET(eStatus, DOUBLEAGENT_STATUS_SUCCESS);

lbl_cleanup:
	/* Closes the thread handle */
	if (NULL != tProcess.hThread)
	{
		(VOID)CloseHandle(tProcess.hThread);
		tProcess.hThread = NULL;
	}

	/* Closes the process handle */
	if (NULL != tProcess.hProcess)
	{
		(VOID)CloseHandle(tProcess.hProcess);
		tProcess.hProcess = NULL;
	}

	/* Returns status */
	return eStatus;
}
