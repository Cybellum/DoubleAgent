#pragma once

/* Includes ******************************************************************/
#include <Windows.h>
#include "Status.h"

/* Function Declarations *****************************************************/
/*
 * Receives an executable path and executes it
 */
DOUBLEAGENT_STATUS PROCESS_Create(IN PCWSTR pcwszExePath);
