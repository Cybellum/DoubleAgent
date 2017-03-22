#pragma once

/* Includes ******************************************************************/
#include <Windows.h>
#include "Status.h"

/* Types *********************************************************************/
typedef enum _OS_ARCHITECTURE
{
	OS_ARCHITECTURE_INVALID_VALUE = -1,
	OS_ARCHITECTURE_X86,
	OS_ARCHITECTURE_X64,

	/* Must be last */
	OS_ARCHITECTURE_COUNT
} OS_ARCHITECTURE, *POS_ARCHITECTURE;

/* Function Declarations *****************************************************/
/*
 * Gets the operating system architecture
 */
DOUBLEAGENT_STATUS OS_GetArchitecture(OUT POS_ARCHITECTURE peOsArchitecture);
