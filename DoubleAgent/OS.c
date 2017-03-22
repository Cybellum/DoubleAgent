/* Includes ******************************************************************/
#include <Windows.h>
#include "Status.h"
#include "OS.h"

/* Public Function Definitions ***********************************************/
DOUBLEAGENT_STATUS OS_GetArchitecture(OUT POS_ARCHITECTURE peOsArchitecture)
{
	DOUBLEAGENT_STATUS eStatus = DOUBLEAGENT_STATUS_INVALID_VALUE;
	OS_ARCHITECTURE eOsArchitectureLocal = OS_ARCHITECTURE_INVALID_VALUE;
#ifndef _WIN64
	BOOL bIsWow64 = FALSE;
#endif

	/* Validates the parameters */
	if (NULL == peOsArchitecture)
	{
		DOUBLEAGENT_SET(eStatus, DOUBLEAGENT_STATUS_DOUBLEAGENT_OS_GETARCHITECTURE_INVALID_PARAMS);
		goto lbl_cleanup;
	}

#ifdef _WIN64
	/* The program is executing 64 bit code, the OS must be 64 bit */
	eOsArchitectureLocal = OS_ARCHITECTURE_X64;
#else
	/* The program is executing 32 bit code, checks the OS architecture */
	if (FALSE == IsWow64Process(GetCurrentProcess(), &bIsWow64))
	{
		DOUBLEAGENT_SET(eStatus, DOUBLEAGENT_STATUS_DOUBLEAGENT_OS_GETARCHITECTURE_ISWOW64PROCESS_FAILED);
		goto lbl_cleanup;
	}

	if (FALSE != bIsWow64)
	{
		/* The process is running under WOW64, the OS is 64 bit */
		eOsArchitectureLocal = OS_ARCHITECTURE_X64;
	}
	else
	{
		/* The process is not running under WOW64, the OS is 32 bit */
		eOsArchitectureLocal = OS_ARCHITECTURE_X86;
	}
#endif

	/* Sets the received parameters */
	*peOsArchitecture = eOsArchitectureLocal;

	/* Succeeded */
	DOUBLEAGENT_SET(eStatus, DOUBLEAGENT_STATUS_SUCCESS);

lbl_cleanup:
	/* Returns status */
	return eStatus;
}
